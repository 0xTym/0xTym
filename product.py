class Product:
    def __init__(self, name: str, price: float, quantity: int):
        self.name = name
        self.price = price
        self.quantity = quantity

    def restock(self, amount: int):
        self.quantity += amount

    def buy(self, amount: int):
        if amount > self.quantity:
            raise ValueError("Not enough stock")
        self.quantity -= amount

    def __str__(self):
        return f"{self.name} - ${self.price:.2f} ({self.quantity} left)"
