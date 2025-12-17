from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import numpy as np
import pandas as pd


def _ema(x: np.ndarray, period: int) -> np.ndarray:
    x = np.asarray(x, dtype=np.float64)
    out = np.full_like(x, np.nan, dtype=np.float64)
    alpha = 2.0 / (period + 1.0)
    for i in range(len(x)):
        if i == 0:
            out[i] = x[i]
        else:
            out[i] = alpha * x[i] + (1 - alpha) * out[i - 1]
    return out


def atr(high: np.ndarray, low: np.ndarray, close: np.ndarray, period: int = 14) -> np.ndarray:
    high = np.asarray(high, dtype=np.float64)
    low = np.asarray(low, dtype=np.float64)
    close = np.asarray(close, dtype=np.float64)

    prev_close = np.concatenate([[close[0]], close[:-1]])
    tr = np.maximum(high - low, np.maximum(np.abs(high - prev_close), np.abs(low - prev_close)))
    return _ema(tr, period)


def rsi(close: np.ndarray, period: int = 14) -> np.ndarray:
    close = np.asarray(close, dtype=np.float64)
    diff = np.diff(close, prepend=close[0])
    gain = np.maximum(diff, 0.0)
    loss = np.maximum(-diff, 0.0)

    avg_gain = _ema(gain, period)
    avg_loss = _ema(loss, period)
    rs = np.divide(avg_gain, avg_loss + 1e-12)
    return 100.0 - (100.0 / (1.0 + rs))


def pivots(high: np.ndarray, low: np.ndarray, left: int, right: int) -> tuple[np.ndarray, np.ndarray]:
    """Pivot-high/low ähnlich TradingView: value an Pivot-Bar, sonst NaN."""
    high = np.asarray(high, dtype=np.float64)
    low = np.asarray(low, dtype=np.float64)
    n = len(high)
    ph = np.full(n, np.nan)
    pl = np.full(n, np.nan)

    for i in range(left, n - right):
        window_h = high[i - left : i + right + 1]
        if np.isfinite(high[i]) and high[i] == np.nanmax(window_h):
            ph[i] = high[i]

        window_l = low[i - left : i + right + 1]
        if np.isfinite(low[i]) and low[i] == np.nanmin(window_l):
            pl[i] = low[i]

    return ph, pl


def bearish_patterns(df: pd.DataFrame, *, pin_ratio: float = 2.5) -> dict[str, np.ndarray]:
    o = df["open"].to_numpy(np.float64)
    h = df["high"].to_numpy(np.float64)
    l = df["low"].to_numpy(np.float64)
    c = df["close"].to_numpy(np.float64)

    body = np.abs(c - o)
    upper = h - np.maximum(c, o)
    lower = np.minimum(c, o) - l
    rng = np.maximum(h - l, 1e-12)

    is_bear = c < o
    is_bull = c > o

    pin = (is_bear) & (upper > body * pin_ratio) & (lower < body * 0.3) & (body > 0)

    prev_bull = np.concatenate([[False], is_bull[:-1]])
    prev_o = np.concatenate([[o[0]], o[:-1]])
    prev_c = np.concatenate([[c[0]], c[:-1]])
    prev_body = np.concatenate([[body[0]], body[:-1]])

    engulf = (is_bear) & prev_bull & (c < prev_o) & (o > prev_c) & (body > prev_body * 0.8)

    # Evening star (3 candles)
    is_bull_2 = np.concatenate([[False, False], is_bull[:-2]])
    body_2 = np.concatenate([[np.nan, np.nan], body[:-2]])
    rng_2 = np.concatenate([[np.nan, np.nan], rng[:-2]])

    body_1 = np.concatenate([[np.nan], body[:-1]])
    rng_1 = np.concatenate([[np.nan], rng[:-1]])

    mid_2 = np.concatenate([[np.nan, np.nan], ((o[:-2] + c[:-2]) / 2.0)])

    evening = (
        is_bull_2
        & (body_2 > rng_2 * 0.6)
        & (body_1 < rng_1 * 0.3)
        & (is_bear)
        & (body > rng * 0.6)
        & (c < mid_2)
    )

    combined = pin | engulf | evening

    return {
        "bear_pin": pin.astype(np.int8),
        "bear_engulf": engulf.astype(np.int8),
        "evening_star": evening.astype(np.int8),
        "bearish_pattern": combined.astype(np.int8),
    }


@dataclass(frozen=True)
class FeatureConfig:
    swing_len: int = 10
    rsi_len: int = 14
    rsi_threshold: float = 60.0
    atr_len: int = 14
    zone_atr_mult: float = 1.0
    pin_ratio: float = 2.5


