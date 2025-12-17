from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Allow running without installing the package
sys.path.append(str(Path(__file__).resolve().parents[1]))

from trading_rl.data import load_ohlcv_csv
from trading_rl.env import TradingEnv
from trading_rl.features import FeatureConfig, compute_features


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--timestamp-col", default=None)
    args = ap.parse_args()

    ohlcv = load_ohlcv_csv(args.csv, timestamp_col=args.timestamp_col)
    feat = compute_features(ohlcv.df, FeatureConfig())
    env = TradingEnv(feat)

    obs, _ = env.reset()
    for _ in range(10):
        obs, r, term, trunc, info = env.step(env.action_space.sample())
        s = info["step"]
        print(
            s,
            f" setup_long={info.get('setup_long')} setup_short={info.get('setup_short')}"
            f" rule_long={info.get('rule_long')} rule_short={info.get('rule_short')}",
        )
        if term or trunc:
            break


if __name__ == "__main__":
    main()
