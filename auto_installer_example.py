import subprocess
import sys

REQUIRED_PACKAGES = [
    "requests",
    "rich",
]

def ensure_packages(packages):
    """Ensure that required packages are installed."""
    for pkg in packages:
        try:
            __import__(pkg)
        except ImportError:
            print(f"Installing {pkg}...")
            subprocess.check_call([sys.executable, "-m", "pip", "install", pkg])


def fetch_cat_fact():
    """Fetch a random cat fact from an API."""
    import requests
    response = requests.get("https://catfact.ninja/fact", timeout=5)
    response.raise_for_status()
    return response.json().get("fact", "No fact found.")


def main():
    ensure_packages(REQUIRED_PACKAGES)
    from rich import print
    from rich.console import Console

    console = Console()
    console.rule("Cat Fact")
    try:
        fact = fetch_cat_fact()
        console.print(f"[bold green]{fact}[/bold green]")
    except Exception as exc:
        console.print(f"[red]Failed to fetch cat fact: {exc}[/red]")


if __name__ == "__main__":
    main()
