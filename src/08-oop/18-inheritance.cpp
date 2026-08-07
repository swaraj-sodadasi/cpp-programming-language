/*
 * =====================================================================================
 * CONCEPT        : Inheritance in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core mechanisms, types, modes, and scope rules of Inheritance:
 *
 *                  1. Base and Derived Classes   : Establishing "is-a" structural relationships,
 *                                                 constructor/destructor execution order.
 *                  2. Inheritance Access Modes   :
 *                     - `public`    : Preserves base access (Public -> Public, Protected -> Protected).
 *                     - `protected` : Converts public and protected base members to protected.
 *                     - `private`   : Converts public and protected base members to private.
 *                  3. Types of Inheritance       :
 *                     - Single       : Derived class inherits from one Base class.
 *                     - Multilevel   : Derived class inherits from a Base class which is itself Derived.
 *                     - Multiple     : Derived class inherits from multiple independent Base classes.
 *                     - Hierarchical : Multiple Derived classes inherit from a single Base class.
 *                     - Hybrid       : Combination of inheritance types (demonstrating Virtual
 *                                      Inheritance to resolve the Diamond Problem).
 *                  4. Function Overriding        : Runtime dynamic dispatch using `virtual` methods,
 *                                                 `override`, and `final` specifiers.
 *                  5. Name Hiding (Shadowing)    : Member functions in derived classes hiding base
 *                                                 overloads in outer scopes, and restoring them via `using`.
 *
 * TIME COMPLEXITY  : Direct Method Call / Non-Virtual : O(1) constant time.
 *                    Virtual Dynamic Dispatch        : O(1) constant time (via vtable indirection).
 * SPACE COMPLEXITY : Class Instance Footprint       : Sum of base/derived member sizes (+ 8-byte vptr
 *                                                     if virtual methods/inheritance present).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. INHERITANCE ACCESS MODES (PUBLIC, PROTECTED, PRIVATE)
// =====================================================================================
class BaseAccount {
public:
    int accountId_;        // Accessible everywhere

protected:
    string accountHolder_; // Accessible within BaseAccount and derived classes

private:
    double secretBalance_; // Accessible ONLY within BaseAccount

public:
    BaseAccount(int id, string holder, double balance)
        : accountId_(id), accountHolder_(std::move(holder)), secretBalance_(balance) {
        cout << "    [BaseAccount Ctor] Created ID: " << accountId_ << " ('" << accountHolder_ << "')\n";
    }

    virtual ~BaseAccount() noexcept {
        cout << "    [BaseAccount Dtor] Destroyed ID: " << accountId_ << "\n";
    }

    [[nodiscard]] double getSecretBalance() const noexcept { return secretBalance_; }
};

// A. PUBLIC INHERITANCE
class PublicDerivedAccount : public BaseAccount {
public:
    PublicDerivedAccount(int id, string holder, double balance)
        : BaseAccount(id, std::move(holder), balance) {}

    void inspectState() const {
        cout << "    [PublicDerived] Public accountId_      : " << accountId_ << "\n";
        cout << "    [PublicDerived] Protected accountHolder_: " << accountHolder_ << "\n";
        // secretBalance_ is inaccessible directly
    }
};

// B. PROTECTED INHERITANCE (Downgrades public base members to protected)
class ProtectedDerivedAccount : protected BaseAccount {
public:
    ProtectedDerivedAccount(int id, string holder, double balance)
        : BaseAccount(id, std::move(holder), balance) {}

    void inspectState() const {
        cout << "    [ProtectedDerived] Downgraded accountId_      : " << accountId_ << "\n";
        cout << "    [ProtectedDerived] Protected accountHolder_    : " << accountHolder_ << "\n";
    }

    // Restoring selective public access using `using`
    using BaseAccount::accountId_;
};

// C. PRIVATE INHERITANCE (Downgrades public/protected base members to private)
class PrivateDerivedAccount : private BaseAccount {
public:
    PrivateDerivedAccount(int id, string holder, double balance)
        : BaseAccount(id, std::move(holder), balance) {}

    void inspectState() const {
        cout << "    [PrivateDerived] Private accountId_           : " << accountId_ << "\n";
        cout << "    [PrivateDerived] Private accountHolder_         : " << accountHolder_ << "\n";
    }

    // Restoring selective public access using `using`
    using BaseAccount::getSecretBalance;
};

// =====================================================================================
// 2. TYPES OF INHERITANCE (SINGLE, MULTILEVEL, MULTIPLE, HIERARCHICAL, HYBRID)
// =====================================================================================

// --- A. SINGLE INHERITANCE ---
class BaseDevice {
protected:
    int deviceId_;

public:
    explicit BaseDevice(int id) : deviceId_(id) {
        cout << "    [Single BaseDevice Ctor] ID: " << deviceId_ << "\n";
    }
    virtual ~BaseDevice() noexcept = default;
};

class SingleSmartPhone : public BaseDevice {
private:
    string modelName_;

public:
    SingleSmartPhone(int id, string model)
        : BaseDevice(id), modelName_(std::move(model)) {
        cout << "    [SingleSmartPhone Ctor] Model: '" << modelName_ << "'\n";
    }
};

// --- B. MULTILEVEL INHERITANCE ---
class ComponentHardware : public BaseDevice {
protected:
    int architectureBits_;

public:
    ComponentHardware(int id, int bits)
        : BaseDevice(id), architectureBits_(bits) {
        cout << "    [Multilevel Level-1 Hardware Ctor] Bits: " << architectureBits_ << "\n";
    }
};

class MultilevelCpuProcessor : public ComponentHardware {
private:
    double clockSpeedGhz_;

public:
    MultilevelCpuProcessor(int id, int bits, double clockSpeed)
        : ComponentHardware(id, bits), clockSpeedGhz_(clockSpeed) {
        cout << "    [Multilevel Level-2 CPU Ctor] Speed: " << clockSpeedGhz_ << " GHz\n";
    }
};

// --- C. MULTIPLE INHERITANCE ---
class NetworkInterface {
protected:
    string ipAddress_;

public:
    explicit NetworkInterface(string ip) : ipAddress_(std::move(ip)) {
        cout << "    [Multiple Interface 1 Ctor] IP: " << ipAddress_ << "\n";
    }
};

class StorageInterface {
protected:
    size_t storageCapacityGb_;

public:
    explicit StorageInterface(size_t capacity) : storageCapacityGb_(capacity) {
        cout << "    [Multiple Interface 2 Ctor] Capacity: " << storageCapacityGb_ << " GB\n";
    }
};

class MultipleNasServer : public NetworkInterface, public StorageInterface {
private:
    int serverId_;

public:
    MultipleNasServer(int id, string ip, size_t capacity)
        : NetworkInterface(std::move(ip)), StorageInterface(capacity), serverId_(id) {
        cout << "    [Multiple NasServer Ctor] Server ID: " << serverId_ << "\n";
    }
};

// --- D. HIERARCHICAL INHERITANCE ---
class ShapeBase {
public:
    virtual ~ShapeBase() noexcept = default;
    virtual void render() const = 0;
};

class CircleShape : public ShapeBase {
public:
    void render() const override { cout << "    [Hierarchical] Rendering Circle\n"; }
};

class SquareShape : public ShapeBase {
public:
    void render() const override { cout << "    [Hierarchical] Rendering Square\n"; }
};

// --- E. HYBRID INHERITANCE & VIRTUAL INHERITANCE (DIAMOND PROBLEM RESOLUTION) ---
class VirtualPowerSource {
protected:
    int watts_;

public:
    explicit VirtualPowerSource(int watts = 500) : watts_(watts) {
        cout << "    [Virtual Base PowerSource Ctor] " << watts_ << " W\n";
    }
    virtual ~VirtualPowerSource() noexcept = default;
};

// Virtual public inheritance eliminates duplicate PowerSource sub-objects
class VirtualScanner : virtual public VirtualPowerSource {
public:
    explicit VirtualScanner(int watts) : VirtualPowerSource(watts) {
        cout << "    [VirtualScanner Ctor]\n";
    }
};

class VirtualPrinter : virtual public VirtualPowerSource {
public:
    explicit VirtualPrinter(int watts) : VirtualPowerSource(watts) {
        cout << "    [VirtualPrinter Ctor]\n";
    }
};

class HybridCopierMachine : public VirtualScanner, public VirtualPrinter {
private:
    int copierId_;

public:
    HybridCopierMachine(int id, int watts)
        : VirtualPowerSource(watts), // Direct initialization of virtual base
          VirtualScanner(watts),
          VirtualPrinter(watts),
          copierId_(id) {
        cout << "    [HybridCopier Machine Ctor] Copier ID: " << copierId_ 
             << " | Single Power Source: " << watts_ << " W\n";
    }
};

// =====================================================================================
// 3. FUNCTION OVERRIDING & DYNAMIC DISPATCH
// =====================================================================================
class BasePaymentProcessor {
public:
    virtual ~BasePaymentProcessor() noexcept = default;

    // Virtual Function for Dynamic Dispatch
    virtual void processPayment(double amount) const {
        cout << "    [BASE PROCESSOR] Standard processing for $" << fixed << setprecision(2) << amount << "\n";
    }
};

class CreditCardProcessor : public BasePaymentProcessor {
public:
    // Overriding Base Method
    void processPayment(double amount) const override {
        cout << "    [OVERRIDDEN CREDIT CARD] Charged $" << amount << " (+ 2% gateway fee)\n";
    }
};

class CryptoPaymentProcessor : public BasePaymentProcessor {
public:
    // Final Overridden Method (Blocks further derived overrides)
    void processPayment(double amount) const override final {
        cout << "    [FINAL OVERRIDDEN CRYPTO] Executing blockchain transfer for $" << amount << "\n";
    }
};

// =====================================================================================
// 4. NAME HIDING (SHADOWING) AND UNHIDING VIA `using`
// =====================================================================================
class BaseCalculator {
public:
    void compute(int x) const {
        cout << "    [BaseCalculator::compute(int)] Value: " << x << "\n";
    }

    void compute(double x) const {
        cout << "    [BaseCalculator::compute(double)] Value: " << x << "\n";
    }
};

// Derived Class demonstrating Name Hiding (Shadowing)
class HidingDerivedCalculator : public BaseCalculator {
public:
    // Defining compute(string) HIDES compute(int) and compute(double) from BaseCalculator!
    void compute(const string& msg) const {
        cout << "    [HidingDerived::compute(string)] Message: '" << msg << "'\n";
    }
};

// Derived Class restoring hidden base methods using `using`
class UnhidingDerivedCalculator : public BaseCalculator {
public:
    // Unhide overloads from BaseCalculator
    using BaseCalculator::compute;

    void compute(const string& msg) const {
        cout << "    [UnhidingDerived::compute(string)] Message: '" << msg << "'\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Inheritance analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. INHERITANCE ACCESS MODES
    // =====================================================================================
    cout << "\n================ 1. INHERITANCE ACCESS MODES ================\n";

    {
        cout << "  - A. Public Inheritance:\n";
        PublicDerivedAccount pubAcc(userInputValue, "Alice", 1000.0);
        pubAcc.inspectState();

        cout << "\n  - B. Protected Inheritance:\n";
        ProtectedDerivedAccount protAcc(userInputValue + 1, "Bob", 2000.0);
        protAcc.inspectState();
        cout << "    Exposed via 'using': accountId_ = " << protAcc.accountId_ << "\n";

        cout << "\n  - C. Private Inheritance:\n";
        PrivateDerivedAccount privAcc(userInputValue + 2, "Charlie", 3000.0);
        privAcc.inspectState();
        cout << "    Exposed via 'using': getSecretBalance() = $" << privAcc.getSecretBalance() << "\n";
    }

    // =====================================================================================
    // 2. TYPES OF INHERITANCE & DIAMOND PROBLEM RESOLUTION
    // =====================================================================================
    cout << "\n================ 2. TYPES OF INHERITANCE ================\n";

    cout << "  - Single Inheritance:\n";
    SingleSmartPhone phone(userInputValue + 10, "Galaxy S24");

    cout << "\n  - Multilevel Inheritance:\n";
    MultilevelCpuProcessor cpu(userInputValue + 20, 64, 4.2);

    cout << "\n  - Multiple Inheritance:\n";
    MultipleNasServer nas(userInputValue + 30, "192.168.1.100", 16000);

    cout << "\n  - Hierarchical Inheritance:\n";
    CircleShape circle;
    SquareShape square;
    circle.render();
    square.render();

    cout << "\n  - Hybrid Inheritance (Diamond Problem Resolved via Virtual Base Class):\n";
    HybridCopierMachine copier(userInputValue + 40, 750);

    // =====================================================================================
    // 3. FUNCTION OVERRIDING & DYNAMIC DISPATCH
    // =====================================================================================
    cout << "\n================ 3. FUNCTION OVERRIDING & DYNAMIC DISPATCH ================\n";

    {
        cout << "  - Invoking methods via Base Pointers (`vector<unique_ptr<BasePaymentProcessor>>`):\n";
        vector<unique_ptr<BasePaymentProcessor>> processors;

        processors.push_back(make_unique<BasePaymentProcessor>());
        processors.push_back(make_unique<CreditCardProcessor>());
        processors.push_back(make_unique<CryptoPaymentProcessor>());

        for (const auto& proc : processors) {
            proc->processPayment(150.00); // Dynamic dispatch executes overriden implementation
        }
    }

    // =====================================================================================
    // 4. NAME HIDING (SHADOWING) AND RESOLUTION VIA `using`
    // =====================================================================================
    cout << "\n================ 4. NAME HIDING (SHADOWING) & RESOLUTION ================\n";

    {
        cout << "  - Case A: HidingDerivedCalculator (Base overloads hidden):\n";
        HidingDerivedCalculator hidingCalc;
        hidingCalc.compute("Shadowing string overload");
        
        // hidingCalc.compute(42); // COMPILE ERROR: Base compute(int) is hidden by compute(string)!
        cout << "    Resolving hidden base method explicitly via scope resolution `BaseCalculator::compute(42)`:\n";
        hidingCalc.BaseCalculator::compute(42);

        cout << "\n  - Case B: UnhidingDerivedCalculator (Base overloads restored via `using`):\n";
        UnhidingDerivedCalculator unhidingCalc;
        unhidingCalc.compute("Unhidden string overload");
        unhidingCalc.compute(42);   // Unhidden compute(int) called successfully!
        unhidingCalc.compute(3.14); // Unhidden compute(double) called successfully!
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ INHERITANCE IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Inheritance Feature   | Modern C++ Implementation         | Key Architectural Role / Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Public Inheritance    | `class D : public B`              | Preserves 'is-a' interface model  |\n"
         << "| Protected Inheritance | `class D : protected B`           | Downgrades public members         |\n"
         << "| Private Inheritance   | `class D : private B`             | Implementation dependency only    |\n"
         << "| Single Inheritance    | `class D : public B`              | One direct base class             |\n"
         << "| Multilevel Inheritance| `class C : public B; B : public A`| Multi-tier hierarchy chain        |\n"
         << "| Multiple Inheritance  | `class D : public B1, public B2`  | Combines multiple base interfaces |\n"
         << "| Virtual Inheritance   | `class B1 : virtual public Base`  | Resolves Diamond Problem duplication|\n"
         << "| Function Overriding   | `void f() override;`              | Runtime polymorphic dispatch      |\n"
         << "| Name Hiding           | Member in derived hides base names| Shadowed by derived name lookup   |\n"
         << "| Unhiding Names        | `using Base::func;`               | Restores base overloads in derived|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}