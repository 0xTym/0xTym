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


def bullish_patterns(df: pd.DataFrame, *, pin_ratio: float = 2.5) -> dict[str, np.ndarray]:
    """Einfache bullish Candlestick-Pattern-Erkennung (symmetrisch zu bearish)."""
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

    # Bullish Pin Bar (Hammer)
    pin = (is_bull) & (lower > body * pin_ratio) & (upper < body * 0.3) & (body > 0)

    prev_bear = np.concatenate([[False], is_bear[:-1]])
    prev_o = np.concatenate([[o[0]], o[:-1]])
    prev_c = np.concatenate([[c[0]], c[:-1]])
    prev_body = np.concatenate([[body[0]], body[:-1]])

    # Bullish Engulfing
    engulf = (is_bull) & prev_bear & (c > prev_o) & (o < prev_c) & (body > prev_body * 0.8)

    # Morning Star (3 candles)
    is_bear_2 = np.concatenate([[False, False], is_bear[:-2]])
    body_2 = np.concatenate([[np.nan, np.nan], body[:-2]])
    rng_2 = np.concatenate([[np.nan, np.nan], rng[:-2]])

    body_1 = np.concatenate([[np.nan], body[:-1]])
    rng_1 = np.concatenate([[np.nan], rng[:-1]])

    mid_2 = np.concatenate([[np.nan, np.nan], ((o[:-2] + c[:-2]) / 2.0)])

    morning = (
        is_bear_2
        & (body_2 > rng_2 * 0.6)
        & (body_1 < rng_1 * 0.3)
        & (is_bull)
        & (body > rng * 0.6)
        & (c > mid_2)
    )

    combined = pin | engulf | morning

    return {
        "bull_pin": pin.astype(np.int8),
        "bull_engulf": engulf.astype(np.int8),
        "morning_star": morning.astype(np.int8),
        "bullish_pattern": combined.astype(np.int8),
    }


