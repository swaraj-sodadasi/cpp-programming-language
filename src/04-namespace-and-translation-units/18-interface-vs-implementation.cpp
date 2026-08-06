/*
 * =====================================================================================
 * CONCEPT        : Interface vs Implementation in C++
 * DESCRIPTION    : Comprehensive program demonstrating the separation of Interface 
 *                  (what a module does) from Implementation (how it does it):
 *                  1. Pure Abstract Interface : Classes containing only pure virtual 
 *                     (`virtual ... = 0`) functions and a virtual destructor. Defines a strict contract.
 *                  2. Concrete Implementations: Derived classes that fulfill the interface 
 *                     contract by overriding virtual methods with specific logic and private data.
 *                  3. Decoupling & Polymorphism: Client code depends strictly on abstract 
 *                     interface types (`IPaymentProcessor`), making it completely agnostic 
 *                     to specific underlying implementations (`CreditCard`, `Crypto`, `PayPal`).
 *                  4. Compilation Isolation   : Decoupling interfaces from implementations 
 *                     prevents cascading recompilation when internal implementation details change.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution; virtual dispatch (vtable lookup) cost is O(1).
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation; per-object vptr pointer overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <iomanip>
#include <utility>

using namespace std;

// =====================================================================================
// 1. THE INTERFACE (ABSTRACT CONTRACT)
// Exposes WHAT the module can do, hiding all implementation details and state variables.
// Rule: Pure virtual methods (= 0) and a virtual destructor (~IPaymentProcessor()).
// =====================================================================================

class IPaymentProcessor {
public:
    virtual ~IPaymentProcessor() = default; // Essential for safe polymorphic deletion

    // Pure virtual methods defining the interface contract
    [[nodiscard]] virtual bool processPayment(double amount) = 0;
    [[nodiscard]] virtual string getProviderName() const = 0;
    virtual void refundPayment(double amount) = 0;
};

// =====================================================================================
// 2. CONCRETE IMPLEMENTATION 1: Credit Card Processor
// Contains specific implementation details (API keys, card numbers, encryption).
// =====================================================================================

class CreditCardPaymentProcessor final : public IPaymentProcessor {
private:
    string maskedCardNumber_;
    string merchantApiKey_;

public:
    CreditCardPaymentProcessor(string cardNumber, string apiKey)
        : maskedCardNumber_(std::move(cardNumber)), merchantApiKey_(std::move(apiKey)) {}

    bool processPayment(double amount) override {
        cout << "  - [CreditCardImpl] Contacting Merchant Gateway with API Key '" << merchantApiKey_ << "'...\n";
        cout << "  - [CreditCardImpl] Charging $" << fixed << setprecision(2) << amount 
             << " to Card: " << maskedCardNumber_ << " -> SUCCESS\n";
        return true;
    }

    [[nodiscard]] string getProviderName() const override {
        return "Visa/Mastercard Credit Gateway";
    }

    void refundPayment(double amount) override {
        cout << "  - [CreditCardImpl] Refunded $" << fixed << setprecision(2) << amount 
             << " back to Card: " << maskedCardNumber_ << "\n";
    }
};

// =====================================================================================
// 3. CONCRETE IMPLEMENTATION 2: Crypto Wallet Processor
// Contains totally different implementation details (blockchain network, wallet addresses).
// =====================================================================================

class CryptoPaymentProcessor final : public IPaymentProcessor {
private:
    string walletAddress_;
    string networkName_;

public:
    CryptoPaymentProcessor(string walletAddress, string network)
        : walletAddress_(std::move(walletAddress)), networkName_(std::move(network)) {}

    bool processPayment(double amount) override {
        cout << "  - [CryptoImpl] Broadcasting transaction to " << networkName_ << " Blockchain...\n";
        cout << "  - [CryptoImpl] Transferred equivalent of $" << fixed << setprecision(2) << amount 
             << " to Wallet: " << walletAddress_ << " -> CONFIRMED IN BLOCK\n";
        return true;
    }

    [[nodiscard]] string getProviderName() const override {
        return "Decentralized Crypto Network (" + networkName_ + ")";
    }

    void refundPayment(double amount) override {
        cout << "  - [CryptoImpl] Executing reverse Smart Contract refund for $" 
             << fixed << setprecision(2) << amount << "\n";
    }
};

// =====================================================================================
// 4. CLIENT CODE (DECOUPLED FROM IMPLEMENTATION)
// High-level business logic depends ONLY on the Interface, NOT concrete implementations!
// =====================================================================================

class CheckoutService {
public:
    // Takes interface reference polymorphic parameter
    static void executeOrderCheckout(IPaymentProcessor& processor, double totalAmount) {
        cout << "\n------------------------------------------------------------\n";
        cout << "  [CheckoutService] Processing checkout using: " << processor.getProviderName() << "\n";
        cout << "------------------------------------------------------------\n";

        bool status = processor.processPayment(totalAmount);
        if (status) {
            cout << "  [CheckoutService] Order payment verified. Fulfilling order...\n";
        } else {
            cout << "  [CheckoutService] Payment failed! Reverting transaction...\n";
            processor.refundPayment(totalAmount);
        }
    }
};

// Factory Function to demonstrate hiding concrete instantiation behind interface pointer
unique_ptr<IPaymentProcessor> createPaymentProcessor(int option, const string& accountDetails) {
    if (option == 1) {
        return make_unique<CreditCardPaymentProcessor>(accountDetails, "API_KEY_LIVE_9982");
    } else {
        return make_unique<CryptoPaymentProcessor>(accountDetails, "Ethereum Mainnet");
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int paymentChoice = 0;
    double checkoutAmount = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Select Payment Provider (1 = Credit Card, 2 = Crypto Wallet): " << flush;
    if (!(cin >> paymentChoice) || (paymentChoice != 1 && paymentChoice != 2)) {
        cout << "Invalid selection. Defaulting to Credit Card (1)." << endl;
        paymentChoice = 1;
    }

    cout << "Enter checkout transaction amount in USD (e.g., 149.50): " << flush;
    if (!(cin >> checkoutAmount) || checkoutAmount <= 0.0) {
        cout << "Invalid amount. Defaulting transaction amount to $149.50." << endl;
        checkoutAmount = 149.50;
    }

    // 1. DEMONSTRATING INTERFACE POLYMORPHISM & DECOUPLING
    cout << "\n================ 1. INTERFACE VS IMPLEMENTATION IN ACTION ================" << endl;

    string accountIdentifier = (paymentChoice == 1) ? "****-****-****-4321" : "0x71C...39A2";

    // Client code works purely through the IPaymentProcessor interface pointer
    unique_ptr<IPaymentProcessor> processor = createPaymentProcessor(paymentChoice, accountIdentifier);

    // Pass interface reference to decoupled business logic
    CheckoutService::executeOrderCheckout(*processor, checkoutAmount);

    // Demonstrate calling interface refund
    cout << "\n[System Event] Customer requested item return & refund:\n";
    processor->refundPayment(checkoutAmount);

    // 2. SUMMARY OF ADVANTAGES
    cout << "\n================ INTERFACE VS IMPLEMENTATION SUMMARY ================" << endl;
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Metric                | Interface (`IPaymentProcessor`)   | Implementation (`CreditCard...`)  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Role                  | Defines WHAT actions are possible | Defines HOW actions are executed  |\n"
         << "| State Variables       | NONE (Pure abstract contract)     | Contains private fields & data    |\n"
         << "| Instantiation         | Impossible (Pure virtual)         | Directly instantiable via `new`   |\n"
         << "| Coupling Impact       | Zero coupling to business logic   | High dependency on specific APIs  |\n"
         << "| Build Isolation       | Changing interface recompiles all | Changing implementation requires  |\n"
         << "|                       | derived classes                   | compiling ONLY its own .cpp file  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}