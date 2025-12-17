### Hi there, I'm Tym 👋

![Profile Views](https://komarev.com/ghpvc/?username=your-username&color=blue)

---

## Trading-KI (Reinforcement Learning) – Grundgerüst

Dieses Repo enthält ein lauffähiges RL-Grundgerüst für Trading:

- **Daten**: CSV (OHLCV)
- **State/Features**: u. a. OHLC-Returns, ATR, RSI, Market-Structure (Pivot-basiert, approx), Liquidity-Sweeps (simple), FVG (simple), Candlestick-Patterns (bearish)
- **Action Space**: `0=Hold`, `1=Buy/Long`, `2=Sell/Short`
- **Reward/Punkte**: Equity-Delta + Reward-Shaping (TP/SL, Overtrading, Drawdown, Regel-Bonus)

### Quickstart

```bash
python3 -m pip install -r requirements.txt

# Smoke-Test (mit Beispiel-CSV)
python3 scripts/smoke_env.py --csv data/sample.csv --timestamp-col timestamp

# PPO trainieren
python3 scripts/train_ppo.py --csv data/sample.csv --timestamp-col timestamp --timesteps 200000 --run-dir runs/ppo_run

# Backtest (Trades + Summary CSV)
python3 scripts/backtest.py --csv data/sample.csv --timestamp-col timestamp --model runs/ppo_run/model.zip --out-dir runs/backtest_run
```

### CSV-Format

Pflicht-Spalten: `open, high, low, close` (optional: `timestamp`, `volume`).

## 🚀 About Me

- 💻 **Tech Stack:** PHP (Yii Framework), JavaScript, HTML, CSS, C++, C#, Python
- 🌍 **Open Source Contributor**
- 📫 How to reach me: [vraquz@icloud.com](mailto:vraquz@icloud.com)

---

## 📊 GitHub Stats

![GitHub Streak](https://streak-stats.demolab.com/?user=0xTym&theme=dark&hide_border=true)

![GitHub Stats](https://github-readme-stats.vercel.app/api?username=0xTym&show_icons=true&theme=dark)

![Top Languages](https://github-readme-stats.vercel.app/api/top-langs/?username=0xTym&layout=compact&theme=dark)

---

## 🌟 Projects

ToDo

---

## 🔗 Connect with Me

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Profile-blue?style=for-the-badge&logo=linkedin)](https://www.linkedin.com/in/your-profile/)
[![Twitter](https://img.shields.io/badge/Twitter-Profile-blue?style=for-the-badge&logo=twitter)](https://twitter.com/your-profile)
[![Website](https://img.shields.io/badge/Website-Portfolio-green?style=for-the-badge)](https://your-website.com)
