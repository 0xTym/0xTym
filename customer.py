from typing import Dict
from product import Product


class Customer:
    def __init__(self, name: str, money: float):
        self.name = name
        self.money = money
        self.cart: Dict[str, int] = {}

    def add_to_cart(self, product: Product, quantity: int):
        if quantity <= 0:
            raise ValueError("Quantity must be positive")
        if product.quantity < quantity:
            raise ValueError("Not enough product in stock")
        self.cart[product.name] = self.cart.get(product.name, 0) + quantity

    def checkout(self, store):
        total = 0
        for item, qty in self.cart.items():
            product = store.get_product(item)
            total += product.price * qty
        if total > self.money:
            raise ValueError("Customer does not have enough money")
        # Process purchase
        for item, qty in self.cart.items():
            product = store.get_product(item)
            product.buy(qty)
        self.money -= total
        store.cash += total
        self.cart.clear()
        return total

    def __str__(self):
        return f"{self.name} - Wallet: ${self.money:.2f}"