def compute_features(df: pd.DataFrame, cfg: FeatureConfig) -> pd.DataFrame:
    out = df.copy()
    h = out["high"].to_numpy(np.float64)
    l = out["low"].to_numpy(np.float64)
    c = out["close"].to_numpy(np.float64)
    o = out["open"].to_numpy(np.float64)

    out["ret1"] = out["close"].pct_change().fillna(0.0)
    out["logret1"] = np.log(out["close"]).diff().replace([np.inf, -np.inf], 0.0).fillna(0.0)

    out["atr"] = atr(h, l, c, period=cfg.atr_len)
    out["rsi"] = rsi(c, period=cfg.rsi_len)

    # Market structure pivots (approx)
    ph, pl = pivots(h, l, left=cfg.swing_len, right=cfg.swing_len)
    out["pivot_high"] = ph
    out["pivot_low"] = pl

    last_lh = np.nan
    prev_lh = np.nan
    last_ll = np.nan

    last_lh_arr = np.full(len(out), np.nan)
    prev_lh_arr = np.full(len(out), np.nan)
    last_ll_arr = np.full(len(out), np.nan)

    for i in range(len(out)):
        if np.isfinite(ph[i]):
            if (not np.isfinite(last_lh)) or (ph[i] < last_lh):
                prev_lh = last_lh
                last_lh = ph[i]
            else:
                prev_lh = last_lh
        if np.isfinite(pl[i]):
            if (not np.isfinite(last_ll)) or (pl[i] < last_ll):
                last_ll = pl[i]

        last_lh_arr[i] = last_lh
        prev_lh_arr[i] = prev_lh
        last_ll_arr[i] = last_ll

    out["last_lh"] = last_lh_arr
    out["prev_lh"] = prev_lh_arr
    out["last_ll"] = last_ll_arr

    out["is_downtrend"] = (
        np.isfinite(out["last_lh"]) & np.isfinite(out["prev_lh"]) & np.isfinite(out["last_ll"]) & (out["last_lh"] < out["prev_lh"]) & (out["close"] < out["last_lh"])
    ).astype(np.int8)

    # Supply zone around last_lh (ATR-buffered)
    zone_buf = out["atr"].bfill().fillna(0.0) * cfg.zone_atr_mult
    out["zone_top"] = out["last_lh"] + zone_buf
    out["zone_bot"] = out["last_lh"] - zone_buf
    out["in_supply_zone"] = (
        np.isfinite(out["zone_top"]) & np.isfinite(out["zone_bot"]) & (out["close"] >= out["zone_bot"]) & (out["close"] <= out["zone_top"])
    ).astype(np.int8)

    # Liquidity sweep (simple): wick breaks last pivot high then closes back below (bearish sweep)
    last_pivot_high = np.full(len(out), np.nan)
    cur = np.nan
    for i in range(len(out)):
        if np.isfinite(ph[i]):
            cur = ph[i]
        last_pivot_high[i] = cur
    out["last_pivot_high"] = last_pivot_high
    out["liq_sweep_high"] = (
        np.isfinite(out["last_pivot_high"]) & (out["high"] > out["last_pivot_high"]) & (out["close"] < out["last_pivot_high"])
    ).astype(np.int8)

    # FVG (very simple): bearish imbalance when low[i] > high[i-2]
    high_2 = out["high"].shift(2)
    out["fvg_bear"] = ((out["low"] > high_2).fillna(False)).astype(np.int8)

    pats = bearish_patterns(out, pin_ratio=cfg.pin_ratio)
    for k, v in pats.items():
        out[k] = v

    # Rule-based short entry signal (aus deinem Pine abgeleitet)
    out["rule_short"] = (
        (out["is_downtrend"] == 1)
        & (out["in_supply_zone"] == 1)
        & (out["bearish_pattern"] == 1)
        & (out["rsi"] < cfg.rsi_threshold)
    ).astype(np.int8)

    # Normalize some raw prices into relative features
    out["hl_range"] = (out["high"] - out["low"]) / (out["close"].abs() + 1e-12)
    out["oc_change"] = (out["close"] - out["open"]) / (out["close"].abs() + 1e-12)

    return out


DEFAULT_FEATURE_COLUMNS = [
    "ret1",
    "logret1",
    "hl_range",
    "oc_change",
    "atr",
    "rsi",
    "is_downtrend",
    "in_supply_zone",
    "liq_sweep_high",
    "fvg_bear",
    "bearish_pattern",
    "rule_short",
]
