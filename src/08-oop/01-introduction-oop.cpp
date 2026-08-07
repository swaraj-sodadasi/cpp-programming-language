/*
 * =====================================================================================
 * CONCEPT        : Introduction to Object-Oriented Programming (OOP) in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the fundamental pillars of Object-Oriented Programming (OOP) and 
 *                  core object lifecycle management in Modern C++:
 *
 *                  1. Encapsulation & Abstraction : Bundling data members and member functions,
 *                                                   hiding internal details via access specifiers
 *                                                   (`private`, `protected`, `public`), and enforcing invariants.
 *                  2. Object Lifecycle            : Explicit constructors (Default, Parameterized, Copy, Move)
 *                                                   and Destructors for deterministic object management.
 *                  3. Inheritance & Code Reuse    : Extending base class functionality, access control models,
 *                                                   and establishing "is-a" structural relationships.
 *                  4. Runtime Polymorphism        : Dynamic dispatch using `virtual` methods, `override`,
 *                                                   abstract base classes (interfaces), and virtual destructors.
 *                  5. Compile-Time Polymorphism   : Static dispatch via function overloading, operator
 *                                                   overloading, and compile-time resolution.
 *
 * TIME COMPLEXITY  : Direct Method Call / Access: O(1) constant time.
 *                    Virtual Function Call      : O(1) vtable indirection.
 * SPACE COMPLEXITY : Object Footprint: Sum of member variable sizes + 8 bytes vptr (if virtual methods exist).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// Portable mathematical constant definition replacing non-standard M_PI
constexpr double PI_VAL = 3.14159265358979323846;

// =====================================================================================
// 1. ENCAPSULATION & ABSTRACTION DEMONSTRATION
// Protects internal object state and enforces domain invariants via a clean interface.
// =====================================================================================
class BankAccount {
private:
    int accountId_;
    string ownerName_;
    double balance_;

public:
    // Explicit constructor enforcing class invariants
    explicit BankAccount(int id, string owner, double initialBalance = 0.0)
        : accountId_(id), ownerName_(std::move(owner)), balance_(0.0) {
        if (id <= 0) {
            throw invalid_argument("Account ID must be strictly positive!");
        }
        if (initialBalance < 0.0) {
            throw invalid_argument("Initial balance cannot be negative!");
        }
        balance_ = initialBalance;
        cout << "    [BankAccount Ctor] Created account #" << accountId_ 
             << " for '" << ownerName_ << "' with initial balance: $" << fixed << setprecision(2) << balance_ << "\n";
    }

    // Public Abstraction Interface (Hides implementation details)
    void deposit(double amount) {
        if (amount <= 0.0) {
            throw invalid_argument("Deposit amount must be greater than zero!");
        }
        balance_ += amount;
        cout << "    [DEPOSIT] Account #" << accountId_ << " deposited: $" << amount 
             << " | New Balance: $" << balance_ << "\n";
    }

    void withdraw(double amount) {
        if (amount <= 0.0) {
            throw invalid_argument("Withdrawal amount must be greater than zero!");
        }
        if (amount > balance_) {
            throw runtime_error("Insufficient funds for withdrawal!");
        }
        balance_ -= amount;
        cout << "    [WITHDRAWAL] Account #" << accountId_ << " withdrew: $" << amount 
             << " | Remaining Balance: $" << balance_ << "\n";
    }

    // Read-only Accessors (Getters)
    [[nodiscard]] int getAccountId() const noexcept { return accountId_; }
    [[nodiscard]] const string& getOwnerName() const noexcept { return ownerName_; }
    [[nodiscard]] double getBalance() const noexcept { return balance_; }
};

// =====================================================================================
// 2. OBJECT LIFECYCLE & SPECIAL MEMBER FUNCTIONS
// Demonstrates Default, Parameterized, Copy, and Move Constructors, plus Destructors.
// =====================================================================================
class LifecycleDemo {
private:
    int id_;
    string label_;
    int* dataBuffer_;

public:
    static inline int activeObjects = 0; // C++17 inline static variable

    // A. Default Constructor
    LifecycleDemo() 
        : id_(0), label_("DefaultLabel"), dataBuffer_(new int(0)) {
        ++activeObjects;
        cout << "    [DEFAULT CTOR]   ID: " << id_ << " (" << label_ << ") created | Active: " << activeObjects << "\n";
    }

    // B. Parameterized Constructor
    LifecycleDemo(int id, string label, int initialValue)
        : id_(id), label_(std::move(label)), dataBuffer_(new int(initialValue)) {
        ++activeObjects;
        cout << "    [PARAM CTOR]     ID: " << id_ << " (" << label_ << ") created | Active: " << activeObjects << "\n";
    }

    // C. Destructor
    ~LifecycleDemo() noexcept {
        delete dataBuffer_;
        --activeObjects;
        cout << "    [DESTRUCTOR]     ID: " << id_ << " (" << label_ << ") destroyed | Active: " << activeObjects << "\n";
    }

    // D. Copy Constructor (Deep Copy)
    LifecycleDemo(const LifecycleDemo& other)
        : id_(other.id_ + 1000), label_(other.label_ + "_Copy"), dataBuffer_(new int(*other.dataBuffer_)) {
        ++activeObjects;
        cout << "    [COPY CTOR]      Cloned ID " << other.id_ << " into new ID " << id_ << " (" << label_ << ")\n";
    }

    // E. Copy Assignment Operator
    LifecycleDemo& operator=(const LifecycleDemo& other) {
        if (this != &other) {
            int* newBuf = new int(*other.dataBuffer_); // Exception-safe allocation
            delete dataBuffer_;
            dataBuffer_ = newBuf;
            id_ = other.id_ + 2000;
            label_ = other.label_ + "_Assigned";
            cout << "    [COPY ASSIGN]    Assigned values from ID " << other.id_ << " into ID " << id_ << "\n";
        }
        return *this;
    }

    // F. Move Constructor (Resource Transfer)
    LifecycleDemo(LifecycleDemo&& other) noexcept
        : id_(other.id_), label_(std::move(other.label_) + "_Moved"), dataBuffer_(other.dataBuffer_) {
        other.id_ = -1;
        other.dataBuffer_ = nullptr; // Relinquish ownership from source
        cout << "    [MOVE CTOR]      Transferred ownership from source object to ID: " << id_ << "\n";
    }

    // G. Move Assignment Operator
    LifecycleDemo& operator=(LifecycleDemo&& other) noexcept {
        if (this != &other) {
            delete dataBuffer_;
            id_ = other.id_;
            label_ = std::move(other.label_) + "_MoveAssigned";
            dataBuffer_ = other.dataBuffer_;

            other.id_ = -1;
            other.dataBuffer_ = nullptr;
            cout << "    [MOVE ASSIGN]    Move-assigned resources into ID " << id_ << "\n";
        }
        return *this;
    }

    void printState() const {
        if (dataBuffer_) {
            cout << "    [STATE INSPECTION] ID: " << id_ << " | Label: " << label_ 
                 << " | Buffer Value: " << *dataBuffer_ << "\n";
        } else {
            cout << "    [STATE INSPECTION] ID: " << id_ << " | Label: " << label_ << " | Buffer: NULLPTR (Moved)\n";
        }
    }
};

// =====================================================================================
// 3. INHERITANCE & ACCESS CONTROL
// Base class and derived class establishing an "is-a" structural hierarchy.
// =====================================================================================
class Vehicle {
protected: // Accessible by derived classes, hidden from outside callers
    int vehicleId_;
    string brand_;
    double topSpeedKmh_;

public:
    Vehicle(int id, string brand, double topSpeed)
        : vehicleId_(id), brand_(std::move(brand)), topSpeedKmh_(topSpeed) {
        cout << "    [Vehicle Base Ctor] Created " << brand_ << " (ID: " << vehicleId_ << ")\n";
    }

    virtual ~Vehicle() noexcept {
        cout << "    [Vehicle Base Dtor] Destroyed Vehicle ID: " << vehicleId_ << "\n";
    }

    void displayBaseInfo() const {
        cout << "    [Vehicle Info] ID: " << vehicleId_ << " | Brand: " << brand_ 
             << " | Top Speed: " << topSpeedKmh_ << " km/h\n";
    }
};

// Derived Class inheriting publicly from Vehicle
class ElectricCar : public Vehicle {
private:
    double batteryCapacityKwh_;

public:
    ElectricCar(int id, string brand, double topSpeed, double batteryCapacity)
        : Vehicle(id, std::move(brand), topSpeed), // Delegate construction to Base class
          batteryCapacityKwh_(batteryCapacity) {
        cout << "    [ElectricCar Derived Ctor] Battery Capacity: " << batteryCapacityKwh_ << " kWh\n";
    }

    ~ElectricCar() noexcept override {
        cout << "    [ElectricCar Derived Dtor] Releasing ElectricCar resources...\n";
    }

    void displayFullSpecs() const {
        // Can directly access 'protected' base members: brand_, topSpeedKmh_
        cout << "    [ElectricCar Full Specs] Brand: " << brand_ 
             << " | Top Speed: " << topSpeedKmh_ << " km/h | Battery: " << batteryCapacityKwh_ << " kWh\n";
    }
};

// =====================================================================================
// 4. RUNTIME POLYMORPHISM & ABSTRACT BASE CLASSES
// Interface specialization using pure virtual functions, dynamic dispatch, and smart pointers.
// =====================================================================================
class AbstractShape {
public:
    virtual ~AbstractShape() noexcept = default; // Essential Virtual Destructor

    // Pure Virtual Functions (Interface Contract)
    [[nodiscard]] virtual double calculateArea() const = 0;
    [[nodiscard]] virtual double calculatePerimeter() const = 0;
    virtual void render() const = 0;
};

class Circle : public AbstractShape {
private:
    double radius_;

public:
    explicit Circle(double radius) : radius_(radius) {
        if (radius <= 0.0) throw invalid_argument("Radius must be positive!");
    }

    [[nodiscard]] double calculateArea() const override {
        return PI_VAL * radius_ * radius_;
    }

    [[nodiscard]] double calculatePerimeter() const override {
        return 2.0 * PI_VAL * radius_;
    }

    void render() const override {
        cout << "    [RENDER CIRCLE] Radius: " << radius_ 
             << " | Area: " << calculateArea() 
             << " | Perimeter: " << calculatePerimeter() << "\n";
    }
};

class Rectangle : public AbstractShape {
private:
    double width_;
    double height_;

public:
    Rectangle(double w, double h) : width_(w), height_(h) {
        if (w <= 0.0 || h <= 0.0) throw invalid_argument("Dimensions must be positive!");
    }

    [[nodiscard]] double calculateArea() const override {
        return width_ * height_;
    }

    [[nodiscard]] double calculatePerimeter() const override {
        return 2.0 * (width_ + height_);
    }

    void render() const override {
        cout << "    [RENDER RECTANGLE] Width: " << width_ << ", Height: " << height_ 
             << " | Area: " << calculateArea() 
             << " | Perimeter: " << calculatePerimeter() << "\n";
    }
};

// =====================================================================================
// 5. COMPILE-TIME POLYMORPHISM (OVERLOADING)
// Static dispatch via method overloading and operator overloading.
// =====================================================================================
class Vector2D {
private:
    double x_;
    double y_;

public:
    Vector2D(double x = 0.0, double y = 0.0) : x_(x), y_(y) {}

    // Method Overloading 1: Add another Vector2D
    [[nodiscard]] Vector2D add(const Vector2D& other) const noexcept {
        return Vector2D(x_ + other.x_, y_ + other.y_);
    }

    // Method Overloading 2: Add a scalar value to both components
    [[nodiscard]] Vector2D add(double scalar) const noexcept {
        return Vector2D(x_ + scalar, y_ + scalar);
    }

    // Operator Overloading: Binary '+'
    Vector2D operator+(const Vector2D& rhs) const noexcept {
        return Vector2D(x_ + rhs.x_, y_ + rhs.y_);
    }

    // Operator Overloading: Stream Insertion '<<'
    friend ostream& operator<<(ostream& os, const Vector2D& vec) {
        os << "(" << vec.x_ << ", " << vec.y_ << ")";
        return os;
    }

    [[nodiscard]] double getX() const noexcept { return x_; }
    [[nodiscard]] double getY() const noexcept { return y_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for OOP demonstration analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. ENCAPSULATION & ABSTRACTION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. ENCAPSULATION & ABSTRACTION ================\n";

    try {
        BankAccount account(userInputValue, "Alice Smith", 500.0);
        account.deposit(250.0);
        account.withdraw(150.0);

        cout << "  - Attempting invalid withdrawal to demonstrate invariant protection...\n";
        account.withdraw(1000.0); // Triggers exception safely
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // 2. OBJECT LIFECYCLE & CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 2. OBJECT LIFECYCLE & CONSTRUCTORS ================\n";

    {
        cout << "  - Creating objects to demonstrate constructors...\n";
        LifecycleDemo obj1(userInputValue + 10, "PrimaryObj", 42);
        
        cout << "\n  - Invoking Copy Constructor...\n";
        LifecycleDemo obj2 = obj1; // Copy Ctor
        obj2.printState();

        cout << "\n  - Invoking Move Constructor...\n";
        LifecycleDemo obj3 = std::move(obj1); // Move Ctor
        obj3.printState();
        
        cout << "  - Inspecting source object state post-move:\n";
        obj1.printState(); // Source is left in valid empty state

        cout << "\n  - Leaving local block scope (Watch destructor execution order)...\n";
    } // Objects destroyed here in reverse LIFO order

    cout << "  - Active Lifecycle Demo Objects Post-Scope: " << LifecycleDemo::activeObjects << "\n";

    // =====================================================================================
    // 3. INHERITANCE & ACCESS CONTROL
    // =====================================================================================
    cout << "\n================ 3. INHERITANCE & ACCESS CONTROL ================\n";

    {
        cout << "  - Instantiating derived ElectricCar object...\n";
        ElectricCar eCar(userInputValue + 20, "Tesla Model S", 250.0, 100.0);
        
        eCar.displayBaseInfo();  // Inherited Base method
        eCar.displayFullSpecs(); // Derived method accessing protected base members
        
        cout << "  - Leaving Derived scope...\n";
    }

    // =====================================================================================
    // 4. RUNTIME POLYMORPHISM (ABSTRACT BASE CLASS & VIRTUAL FUNCTIONS)
    // =====================================================================================
    cout << "\n================ 4. RUNTIME POLYMORPHISM & VIRTUAL DISPATCH ================\n";

    {
        cout << "  - Storing heterogeneous derived objects in vector<unique_ptr<AbstractShape>>...\n";
        vector<unique_ptr<AbstractShape>> shapes;
        
        shapes.push_back(make_unique<Circle>(5.0));
        shapes.push_back(make_unique<Rectangle>(4.0, 6.0));
        shapes.push_back(make_unique<Circle>(2.5));

        cout << "  - Iterating polymorphically through Abstract Base Class pointers:\n";
        for (const auto& shape : shapes) {
            // Dynamic dispatch selects appropriate overriden method at runtime
            shape->render();
        }

        cout << "  - Exiting polymorphic vector scope (Virtual destructors run automatically)...\n";
    }

    // =====================================================================================
    // 5. COMPILE-TIME POLYMORPHISM (METHOD & OPERATOR OVERLOADING)
    // =====================================================================================
    cout << "\n================ 5. COMPILE-TIME POLYMORPHISM (OVERLOADING) ================\n";

    Vector2D v1(3.0, 4.0);
    Vector2D v2(1.5, 2.5);

    cout << "  - Initial Vector v1 : " << v1 << "\n";
    cout << "  - Initial Vector v2 : " << v2 << "\n";

    // Method Overloading Execution
    Vector2D vAddVector = v1.add(v2);
    Vector2D vAddScalar = v1.add(10.0);

    cout << "  - Method Overload v1.add(v2)       : " << vAddVector << "\n";
    cout << "  - Method Overload v1.add(10.0)     : " << vAddScalar << "\n";

    // Operator Overloading Execution
    Vector2D vSum = v1 + v2;
    cout << "  - Operator Overload v1 + v2         : " << vSum << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ INTRODUCTION TO OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| OOP Pillar / Concept  | Implementation Mechanism          | Primary Architectural Benefit     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Encapsulation         | private fields + Public Methods   | Protects object state invariants  |\n"
         << "| Abstraction           | Public Member Interfaces          | Hides complex implementation details|\n"
         << "| Object Lifecycle      | Ctors, Dtor, Rule of 5            | Deterministic resource management |\n"
         << "| Inheritance           | class Derived : public Base       | Enables code reuse ('is-a' model) |\n"
         << "| Runtime Polymorphism  | virtual methods + override        | Dynamic dispatch & interface reuse|\n"
         << "| Abstract Base Class   | Pure virtual 'virtual void f()=0;'| Defines strict interface contracts|\n"
         << "| Compile Polymorphism  | Method & Operator Overloading     | Static dispatch without vtable cost|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}