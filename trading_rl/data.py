from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Optional

import pandas as pd


@dataclass(frozen=True)
class OHLCV:
    df: pd.DataFrame  # columns: [timestamp?, open, high, low, close, volume?]


REQUIRED_COLS = {"open", "high", "low", "close"}


def load_ohlcv_csv(
    path: str | Path,
    *,
    timestamp_col: Optional[str] = None,
    tz: Optional[str] = None,
) -> OHLCV:
    p = Path(path)
    df = pd.read_csv(p)

    # Normalize column names
    df.columns = [c.strip().lower() for c in df.columns]

    if timestamp_col is not None:
        tcol = timestamp_col.strip().lower()
        if tcol not in df.columns:
            raise ValueError(f"timestamp_col='{timestamp_col}' not found in CSV columns={list(df.columns)}")
        df.rename(columns={tcol: "timestamp"}, inplace=True)
        df["timestamp"] = pd.to_datetime(df["timestamp"], utc=True, errors="coerce")
        if tz is not None:
            df["timestamp"] = df["timestamp"].dt.tz_convert(tz)

    missing = REQUIRED_COLS - set(df.columns)
    if missing:
        raise ValueError(f"Missing required columns: {sorted(missing)}")

    for c in ["open", "high", "low", "close", "volume"]:
        if c in df.columns:
            df[c] = pd.to_numeric(df[c], errors="coerce")

    df = df.dropna(subset=["open", "high", "low", "close"]).reset_index(drop=True)

    # Basic sanity
    if (df["high"] < df[["open", "close"]].max(axis=1)).any() or (df["low"] > df[["open", "close"]].min(axis=1)).any():
        # allow but warn via exception message for now (keeps skeleton strict)
        raise ValueError("OHLC sanity check failed (high/low inconsistent with open/close)")

    return OHLCV(df=df)
