from product import Product
from customer import Customer
from store import Store


def setup_store():
    store = Store()
    store.add_product(Product("Apple", 1.0, 10))
    store.add_product(Product("Banana", 0.5, 20))
    store.add_product(Product("Chocolate", 2.5, 5))
    return store


def main():
    store = setup_store()
    customer = Customer("Player", 20.0)

    while True:
        print("\n--- Market ---")
        store.list_products()
        print(customer)
        print(store)
        print("1. Add to cart")
        print("2. Checkout")
        print("3. Quit")
        choice = input("Select option: ")
        if choice == "1":
            name = input("Product name: ")
            qty = int(input("Quantity: "))
            try:
                product = store.get_product(name)
                customer.add_to_cart(product, qty)
                print(f"Added {qty} {name}(s) to cart.")
            except Exception as e:
                print(f"Error: {e}")
        elif choice == "2":
            try:
                total = customer.checkout(store)
                print(f"Purchased items for ${total:.2f}")
            except Exception as e:
                print(f"Error: {e}")
        elif choice == "3":
            break
        else:
            print("Invalid option")


if __name__ == "__main__":
    main()
