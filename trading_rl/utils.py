from __future__ import annotations

from pathlib import Path
from typing import Iterable

import pandas as pd

from .types import Trade


def trades_to_df(trades: Iterable[Trade]) -> pd.DataFrame:
    rows = []
    for t in trades:
        rows.append(
            {
                "direction": t.direction,
                "entry_idx": t.entry_idx,
                "exit_idx": t.exit_idx,
                "entry_price": t.entry_price,
                "exit_price": t.exit_price,
                "sl": t.sl,
                "tp": t.tp,
                "size": t.size,
                "reason": t.reason,
            }
        )
    return pd.DataFrame(rows)


def ensure_dir(path: str | Path) -> Path:
    p = Path(path)
    p.mkdir(parents=True, exist_ok=True)
    return p
