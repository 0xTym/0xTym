from __future__ import annotations

from dataclasses import dataclass
from typing import Optional

import numpy as np


@dataclass
class Trade:
    direction: int  # 1=long, -1=short
    entry_idx: int
    exit_idx: Optional[int]
    entry_price: float
    exit_price: Optional[float]
    sl: float
    tp: float
    size: float
    reason: str = ""


@dataclass
class StepInfo:
    idx: int
    timestamp: Optional[str]
    position: int
    equity: float
    balance: float
    peak_equity: float
    drawdown: float
    reward: float
    action: int
    rule_signal: int
    pnl_realized: float
    trade_closed: bool


def as_float32(x: np.ndarray) -> np.ndarray:
    return np.asarray(x, dtype=np.float32)