@dataclass(frozen=True)
class FeatureConfig:
    swing_len: int = 10
    rsi_len: int = 14
    rsi_threshold: float = 60.0
    rsi_long_threshold: float = 40.0
    atr_len: int = 14
    zone_atr_mult: float = 1.0
    pin_ratio: float = 2.5
    # Multi-step setup detection
    consolidation_len: int = 20
    consolidation_atr_mult: float = 1.25  # range < ATR*mult => consolidation
    breakout_atr_mult: float = 0.15  # breakout beyond range by ATR*mult
    correction_lookahead: int = 25  # bars after breakout to accept correction
    continuation_lookahead: int = 50  # bars after correction/entry to accept continuation


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

    # -------------------------------------------------------------------------
    # Price Action Market Structure (HH/LH/HL/LL) - pivot based
    #
    # - Pivot Highs classified as Higher High (HH) or Lower High (LH)
    # - Pivot Lows classified as Higher Low (HL) or Lower Low (LL)
    #
    # We expose two kinds of signals:
    # - pivot_* flags: only 1 on the pivot bar where it was confirmed
    # - ms_last_* flags: "last confirmed structure state", carried forward
    # -------------------------------------------------------------------------
    last_sh = np.nan  # last swing high value
    prev_sh = np.nan  # previous swing high value
    last_sl = np.nan  # last swing low value
    prev_sl = np.nan  # previous swing low value

    last_sh_arr = np.full(len(out), np.nan)
    prev_sh_arr = np.full(len(out), np.nan)
    last_sl_arr = np.full(len(out), np.nan)
    prev_sl_arr = np.full(len(out), np.nan)

    pivot_hh = np.zeros(len(out), dtype=np.int8)
    pivot_lh = np.zeros(len(out), dtype=np.int8)
    pivot_hl = np.zeros(len(out), dtype=np.int8)
    pivot_ll = np.zeros(len(out), dtype=np.int8)

    ms_last_high_hh = np.zeros(len(out), dtype=np.int8)
    ms_last_high_lh = np.zeros(len(out), dtype=np.int8)
    ms_last_low_hl = np.zeros(len(out), dtype=np.int8)
    ms_last_low_ll = np.zeros(len(out), dtype=np.int8)

    for i in range(len(out)):
        # carry forward last state by default
        if i > 0:
            ms_last_high_hh[i] = ms_last_high_hh[i - 1]
            ms_last_high_lh[i] = ms_last_high_lh[i - 1]
            ms_last_low_hl[i] = ms_last_low_hl[i - 1]
            ms_last_low_ll[i] = ms_last_low_ll[i - 1]

        if np.isfinite(ph[i]):
            prev_sh = last_sh
            last_sh = ph[i]
            if np.isfinite(prev_sh):
                if last_sh > prev_sh:
                    pivot_hh[i] = 1
                    ms_last_high_hh[i] = 1
                    ms_last_high_lh[i] = 0
                else:
                    pivot_lh[i] = 1
                    ms_last_high_hh[i] = 0
                    ms_last_high_lh[i] = 1

        if np.isfinite(pl[i]):
            prev_sl = last_sl
            last_sl = pl[i]
            if np.isfinite(prev_sl):
                if last_sl > prev_sl:
                    pivot_hl[i] = 1
                    ms_last_low_hl[i] = 1
                    ms_last_low_ll[i] = 0
                else:
                    pivot_ll[i] = 1
                    ms_last_low_hl[i] = 0
                    ms_last_low_ll[i] = 1

        last_sh_arr[i] = last_sh
        prev_sh_arr[i] = prev_sh
        last_sl_arr[i] = last_sl
        prev_sl_arr[i] = prev_sl

    out["last_swing_high"] = last_sh_arr
    out["prev_swing_high"] = prev_sh_arr
    out["last_swing_low"] = last_sl_arr
    out["prev_swing_low"] = prev_sl_arr

    out["pivot_hh"] = pivot_hh
    out["pivot_lh"] = pivot_lh
    out["pivot_hl"] = pivot_hl
    out["pivot_ll"] = pivot_ll

    out["ms_last_high_hh"] = ms_last_high_hh
    out["ms_last_high_lh"] = ms_last_high_lh
    out["ms_last_low_hl"] = ms_last_low_hl
    out["ms_last_low_ll"] = ms_last_low_ll
    out["ms_downtrend"] = ((out["ms_last_high_lh"] == 1) & (out["ms_last_low_ll"] == 1)).astype(np.int8)
    out["ms_uptrend"] = ((out["ms_last_high_hh"] == 1) & (out["ms_last_low_hl"] == 1)).astype(np.int8)

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

    # Demand zone around last swing low (ATR-buffered)
    out["demand_top"] = out["last_swing_low"] + zone_buf
    out["demand_bot"] = out["last_swing_low"] - zone_buf
    out["in_demand_zone"] = (
        np.isfinite(out["demand_top"]) & np.isfinite(out["demand_bot"]) & (out["close"] >= out["demand_bot"]) & (out["close"] <= out["demand_top"])
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

    last_pivot_low = np.full(len(out), np.nan)
    cur = np.nan
    for i in range(len(out)):
        if np.isfinite(pl[i]):
            cur = pl[i]
        last_pivot_low[i] = cur
    out["last_pivot_low"] = last_pivot_low
    out["liq_sweep_low"] = (
        np.isfinite(out["last_pivot_low"]) & (out["low"] < out["last_pivot_low"]) & (out["close"] > out["last_pivot_low"])
    ).astype(np.int8)

    # FVG (very simple): gap/imbalance proxies
    high_2 = out["high"].shift(2)
    low_2 = out["low"].shift(2)
    out["fvg_up"] = ((out["low"] > high_2).fillna(False)).astype(np.int8)
    out["fvg_down"] = ((out["high"] < low_2).fillna(False)).astype(np.int8)
    # Backwards compatibility (kept name)
    out["fvg_bear"] = out["fvg_up"]

    pats = bearish_patterns(out, pin_ratio=cfg.pin_ratio)
    for k, v in pats.items():
        out[k] = v

    pats2 = bullish_patterns(out, pin_ratio=cfg.pin_ratio)
    for k, v in pats2.items():
        out[k] = v

    # Rule-based short entry signal (aus deinem Pine abgeleitet)
    out["rule_short"] = (
        (out["is_downtrend"] == 1)
        & (out["in_supply_zone"] == 1)
        & (out["bearish_pattern"] == 1)
        & (out["rsi"] < cfg.rsi_threshold)
    ).astype(np.int8)

    out["rule_long"] = (
        (out["ms_uptrend"] == 1)
        & (out["in_demand_zone"] == 1)
        & (out["bullish_pattern"] == 1)
        & (out["rsi"] > cfg.rsi_long_threshold)
    ).astype(np.int8)

    # Normalize some raw prices into relative features
    out["hl_range"] = (out["high"] - out["low"]) / (out["close"].abs() + 1e-12)
    out["oc_change"] = (out["close"] - out["open"]) / (out["close"].abs() + 1e-12)

    # -------------------------------------------------------------------------
    # Multi-step setup features: Consolidation -> Breakout -> Correction -> Continuation
    # (simple, rule-of-thumb proxies; meant for RL shaping + state context)
    # -------------------------------------------------------------------------
    n = max(2, int(cfg.consolidation_len))
    atr_f = out["atr"].bfill().fillna(0.0)
    atr_safe = atr_f.mask(atr_f <= 0, 1e-12)

    roll_hi = out["high"].rolling(n, min_periods=n).max()
    roll_lo = out["low"].rolling(n, min_periods=n).min()
    cons_range = (roll_hi - roll_lo).fillna(0.0)
    out["cons_range"] = cons_range
    out["cons_range_atr"] = (cons_range / atr_safe).clip(lower=0.0).fillna(0.0)
    out["is_consolidating"] = ((out["cons_range_atr"] > 0) & (out["cons_range_atr"] <= float(cfg.consolidation_atr_mult))).astype(np.int8)

    # Breakout: close breaks prior consolidation range with small ATR buffer
    prev_roll_hi = roll_hi.shift(1)
    prev_roll_lo = roll_lo.shift(1)
    buf = atr_safe * float(cfg.breakout_atr_mult)

    out["breakout_up"] = (
        (out["is_consolidating"].shift(1).fillna(0).astype(int) == 1)
        & (out["close"] > (prev_roll_hi + buf))
    ).fillna(False).astype(np.int8)
    out["breakout_down"] = (
        (out["is_consolidating"].shift(1).fillna(0).astype(int) == 1)
        & (out["close"] < (prev_roll_lo - buf))
    ).fillna(False).astype(np.int8)

    # Correction zone = retest of breakout level (previous range edge) or demand/supply zone
    # We mark correction-ready when price comes back near breakout edge AND we see pattern.
    corr_buf = atr_safe * 0.25
    out["retest_high_edge"] = ((out["close"] <= (prev_roll_hi + corr_buf)) & (out["close"] >= (prev_roll_hi - corr_buf))).fillna(False).astype(np.int8)
    out["retest_low_edge"] = ((out["close"] <= (prev_roll_lo + corr_buf)) & (out["close"] >= (prev_roll_lo - corr_buf))).fillna(False).astype(np.int8)

    out["correction_long_ready"] = (
        (out["ms_uptrend"] == 1)
        & ((out["retest_high_edge"] == 1) | (out["in_demand_zone"] == 1))
        & (out["bullish_pattern"] == 1)
    ).astype(np.int8)

    out["correction_short_ready"] = (
        (out["ms_downtrend"] == 1)
        & ((out["retest_low_edge"] == 1) | (out["in_supply_zone"] == 1))
        & (out["bearish_pattern"] == 1)
    ).astype(np.int8)

    # Continuation proxy: after a correction-ready, price expands in breakout direction
    # (simple: make new N-bar extreme)
    cont_n = max(5, int(cfg.consolidation_len // 2))
    out["new_high"] = (out["high"] >= out["high"].rolling(cont_n, min_periods=cont_n).max()).fillna(False).astype(np.int8)
    out["new_low"] = (out["low"] <= out["low"].rolling(cont_n, min_periods=cont_n).min()).fillna(False).astype(np.int8)
    out["continuation_up"] = ((out["ms_uptrend"] == 1) & (out["new_high"] == 1)).astype(np.int8)
    out["continuation_down"] = ((out["ms_downtrend"] == 1) & (out["new_low"] == 1)).astype(np.int8)

    return out


DEFAULT_FEATURE_COLUMNS = [
    "ret1",
    "logret1",
    "hl_range",
    "oc_change",
    "atr",
    "rsi",
    "ms_last_high_hh",
    "ms_last_high_lh",
    "ms_last_low_hl",
    "ms_last_low_ll",
    "ms_downtrend",
    "ms_uptrend",
    "is_downtrend",
    "in_demand_zone",
    "in_supply_zone",
    "liq_sweep_high",
    "liq_sweep_low",
    "fvg_bear",
    "fvg_down",
    "bearish_pattern",
    "bullish_pattern",
    "rule_short",
    "rule_long",
    "is_consolidating",
    "breakout_up",
    "breakout_down",
    "correction_long_ready",
    "correction_short_ready",
    "continuation_up",
    "continuation_down",
]
