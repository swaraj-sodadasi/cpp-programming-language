/*
 * =====================================================================================
 * CONCEPT        : Forward Declarations in C++ (Classes, Structs, and Functions)
 * DESCRIPTION    : Comprehensive implementation detailing Forward Declarations:
 *                  1. What is a Forward Declaration? : Informs the compiler about the 
 *                     existence of an entity (class, struct, or function) before its full definition.
 *                  2. Why Use Forward Declarations?  :
 *                     - Breaks circular dependencies (Class A needs B, Class B needs A).
 *                     - Speeds up compilation by avoiding unnecessary `#include` directives.
 *                     - Enables implementation of the Pimpl (Pointer to Implementation) idiom.
 *                  3. When is a Forward Declaration Sufficient? :
 *                     - Declaring pointers or references (`T*`, `T&`, `std::unique_ptr<T>`).
 *                     - Declaring function parameters or return types of type `T`.
 *                  4. When is a Full Definition Required? :
 *                     - Instantiating an object of type `T` by value (compiler needs object size).
 *                     - Accessing members, fields, or methods of type `T`.
 *                     - Inheriting from type `T` (`class A : public T`).
 *                     - Calling `sizeof(T)`.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime; reduces compile-time header dependency trees.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <utility>

using namespace std;

// =====================================================================================
// 1. FORWARD DECLARATION OF FUNCTIONS
// =====================================================================================

// Function Prototype / Forward Declaration
// Informs compiler that 'calculateDiscount' exists before its main() call site.
double calculateDiscount(double originalPrice, double percentage);


// =====================================================================================
// 2. FORWARD DECLARATION OF CLASSES & CIRCULAR DEPENDENCIES
// Breaking mutual references: Customer needs Order, Order needs Customer.
// =====================================================================================

// Forward Declaration of 'Order' class
class Order;

// Class 'Customer' definition
class Customer {
private:
    string name_;
    vector<Order*> orders_; // SAFE: Pointer to incomplete type 'Order' is allowed!

public:
    explicit Customer(string name) : name_(std::move(name)) {}

    [[nodiscard]] const string& getName() const { return name_; }

    // Method prototype taking incomplete type 'Order*'
    void addOrder(Order* order);
    void printCustomerSummary() const;
};

// Class 'Order' definition
class Order {
private:
    int orderId_;
    double amount_;
    Customer* customer_; // SAFE: Pointer to complete type 'Customer'

public:
    Order(int id, double amount, Customer* customer)
        : orderId_(id), amount_(amount), customer_(customer) {
        if (customer_) {
            customer_->addOrder(this); // Can access customer_ methods because Customer is fully defined
        }
    }

    [[nodiscard]] int getId() const { return orderId_; }
    [[nodiscard]] double getAmount() const { return amount_; }
    [[nodiscard]] const Customer* getCustomer() const { return customer_; }
};


// =====================================================================================
// 3. DEFINE METHODS OF CUSTOMER AFTER ORDER IS FULLY DEFINED
// =====================================================================================

void Customer::addOrder(Order* order) {
    if (order) {
        orders_.push_back(order);
    }
}

void Customer::printCustomerSummary() const {
    cout << "  - Customer: " << name_ << " | Total Orders: " << orders_.size() << "\n";
    for (const auto* order : orders_) {
        // Can call order->getAmount() here because Order class definition is now complete
        cout << "    |- Order #" << order->getId() << " -> Amount: $" 
             << fixed << setprecision(2) << order->getAmount() << "\n";
    }
}


// =====================================================================================
// 4. FORWARD DECLARATION VS FULL DEFINITION RULES
// =====================================================================================

// Forward declaration of an incomplete class
class PaymentGateway;

class PaymentManager {
private:
    // Pointer or Reference to incomplete type is LEGAL
    PaymentGateway* gatewayPtr_; 
    
    /* 
     * ILLEGAL (Commented out):
     * PaymentGateway gatewayObj; 
     * COMPILER ERROR: field 'gatewayObj' has incomplete type 'PaymentGateway'
     * (Compiler needs to know sizeof(PaymentGateway) to allocate stack/member layout)
     */

public:
    PaymentManager() : gatewayPtr_(nullptr) {}

    // Parameter as reference to incomplete type is LEGAL in function signatures
    void registerGateway(PaymentGateway& gateway); 
};


// Function implementation declared in Section 1
double calculateDiscount(double originalPrice, double percentage) {
    return originalPrice * (percentage / 100.0);
}


// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string customerNameInput;
    double orderAmountInput = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter Customer Name (e.g., Alice Vance): " << flush;
    getline(cin, customerNameInput);
    if (customerNameInput.empty()) {
        customerNameInput = "Alice Vance";
    }

    cout << "Enter Order Amount in USD (e.g., 249.99): " << flush;
    if (!(cin >> orderAmountInput) || orderAmountInput <= 0.0) {
        cout << "Invalid amount input. Defaulting to 249.99." << endl;
        orderAmountInput = 249.99;
    }

    // 1. DEMONSTRATING FUNCTION FORWARD DECLARATIONS
    cout << "\n================ 1. FUNCTION FORWARD DECLARATION ================" << endl;
    double discount = calculateDiscount(orderAmountInput, 15.0);
    cout << "  - Original Amount : $" << fixed << setprecision(2) << orderAmountInput << "\n";
    cout << "  - 15% Discount    : $" << discount << "\n";
    cout << "  - Final Total     : $" << (orderAmountInput - discount) << "\n";

    // 2. DEMONSTRATING CLASS FORWARD DECLARATIONS & CIRCULAR REFERENCES
    cout << "\n================ 2. CLASS FORWARD DECLARATION & CIRCULAR REFS ================" << endl;
    
    // Creating Customer instance
    Customer customer(customerNameInput);

    // Creating Order instances linking to Customer
    Order order1(5001, orderAmountInput, &customer);
    Order order2(5002, orderAmountInput * 0.5, &customer);

    // Displaying cross-referenced summary
    customer.printCustomerSummary();

    // 3. SUMMARY OF FORWARD DECLARATION RULES
    cout << "\n================ FORWARD DECLARATIONS SUMMARY ================" << endl;
    cout << "+---------------------------------+------------------------+------------------------+\n"
         << "| Operation                       | Forward Decl Allowed?  | Full Definition Req?   |\n"
         << "+---------------------------------+------------------------+------------------------+\n"
         << "| Declare Pointer (`T*`)          | YES                    | NO                     |\n"
         << "| Declare Reference (`T&`)        | YES                    | NO                     |\n"
         << "| Declare Function Parameter      | YES                    | NO                     |\n"
         << "| Declare Member Variable (`T x`) | NO                     | YES (Needs size)       |\n"
         << "| Access Member (`obj.x` / `p->x`)| NO                     | YES (Needs layout)     |\n"
         << "| Inherit Class (`class A : B`)   | NO                     | YES (Needs vtable/size)|\n"
         << "| Call `sizeof(T)`                | NO                     | YES (Needs exact size) |\n"
         << "+---------------------------------+------------------------+------------------------+\n";

    return 0;
}