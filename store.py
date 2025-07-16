from typing import Dict
from product import Product


class Store:
    def __init__(self):
        self.inventory: Dict[str, Product] = {}
        self.cash: float = 0.0

    def add_product(self, product: Product):
        self.inventory[product.name] = product

    def get_product(self, name: str) -> Product:
        if name not in self.inventory:
            raise ValueError("Product not found")
        return self.inventory[name]

    def list_products(self):
        for product in self.inventory.values():
            print(product)

    def restock(self, name: str, amount: int):
        product = self.get_product(name)
        product.restock(amount)

    def __str__(self):
        return f"Store cash: ${self.cash:.2f}"
