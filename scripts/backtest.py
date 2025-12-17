from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.append(str(Path(__file__).resolve().parents[1]))

import pandas as pd
from stable_baselines3 import DQN, PPO

from trading_rl.data import load_ohlcv_csv
from trading_rl.env import EnvConfig, TradingEnv
from trading_rl.features import FeatureConfig, compute_features
from trading_rl.utils import ensure_dir, trades_to_df


def load_model(model_path: str):
    p = Path(model_path)
    if p.name.endswith(".zip"):
        # try PPO then DQN
        try:
            return PPO.load(model_path)
        except Exception:
            return DQN.load(model_path)
    raise ValueError("Model muss .zip sein (stable-baselines3)")


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True)
    ap.add_argument("--timestamp-col", default=None)
    ap.add_argument("--model", required=True)
    ap.add_argument("--out-dir", default="/workspace/runs/backtest")
    args = ap.parse_args()

    out_dir = ensure_dir(args.out_dir)

    ohlcv = load_ohlcv_csv(args.csv, timestamp_col=args.timestamp_col)
    feat = compute_features(ohlcv.df, FeatureConfig())

    env = TradingEnv(feat, env_cfg=EnvConfig(window=64),)
    model = load_model(args.model)

    obs, _ = env.reset()
    done = False
    total_reward = 0.0

    while not done:
        action, _ = model.predict(obs, deterministic=True)
        obs, r, terminated, truncated, info = env.step(int(action))
        total_reward += float(r)
        done = terminated or truncated

    trades_df = trades_to_df(env.trades)
    trades_df.to_csv(out_dir / "trades.csv", index=False)

    summary = {
        "final_equity": env.equity,
        "final_balance": env.balance,
        "n_trades": len(env.trades),
        "total_reward": total_reward,
    }
    pd.Series(summary).to_csv(out_dir / "summary.csv")

    print(summary)
    print(f"Trades: {out_dir / 'trades.csv'}")


if __name__ == "__main__":
    main()
