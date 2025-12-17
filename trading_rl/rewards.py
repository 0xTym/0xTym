from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class RewardConfig:
    # Base reward: delta equity in "points" (scaled)
    pnl_scale: float = 100.0

    # Shaping
    rr_target: float = 2.0
    tp_bonus: float = 2.0
    sl_penalty: float = 2.0

    # Behavior shaping
    overtrade_penalty: float = 0.05
    rule_bonus: float = 0.10
    rule_penalty: float = 0.10

    # Risk shaping
    drawdown_penalty_scale: float = 1.0  # multiplied by current drawdown fraction

    # Time filters
    trade_outside_session_penalty: float = 0.10
