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
    hold_position_penalty: float = 0.001  # small time-cost per bar while in position
    invalid_action_penalty: float = 0.02  # e.g. buy while already long
    flip_penalty: float = 0.02  # closing/flip via opposite action

    rule_bonus: float = 0.10  # action aligned with rule signal
    rule_penalty: float = 0.10  # action against rule signal

    structure_bonus: float = 0.05  # long in uptrend / short in downtrend
    structure_penalty: float = 0.05

    # Multi-step setup shaping (Trend -> Consolidation -> Breakout -> Correction/Entry)
    trend_recognition_bonus: float = 0.02
    consolidation_wait_bonus: float = 0.05
    breakout_patience_bonus: float = 0.05  # don't chase; acknowledge breakout as indication
    correction_entry_bonus: float = 0.20  # enter on correction (in zone + pattern)
    early_entry_penalty: float = 0.10  # entering before correction
    chase_breakout_penalty: float = 0.10  # entering on breakout candle

    # Risk shaping
    drawdown_penalty_scale: float = 1.0  # multiplied by current drawdown fraction

    # Time filters
    trade_outside_session_penalty: float = 0.10
