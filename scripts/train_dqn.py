from __future__ import annotations

import argparse
import sys
from pathlib import Path

# Allow running without installing the package
sys.path.append(str(Path(__file__).resolve().parents[1]))

from stable_baselines3 import DQN
from stable_baselines3.common.env_util import make_vec_env
from stable_baselines3.common.monitor import Monitor

from trading_rl.data import load_ohlcv_csv
from trading_rl.env import EnvConfig, TradingEnv
from trading_rl.features import FeatureConfig, compute_features
from trading_rl.rewards import RewardConfig
from trading_rl.utils import ensure_dir


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", required=True, help="Pfad zur OHLCV-CSV")
    ap.add_argument("--timestamp-col", default=None)
    ap.add_argument("--timesteps", type=int, default=200_000)
    ap.add_argument("--run-dir", default="/workspace/runs/dqn")
    args = ap.parse_args()

    run_dir = ensure_dir(args.run_dir)

    ohlcv = load_ohlcv_csv(args.csv, timestamp_col=args.timestamp_col)
    feat = compute_features(ohlcv.df, FeatureConfig())

    def _make():
        env = TradingEnv(
            feat,
            env_cfg=EnvConfig(window=64, rr=2.0, sl_atr_mult=1.0),
            reward_cfg=RewardConfig(),
        )
        return Monitor(env)

    vec_env = make_vec_env(_make, n_envs=1)

    model = DQN(
        policy="MlpPolicy",
        env=vec_env,
        verbose=1,
        tensorboard_log=str(run_dir / "tb"),
        learning_rate=1e-4,
        buffer_size=200_000,
        learning_starts=10_000,
        batch_size=256,
        gamma=0.995,
        train_freq=4,
        target_update_interval=2_000,
        exploration_fraction=0.2,
        exploration_final_eps=0.02,
    )

    model.learn(total_timesteps=args.timesteps)
    out = run_dir / "model.zip"
    model.save(str(out))
    print(f"Gespeichert: {out}")


if __name__ == "__main__":
    main()
