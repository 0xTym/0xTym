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
    initial_balance: float = 10_000.0
    commission_per_trade: float = 1.0  # fixed cost per open/close
    sl_atr_mult: float = 1.0
    rr: float = 2.0
    allow_short: bool = True
    allow_long: bool = True

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

        self._features = self.df[self.feature_columns].to_numpy(np.float32)
        self._open = self.df["open"].to_numpy(np.float64)
        self._high = self.df["high"].to_numpy(np.float64)
        self._low = self.df["low"].to_numpy(np.float64)
        self._close = self.df["close"].to_numpy(np.float64)
        self._atr = self.df.get("atr", pd.Series(np.zeros(len(self.df)))).to_numpy(np.float64)
        self._timestamp = self.df["timestamp"].astype(str).to_numpy() if "timestamp" in self.df.columns else None
        self._rule_short = self.df.get("rule_short", pd.Series(np.zeros(len(self.df)))).to_numpy(np.int8)

        obs_dim = self._features.shape[1]
        self.observation_space = gym.spaces.Box(
            low=-np.inf,
            high=np.inf,
            shape=(env_cfg.window, obs_dim),
            dtype=np.float32,
        )
        self.action_space = gym.spaces.Discrete(3)

        # State
        self.idx = 0
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
        obs = self._features[start : i + 1]
        if len(obs) < w:
            pad = np.zeros((w - len(obs), obs.shape[1]), dtype=np.float32)
            obs = np.vstack([pad, obs])
        return as_float32(obs)

    def reset(self, *, seed: Optional[int] = None, options: Optional[dict[str, Any]] = None):
        super().reset(seed=seed)
        self.idx = max(self.env_cfg.window - 1, 0)
        self.position = 0
        self.balance = self.env_cfg.initial_balance
        self.equity = self.env_cfg.initial_balance
        self.peak_equity = self.env_cfg.initial_balance
        self.open_trade = None
        self.trades = []
        self._last_equity = self.env_cfg.initial_balance
        return self._get_obs(), {}

    def _open_position(self, direction: int):
        entry = float(self._close[self.idx])
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

        # Execute action
        if self.position == 0:
            if action == 1 and self.env_cfg.allow_long:
                self._open_position(direction=1)
                reward -= self.reward_cfg.overtrade_penalty
            elif action == 2 and self.env_cfg.allow_short:
                self._open_position(direction=-1)
                reward -= self.reward_cfg.overtrade_penalty

            # Penalize trading outside session
            if action in (1, 2) and not in_session:
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

        # Rule adherence shaping (nur für SHORT Regel aus Pine abgeleitet)
        rule_signal = int(self._rule_short[i])
        if action == 2:
            reward += self.reward_cfg.rule_bonus if rule_signal == 1 else -self.reward_cfg.rule_penalty

        self._last_equity = self.equity

        # Advance time
        self.idx += 1

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

        # Terminate if bankrupt-ish
        if self.equity <= 0:
            terminated = True

        return self._get_obs(), float(reward), terminated, truncated, info

    def render(self):
        print(
            f"idx={self.idx} pos={self.position} bal={self.balance:.2f} eq={self.equity:.2f} trades={len(self.trades)}"
        )
