from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Optional

import gymnasium as gym
import numpy as np
import pandas as pd

from .features import DEFAULT_FEATURE_COLUMNS
from .rewards import RewardConfig
from .types import StepInfo, Trade, as_float32


@dataclass(frozen=True)
class EnvConfig:
    window: int = 64
    episode_len: int = 512  # truncated after N steps (helps training stability)
    random_start: bool = True
    initial_balance: float = 10_000.0
    commission_per_trade: float = 1.0  # fixed cost per open/close
    sl_atr_mult: float = 1.0
    rr: float = 2.0
    allow_short: bool = True
    allow_long: bool = True
    close_on_opposite_action: bool = True  # action opposite -> close/flip
    include_position_features: bool = True
    slippage_bps: float = 0.5  # 0.5 bps = 0.005%
    max_trade_bars: Optional[int] = None  # optional time-stop

    # session filter (optional) - hours in 0..23
    session_start_hour: Optional[int] = None
    session_end_hour: Optional[int] = None


class TradingEnv(gym.Env):
    """Discrete: 0=Hold, 1=Buy/Long, 2=Sell/Short.

    Position is -1/0/+1. Trades have ATR-based SL/TP.
    Reward = Equity delta (scaled) + shaping bonuses/penalties.
    """

    metadata = {"render_modes": ["human"]}

    def __init__(
        self,
        df_feat: pd.DataFrame,
        *,
        feature_columns: Optional[list[str]] = None,
        env_cfg: EnvConfig = EnvConfig(),
        reward_cfg: RewardConfig = RewardConfig(),
    ) -> None:
        super().__init__()

        if feature_columns is None:
            feature_columns = DEFAULT_FEATURE_COLUMNS

        self.df = df_feat.reset_index(drop=True)
        self.feature_columns = feature_columns
        self.env_cfg = env_cfg
        self.reward_cfg = reward_cfg

        self._base_features = self.df[self.feature_columns].to_numpy(np.float32)
        self._open = self.df["open"].to_numpy(np.float64)
        self._high = self.df["high"].to_numpy(np.float64)
        self._low = self.df["low"].to_numpy(np.float64)
        self._close = self.df["close"].to_numpy(np.float64)
        self._atr = self.df.get("atr", pd.Series(np.zeros(len(self.df)))).to_numpy(np.float64)
        self._timestamp = self.df["timestamp"].astype(str).to_numpy() if "timestamp" in self.df.columns else None
        self._rule_short = self.df.get("rule_short", pd.Series(np.zeros(len(self.df)))).to_numpy(np.int8)
        self._rule_long = self.df.get("rule_long", pd.Series(np.zeros(len(self.df)))).to_numpy(np.int8)
        self._ms_uptrend = self.df.get("ms_uptrend", pd.Series(np.zeros(len(self.df)))).to_numpy(np.int8)
        self._ms_downtrend = self.df.get("ms_downtrend", pd.Series(np.zeros(len(self.df)))).to_numpy(np.int8)

        base_dim = self._base_features.shape[1]
        dyn_dim = 5 if self.env_cfg.include_position_features else 0
        obs_dim = base_dim + dyn_dim
        self.observation_space = gym.spaces.Box(
            low=-np.inf,
            high=np.inf,
            shape=(env_cfg.window, obs_dim),
            dtype=np.float32,
        )
        self.action_space = gym.spaces.Discrete(3)

        # State
        self.idx = 0
        self._ep_start_idx = 0
        self._ep_steps = 0
        self.position = 0
        self.balance = env_cfg.initial_balance
        self.equity = env_cfg.initial_balance
        self.peak_equity = env_cfg.initial_balance
        self.open_trade: Optional[Trade] = None
        self.trades: list[Trade] = []
        self._last_equity = env_cfg.initial_balance

    def _in_session(self, i: int) -> bool:
        if self.env_cfg.session_start_hour is None or self.env_cfg.session_end_hour is None:
            return True
        if self._timestamp is None:
            return True
        try:
            ts = pd.to_datetime(self._timestamp[i], utc=True, errors="coerce")
            if pd.isna(ts):
                return True
            hour = int(ts.hour)
        except Exception:
            return True
        start = int(self.env_cfg.session_start_hour)
        end = int(self.env_cfg.session_end_hour)
        if start <= end:
            return start <= hour <= end
        # overnight session
        return hour >= start or hour <= end

    def _get_obs(self) -> np.ndarray:
        w = self.env_cfg.window
        i = self.idx
        start = max(0, i - w + 1)
        base = self._base_features[start : i + 1]
        if len(base) < w:
            pad = np.zeros((w - len(base), base.shape[1]), dtype=np.float32)
            base = np.vstack([pad, base])

        if not self.env_cfg.include_position_features:
            return as_float32(base)

        # Dynamic, trade-aware features (repeated over window)
        dyn = np.zeros((w, 5), dtype=np.float32)
        if self.open_trade is not None:
            t = self.open_trade
            atr = float(self._atr[i])
            atr = atr if np.isfinite(atr) and atr > 0 else 1.0
            px = float(self._close[i])
            unreal = (px - t.entry_price) * t.direction * t.size
            dist_sl = (px - t.sl) * t.direction  # positive if moving away from SL
            dist_tp = (t.tp - px) * t.direction  # positive if TP still ahead
            time_in_trade = max(0, i - t.entry_idx)
            time_frac = 0.0
            if self.env_cfg.max_trade_bars is not None and self.env_cfg.max_trade_bars > 0:
                time_frac = min(1.0, float(time_in_trade) / float(self.env_cfg.max_trade_bars))

            dyn[:, 0] = float(self.position)
            dyn[:, 1] = float(unreal / max(self.env_cfg.initial_balance, 1e-9))
            dyn[:, 2] = float(dist_sl / atr)
            dyn[:, 3] = float(dist_tp / atr)
            dyn[:, 4] = float(time_frac)

        obs = np.concatenate([base, dyn], axis=1)
        return as_float32(obs)

    def reset(self, *, seed: Optional[int] = None, options: Optional[dict[str, Any]] = None):
        super().reset(seed=seed)
        n = len(self.df)
        min_start = max(self.env_cfg.window - 1, 0)
        max_start = max(min_start, n - 2 - max(1, self.env_cfg.episode_len))

        if self.env_cfg.random_start and max_start > min_start:
            self.idx = int(self.np_random.integers(min_start, max_start + 1))
        else:
            self.idx = min_start

        self._ep_start_idx = self.idx
        self._ep_steps = 0
        self.position = 0
        self.balance = self.env_cfg.initial_balance
        self.equity = self.env_cfg.initial_balance
        self.peak_equity = self.env_cfg.initial_balance
        self.open_trade = None
        self.trades = []
        self._last_equity = self.env_cfg.initial_balance
        return self._get_obs(), {}

    def _slip(self) -> float:
        return float(self.env_cfg.slippage_bps) / 10_000.0

    def _open_position(self, direction: int):
        mid = float(self._close[self.idx])
        slip = self._slip()
        entry = mid * (1.0 + slip) if direction == 1 else mid * (1.0 - slip)
        atr = float(self._atr[self.idx])
        risk = max(atr * self.env_cfg.sl_atr_mult, 1e-6)

        if direction == 1:
            sl = entry - risk
            tp = entry + risk * self.env_cfg.rr
        else:
            sl = entry + risk
            tp = entry - risk * self.env_cfg.rr

        self.balance -= self.env_cfg.commission_per_trade
        t = Trade(
            direction=direction,
            entry_idx=self.idx,
            exit_idx=None,
            entry_price=entry,
            exit_price=None,
            sl=float(sl),
            tp=float(tp),
            size=1.0,
        )
        self.open_trade = t
        self.position = direction

    def _close_position(self, exit_price: float, reason: str) -> float:
        assert self.open_trade is not None
        t = self.open_trade
        pnl = (exit_price - t.entry_price) * t.direction * t.size
        self.balance += pnl
        self.balance -= self.env_cfg.commission_per_trade

        closed = Trade(
            direction=t.direction,
            entry_idx=t.entry_idx,
            exit_idx=self.idx,
            entry_price=t.entry_price,
            exit_price=float(exit_price),
            sl=t.sl,
            tp=t.tp,
            size=t.size,
            reason=reason,
        )
        self.trades.append(closed)
        self.open_trade = None
        self.position = 0
        return float(pnl)

    def step(self, action: int):
        # Episode end
        terminated = False
        truncated = False

        i = self.idx
        if i >= len(self.df) - 2:
            terminated = True
            return self._get_obs(), 0.0, terminated, truncated, {}

        reward = 0.0
        pnl_realized = 0.0
        trade_closed = False

        # Optional session shaping
        in_session = self._in_session(i)

        # Execute action / handle flips
        did_trade_action = action in (1, 2)

        if self.position == 0:
            if action == 1 and self.env_cfg.allow_long:
                self._open_position(direction=1)
                reward -= self.reward_cfg.overtrade_penalty
            elif action == 2 and self.env_cfg.allow_short:
                self._open_position(direction=-1)
                reward -= self.reward_cfg.overtrade_penalty
        else:
            # invalid action: same direction again
            if (action == 1 and self.position == 1) or (action == 2 and self.position == -1):
                reward -= self.reward_cfg.invalid_action_penalty

            # flip/close on opposite action
            if self.env_cfg.close_on_opposite_action and self.open_trade is not None:
                if (action == 1 and self.position == -1) or (action == 2 and self.position == 1):
                    slip = self._slip()
                    mid = float(self._close[i])
                    # worst-case exit
                    exit_px = mid * (1.0 + slip) if self.position == -1 else mid * (1.0 - slip)
                    pnl_realized = self._close_position(exit_px, reason="FLIP")
                    trade_closed = True
                    reward -= self.reward_cfg.flip_penalty
                    # open new position in the new direction
                    if action == 1 and self.env_cfg.allow_long:
                        self._open_position(direction=1)
                        reward -= self.reward_cfg.overtrade_penalty
                    elif action == 2 and self.env_cfg.allow_short:
                        self._open_position(direction=-1)
                        reward -= self.reward_cfg.overtrade_penalty

        # Penalize trading outside session
        if did_trade_action and not in_session and (self.position != 0 or trade_closed):
            reward -= self.reward_cfg.trade_outside_session_penalty

        # Manage open trade: check SL/TP hit on next bar range
        if self.open_trade is not None:
            t = self.open_trade
            next_i = i + 1
            hi = float(self._high[next_i])
            lo = float(self._low[next_i])

            hit_tp = False
            hit_sl = False

            if t.direction == 1:
                hit_sl = lo <= t.sl
                hit_tp = hi >= t.tp
            else:
                hit_sl = hi >= t.sl
                hit_tp = lo <= t.tp

            # If both hit in same bar, assume SL first (conservative)
            if hit_sl:
                exit_price = float(t.sl)
                pnl_realized = self._close_position(exit_price, reason="SL")
                trade_closed = True
                reward -= self.reward_cfg.sl_penalty
            elif hit_tp:
                exit_price = float(t.tp)
                pnl_realized = self._close_position(exit_price, reason="TP")
                trade_closed = True
                reward += self.reward_cfg.tp_bonus
            else:
                # time stop
                if self.env_cfg.max_trade_bars is not None:
                    if (self.idx - t.entry_idx) >= int(self.env_cfg.max_trade_bars):
                        slip = self._slip()
                        mid = float(self._close[i])
                        exit_px = mid * (1.0 + slip) if t.direction == -1 else mid * (1.0 - slip)
                        pnl_realized = self._close_position(exit_px, reason="TIME")
                        trade_closed = True

        # Mark-to-market equity
        if self.open_trade is not None:
            t = self.open_trade
            unreal = (float(self._close[i]) - t.entry_price) * t.direction * t.size
            self.equity = self.balance + unreal
        else:
            self.equity = self.balance

        self.peak_equity = max(self.peak_equity, self.equity)
        drawdown = 0.0 if self.peak_equity <= 0 else max(0.0, (self.peak_equity - self.equity) / self.peak_equity)

        # Base reward: equity delta
        reward += (self.equity - self._last_equity) * self.reward_cfg.pnl_scale / max(self.env_cfg.initial_balance, 1e-9)

        # Drawdown shaping
        reward -= drawdown * self.reward_cfg.drawdown_penalty_scale

        # Small time-cost while holding a position (reduces infinite holding)
        if self.open_trade is not None:
            reward -= self.reward_cfg.hold_position_penalty

        # Rule adherence shaping (LONG + SHORT)
        rule_short = int(self._rule_short[i])
        rule_long = int(self._rule_long[i])
        rule_signal = 0
        if action == 2:
            rule_signal = rule_short
            reward += self.reward_cfg.rule_bonus if rule_short == 1 else -self.reward_cfg.rule_penalty
        elif action == 1:
            rule_signal = rule_long
            reward += self.reward_cfg.rule_bonus if rule_long == 1 else -self.reward_cfg.rule_penalty

        # Structure alignment shaping
        if action == 2:
            reward += self.reward_cfg.structure_bonus if int(self._ms_downtrend[i]) == 1 else -self.reward_cfg.structure_penalty
        elif action == 1:
            reward += self.reward_cfg.structure_bonus if int(self._ms_uptrend[i]) == 1 else -self.reward_cfg.structure_penalty

        self._last_equity = self.equity

        # Advance time
        self.idx += 1
        self._ep_steps += 1

        info = {
            "step": StepInfo(
                idx=self.idx,
                timestamp=(str(self._timestamp[self.idx]) if self._timestamp is not None else None),
                position=int(self.position),
                equity=float(self.equity),
                balance=float(self.balance),
                peak_equity=float(self.peak_equity),
                drawdown=float(drawdown),
                reward=float(reward),
                action=int(action),
                rule_signal=rule_signal,
                pnl_realized=float(pnl_realized),
                trade_closed=bool(trade_closed),
            )
        }
        info["rule_short"] = rule_short
        info["rule_long"] = rule_long

        # Terminate if bankrupt-ish
        if self.equity <= 0:
            terminated = True

        # Truncate after episode_len
        if self._ep_steps >= max(1, int(self.env_cfg.episode_len)):
            truncated = True

        return self._get_obs(), float(reward), terminated, truncated, info

    def render(self):
        print(
            f"idx={self.idx} pos={self.position} bal={self.balance:.2f} eq={self.equity:.2f} trades={len(self.trades)}"
        )
