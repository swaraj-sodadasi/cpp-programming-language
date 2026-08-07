/*
 * =====================================================================================
 * CONCEPT        : Constructors and Destructors in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the lifecycle management, initialization order, and taxonomy of 
 *                  Constructors and Destructors in C++:
 *
 *                  1. Default & Parameterized Ctors : Explicit initialization and member initializer lists.
 *                  2. Explicit Constructors         : Preventing unintended implicit type conversions (`explicit`).
 *                  3. Delegating Constructors        : Reducing code duplication by calling peer constructors.
 *                  4. Copy & Move Constructors      : Deep-copying vs. zero-cost resource ownership transfer.
 *                  5. Defaulted & Deleted Operations: Controlling member generation (`= default`, `= delete`).
 *                  6. Deterministic Destructors     : Stack unwinding, RAII cleanup, and Virtual Destructors.
 *                  7. Construction/Destruction Order: LIFO order for stack/members; Base-to-Derived (Ctor) 
 *                                                     and Derived-to-Base (Dtor) sequencing.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Access / Move : O(1) constant time.
 * SPACE COMPLEXITY : Object Footprint : Sum of data members (+ 8 bytes vptr if virtual destructor present).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// 1. CONSTRUCTOR TAXONOMY & LIFECYCLE MANAGEMENT CLASS
// Demonstrates default, parameterized, explicit, delegating, copy, and move constructors.
// =====================================================================================
class ConstructorTaxonomy {
private:
    int id_;
    string label_;
    int* dataBuffer_;

public:
    static inline int activeCount = 0; // C++17 inline static variable

    // A. Default Constructor (Delegates to Parameterized Constructor)
    ConstructorTaxonomy() : ConstructorTaxonomy(0, "Default_Label", 0) {
        cout << "    [DEFAULT CTOR]   Delegated initialization complete.\n";
    }

    // B. Explicit Parameterized Constructor with Member Initializer List
    // 'explicit' prevents implicit type conversions (e.g., ConstructorTaxonomy obj = 100;)
    explicit ConstructorTaxonomy(int id, string label = "Generic", int bufferVal = 0)
        : id_(id), label_(std::move(label)), dataBuffer_(new int(bufferVal)) {
        if (id < 0) {
            delete dataBuffer_;
            throw invalid_argument("ID cannot be negative!");
        }
        ++activeCount;
        cout << "    [PARAM CTOR]     Created ID: " << setw(4) << id_ 
             << " ('" << setw(15) << left << label_ << "') | Active: " << activeCount << "\n";
    }

    // C. Destructor (Deterministic Teardown)
    ~ConstructorTaxonomy() noexcept {
        delete dataBuffer_; // Safe delete (no-op if nullptr)
        --activeCount;
        cout << "    [DESTRUCTOR]     Destroyed ID: " << setw(4) << id_ 
             << " ('" << setw(15) << left << label_ << "') | Active: " << activeCount << "\n";
    }

    // D. Copy Constructor (Deep Copy)
    ConstructorTaxonomy(const ConstructorTaxonomy& other)
        : id_(other.id_ + 1000), 
          label_(other.label_ + "_Copy"), 
          dataBuffer_(new int(*other.dataBuffer_)) {
        ++activeCount;
        cout << "    [COPY CTOR]      Deep-copied source ID " << other.id_ << " into new ID " << id_ << "\n";
    }

    // E. Copy Assignment Operator
    ConstructorTaxonomy& operator=(const ConstructorTaxonomy& other) {
        if (this != &other) { // Self-assignment guard
            int* newBuf = new int(*other.dataBuffer_); // Exception-safe allocation
            delete dataBuffer_;
            dataBuffer_ = newBuf;
            id_ = other.id_ + 2000;
            label_ = other.label_ + "_Assigned";
            cout << "    [COPY ASSIGN]    Assigned state from source ID " << other.id_ << " to ID " << id_ << "\n";
        }
        return *this;
    }

    // F. Move Constructor (Resource Transfer)
    ConstructorTaxonomy(ConstructorTaxonomy&& other) noexcept
        : id_(other.id_), 
          label_(std::move(other.label_) + "_Moved"), 
          dataBuffer_(other.dataBuffer_) {
        other.id_ = -1;
        other.dataBuffer_ = nullptr; // Disarm source object
        cout << "    [MOVE CTOR]      Transferred resource ownership to ID " << id_ << "\n";
    }

    // G. Move Assignment Operator
    ConstructorTaxonomy& operator=(ConstructorTaxonomy&& other) noexcept {
        if (this != &other) {
            delete dataBuffer_;
            id_ = other.id_;
            label_ = std::move(other.label_) + "_MoveAssigned";
            dataBuffer_ = other.dataBuffer_;

            other.id_ = -1;
            other.dataBuffer_ = nullptr; // Disarm source object
            cout << "    [MOVE ASSIGN]    Move-assigned resource ownership into ID " << id_ << "\n";
        }
        return *this;
    }

    void displayState() const {
        if (dataBuffer_) {
            cout << "    [STATE INSPECTION] ID: " << setw(4) << id_ 
                 << " | Label: " << setw(18) << left << label_ 
                 << " | Value: " << *dataBuffer_ << "\n";
        } else {
            cout << "    [STATE INSPECTION] ID: " << setw(4) << id_ 
                 << " | Label: " << setw(18) << left << label_ 
                 << " | Buffer: NULLPTR (Moved-From)\n";
        }
    }

    [[nodiscard]] int getId() const noexcept { return id_; }
};

// =====================================================================================
// 2. INHERITANCE SEQUENCE & VIRTUAL DESTRUCTOR CLASS HIERARCHY
// Demonstrates Base-to-Derived construction order and Derived-to-Base destruction order.
// =====================================================================================
class BaseResource {
protected:
    int baseId_;

public:
    explicit BaseResource(int baseId) : baseId_(baseId) {
        cout << "      [1. BaseResource Ctor] Initializing Base ID: " << baseId_ << "\n";
    }

    // CRITICAL: Virtual Destructor guarantees derived destructors execute through base pointers!
    virtual ~BaseResource() noexcept {
        cout << "      [4. BaseResource Dtor] Cleaning up Base ID: " << baseId_ << "\n";
    }

    virtual void execute() const {
        cout << "      [Base Execute] Running Base ID: " << baseId_ << "\n";
    }
};

class MemberSubObject {
private:
    string memberName_;

public:
    explicit MemberSubObject(string name) : memberName_(std::move(name)) {
        cout << "      [2. MemberSubObject Ctor] Constructing member: '" << memberName_ << "'\n";
    }

    ~MemberSubObject() noexcept {
        cout << "      [3. MemberSubObject Dtor] Destroying member: '" << memberName_ << "'\n";
    }
};

class DerivedResource : public BaseResource {
private:
    MemberSubObject subMember_; // Member sub-object to trace member initialization order
    double derivedData_;

public:
    DerivedResource(int baseId, string memberName, double data)
        : BaseResource(baseId),          // Step 1: Base Ctor called first
          subMember_(std::move(memberName)), // Step 2: Member sub-object Ctor called second
          derivedData_(data) {           // Step 3: Derived Ctor body executes third
        cout << "      [3. DerivedResource Ctor] Fully constructed Derived object with data: " 
             << derivedData_ << "\n";
    }

    ~DerivedResource() noexcept override {
        cout << "      [1. DerivedResource Dtor] Executing Derived Dtor body...\n";
        // Step 2: Member sub-object Dtor executes automatically after Derived Dtor
        // Step 3: Base Dtor executes automatically last
    }

    void execute() const override {
        cout << "      [Derived Execute] Base ID: " << baseId_ 
             << " | Derived Data: " << derivedData_ << "\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Constructors/Destructors analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CONSTRUCTOR TAXONOMY (DEFAULT, PARAMETERIZED, EXPLICIT, DELEGATING)
    // =====================================================================================
    cout << "\n================ 1. CONSTRUCTOR TAXONOMY & DELEGATION ================\n";

    {
        cout << "  - Creating object using Default Constructor (Delegates to Parameterized)...\n";
        ConstructorTaxonomy objDefault;
        objDefault.displayState();

        cout << "\n  - Creating object using Explicit Parameterized Constructor...\n";
        ConstructorTaxonomy objParam(userInputValue, "PrimaryNode", 42);
        objParam.displayState();

        // ConstructorTaxonomy implicitObj = userInputValue + 1; 
        // COMPILE ERROR: 'explicit' prevents implicit type conversion from integer!

        cout << "\n  - Exiting local scope (Watch deterministic LIFO destructor calls)...\n";
    }

    cout << "  - Active Instances Post-Scope: " << ConstructorTaxonomy::activeCount << "\n";

    // =====================================================================================
    // 2. COPY & MOVE LIFECYCLE SEMANTICS (RULE OF 5)
    // =====================================================================================
    cout << "\n================ 2. COPY & MOVE CONSTRUCTORS (RULE OF 5) ================\n";

    {
        cout << "  - Instantiating primary object...\n";
        ConstructorTaxonomy sourceObj(userInputValue + 10, "MovableResource", 99);

        cout << "\n  - Invoking Copy Constructor (Deep Copy)...\n";
        ConstructorTaxonomy copiedObj = sourceObj; // Copy Ctor
        copiedObj.displayState();

        cout << "\n  - Invoking Move Constructor (Resource Transfer)...\n";
        ConstructorTaxonomy movedObj = std::move(sourceObj); // Move Ctor
        movedObj.displayState();

        cout << "  - Inspecting source object state post-move:\n";
        sourceObj.displayState(); // Disarmed source in valid empty state

        cout << "\n  - Exiting copy/move scope...\n";
    }

    cout << "  - Active Instances Post Copy/Move Scope: " << ConstructorTaxonomy::activeCount << "\n";

    // =====================================================================================
    // 3. INHERITANCE SEQUENCE & VIRTUAL DESTRUCTORS
    // =====================================================================================
    cout << "\n================ 3. INHERITANCE CONSTRUCTION & DESTRUCTION SEQUENCE ================\n";

    {
        cout << "  - Instantiating `DerivedResource` object on stack:\n";
        cout << "    [CONSTRUCTION SEQUENCE]: Base -> Members -> Derived Body\n";
        DerivedResource derivedObj(userInputValue + 20, "SubMemberA", 3.14159);
        derivedObj.execute();

        cout << "\n    [DESTRUCTION SEQUENCE]: Derived Body -> Members -> Base\n";
        cout << "  - Leaving `DerivedResource` stack scope...\n";
    }

    cout << "\n================ 4. VIRTUAL DESTRUCTOR POLYMORPHIC CLEANUP ================\n";

    {
        cout << "  - Creating Derived object assigned to Base class pointer (`unique_ptr<BaseResource>`):\n";
        unique_ptr<BaseResource> polyPtr = make_unique<DerivedResource>(
            userInputValue + 30, "PolymorphicSubMember", 99.8
        );

        polyPtr->execute();

        cout << "\n  - Releasing polymorphic object through Base pointer...\n";
        cout << "  - [VIRTUAL DTOR GUARANTEE]: Virtual destructor ensures Derived Dtor fires first!\n";
    } // `polyPtr` destroyed automatically HERE via RAII

    // =====================================================================================
    // 5. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 5. CONSTRUCTOR INVARIANT GUARDS ================\n";

    try {
        cout << "  - Attempting to pass invalid negative ID (-50) to constructor...\n";
        ConstructorTaxonomy invalidObj(-50, "FaultyNode", 10);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    cout << "\n  - Final Active Instance Count: " << ConstructorTaxonomy::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CONSTRUCTORS & DESTRUCTORS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Special Member Type   | Syntax / Modifier                 | Primary Architectural Function    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Default Constructor   | `Type();`                         | Initializes object without arguments|\n"
         << "| Parameterized Ctor    | `Type(int a, string b);`          | Initializes state with arguments  |\n"
         << "| Explicit Constructor  | `explicit Type(int a);`           | Blocks implicit type conversions  |\n"
         << "| Delegating Ctor       | `Type() : Type(0, \"\");`           | Eliminates constructor duplication|\n"
         << "| Copy Constructor      | `Type(const Type&);`              | Performs deep-copy cloning        |\n"
         << "| Move Constructor      | `Type(Type&&) noexcept;`          | Zero-cost ownership transfer      |\n"
         << "| Defaulted Operation   | `Type() = default;`               | Requests compiler-generated logic |\n"
         << "| Deleted Operation     | `Type(const Type&) = delete;`     | Blocks operation usage at compile |\n"
         << "| Destructor            | `~Type() noexcept;`               | Deterministic RAII resource cleanup|\n"
         << "| Virtual Destructor    | `virtual ~Base() noexcept;`       | Enables safe polymorphic deletion |\n"
         << "| Base Ctor Execution   | Base -> Members -> Derived        | Order of construction execution   |\n"
         << "| Base Dtor Execution   | Derived -> Members -> Base        | Order of destruction execution    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}