/*
 * =====================================================================================
 * CONCEPT        : C++ Coding Style Basics and Modern Clean Code Guidelines
 * DESCRIPTION    : Comprehensive implementation illustrating industry-standard C++ 
 *                  style and formatting guidelines:
 *                  1. Naming Conventions : PascalCase for types/classes, camelCase for 
 *                                          functions/variables, UPPER_SNAKE_CASE for 
 *                                          constants, and trailing '_' for member vars.
 *                  2. Const Correctness   : Marking immutable variables and read-only 
 *                                          member functions as 'const'.
 *                  3. Modern Type Safety  : Using 'enum class', 'explicit' constructors, 
 *                                          and 'std::move' with explicit std qualification.
 *                  4. Clean Memory & RAII : Preferring standard containers and smart pointers 
 *                                          over raw pointer manual allocations.
 *                  5. Self-Documenting    : Meaningful variable names and clean Doxygen-style 
 *                                          documentation comments.
 *
 * TIME COMPLEXITY  : Best Case: O(N) — Linear traversal over N order items for total calculation.
 * SPACE COMPLEXITY : Best Case: O(N) — Memory allocation proportional to N dynamic vector entries.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <utility>

using namespace std;

// 1. CONSTANTS NAMING: UPPER_SNAKE_CASE for global constants
constexpr double DEFAULT_TAX_RATE = 0.08; // 8% sales tax

// 2. ENUM NAMING: PascalCase for Scoped Enum, PascalCase for Enumerators
enum class OrderStatus {
    Pending,
    Processing,
    Completed,
    Cancelled
};

// 3. STRUCT / CLASS NAMING: PascalCase
struct OrderItem {
    string itemName;  // camelCase for variable names
    double unitPrice;
    int quantity;

    // Explicit constructor with std::move for efficiency
    OrderItem(string name, double price, int qty)
        : itemName(std::move(name)), unitPrice(price), quantity(qty) {}
};

/**
 * @brief Manages shopping order processing demonstrating clean code and encapsulation.
 */
class OrderProcessor {
private:
    // MEMBER VARIABLES: Trailing underscore '_' prevents shadowing and identifies scope
    vector<OrderItem> items_;
    double taxRate_;
    OrderStatus status_;

public:
    // EXPLICIT CONSTRUCTOR: Prevents implicit type conversions
    explicit OrderProcessor(double taxRate = DEFAULT_TAX_RATE)
        : taxRate_(taxRate), status_(OrderStatus::Pending) {}

    // MUTATOR METHOD: camelCase name, clear parameters
    void addItem(string name, double price, int qty) {
        if (price < 0.0 || qty <= 0) {
            cout << "  [Warning] Invalid item skipped: " << name << endl;
            return;
        }
        items_.emplace_back(std::move(name), price, qty);
    }

    // CONST MEMBER FUNCTION: Marked 'const' because it does not alter object state
    double calculateSubtotal() const noexcept {
        double subtotal = 0.0;
        for (const auto& item : items_) {
            subtotal += item.unitPrice * item.quantity;
        }
        return subtotal;
    }

    // PASS-BY-CONST-REFERENCE: Avoids costly value copies
    double calculateTotalTax(double subtotal) const noexcept {
        return subtotal * taxRate_;
    }

    // CONST METHOD FOR DISPLAYING SUMMARY
    void printOrderSummary() const {
        double subtotal = calculateSubtotal();
        double tax = calculateTotalTax(subtotal);
        double grandTotal = subtotal + tax;

        cout << "\n================ ORDER RECEIPT & SUMMARY ================" << endl;
        cout << left << setw(20) << "Item Name" 
             << setw(12) << "Unit Price" 
             << setw(10) << "Quantity" 
             << "Line Total" << endl;
        cout << string(52, '-') << endl;

        for (const auto& item : items_) {
            double lineTotal = item.unitPrice * item.quantity;
            cout << left << setw(20) << item.itemName 
                 << "$" << setw(11) << fixed << setprecision(2) << item.unitPrice 
                 << setw(10) << item.quantity 
                 << "$" << lineTotal << endl;
        }

        cout << string(52, '-') << endl;
        cout << left << setw(42) << "Subtotal:" << "$" << subtotal << endl;
        cout << left << setw(42) << "Sales Tax:" << "$" << tax << endl;
        cout << left << setw(42) << "Grand Total:" << "$" << grandTotal << endl;
    }
};

int main() {
    int itemQuantity = 0;
    double itemPrice = 0.0;
    string itemName;

    OrderProcessor currentOrder;

    // 4. DYNAMIC INPUT COLLECTION WITH FLUSHING
    cout << "Enter dynamic order item name: " << flush;
    if (!(getline(cin, itemName)) || itemName.empty()) {
        itemName = "Standard Widget";
    }

    cout << "Enter unit price for " << itemName << ": " << flush;
    if (!(cin >> itemPrice) || itemPrice <= 0.0) {
        cout << "Invalid price input. Setting default price $19.99." << endl;
        itemPrice = 19.99;
    }

    cout << "Enter quantity: " << flush;
    if (!(cin >> itemQuantity) || itemQuantity <= 0) {
        cout << "Invalid quantity input. Setting default quantity 1." << endl;
        itemQuantity = 1;
    }

    // Add items demonstrating clean method calls
    currentOrder.addItem(itemName, itemPrice, itemQuantity);
    currentOrder.addItem("Shipping & Handling", 5.50, 1);

    // Display formatted summary
    currentOrder.printOrderSummary();

    // 5. CODING STYLE SUMMARY
    cout << "\n================ KEY C++ STYLE GUIDELINES ================" << endl;
    cout << "1. Naming       : Types (PascalCase), Vars/Funcs (camelCase), Members (m_ or _)" << endl;
    cout << "2. Constness    : Mark non-modifying methods and variables as 'const'" << endl;
    cout << "3. Safety       : Use 'explicit' for constructors and 'enum class' for enums" << endl;
    cout << "4. Efficiency   : Pass objects by 'const Type&' and use 'std::move' where appropriate" << endl;
    cout << "5. Formatting   : Keep consistent indentation, vertical space, and clean comments" << endl;

    return 0;
}