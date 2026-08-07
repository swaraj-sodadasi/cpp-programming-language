/*
 * =====================================================================================
 * CONCEPT        : Object Lifetime in RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  how C++ storage durations, lexical scopes, object construction order,
 *                  destruction rules (LIFO), and stack unwinding dictate resource lifecycle:
 *
 *                  1. Automatic (Stack) Lifetime: Deterministic construction and LIFO destruction.
 *                  2. Member Sub-Object Lifetime: Parent-child initialization & teardown sequence.
 *                  3. Temporary Lifetime Extension: Binding temporaries to const/rvalue references.
 *                  4. Heap Storage Duration & RAII: Tying dynamic allocations to wrapper lifetimes.
 *                  5. Exception Stack Unwinding : Guaranteed cleanup during exception handling.
 *                  6. Static Storage Duration   : Program-lifetime RAII objects.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Scope Exit: O(1) constant time.
 * SPACE COMPLEXITY : Stack/Heap Overhead: Governed by object payload sizes.
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
// HELPER CLASS 1: LIFETIME TRACKER
// Logs construction, destruction, and active instance counts to trace object lifecycles.
// =====================================================================================
class LifetimeTracker {
private:
    int id_;
    string tag_;

public:
    static inline int activeCount = 0; // C++17 inline static variable

    LifetimeTracker(int id, string tag) : id_(id), tag_(std::move(tag)) {
        ++activeCount;
        cout << "    [CTOR] Object ID: " << setw(3) << id_ << " (" << setw(18) << left << tag_ 
             << ") created at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    ~LifetimeTracker() {
        --activeCount;
        cout << "    [DTOR] Object ID: " << setw(3) << id_ << " (" << setw(18) << left << tag_ 
             << ") destroyed at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    // Disable copy semantics to keep tracking unambiguous
    LifetimeTracker(const LifetimeTracker&) = delete;
    LifetimeTracker& operator=(const LifetimeTracker&) = delete;

    // Enable move semantics
    LifetimeTracker(LifetimeTracker&& other) noexcept 
        : id_(other.id_), tag_(std::move(other.tag_) + "_Moved") {
        other.id_ = -1;
        cout << "    [MOVE CTOR] Transferred to ID: " << id_ << " (" << tag_ << ")\n";
    }

    LifetimeTracker& operator=(LifetimeTracker&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            tag_ = std::move(other.tag_) + "_Moved";
            other.id_ = -1;
            cout << "    [MOVE ASSIGN] Transferred to ID: " << id_ << " (" << tag_ << ")\n";
        }
        return *this;
    }

    void execute() const {
        if (id_ != -1) {
            cout << "    [EXECUTE] ID: " << id_ << " (" << tag_ << ") processing work.\n";
        }
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getTag() const { return tag_; }
};

// =====================================================================================
// HELPER CLASS 2: COMPOSITE OBJECT (MEMBER LIFETIME DEMONSTRATION)
// Demonstrates that class members are constructed BEFORE parent body and destructed AFTER.
// =====================================================================================
class ParentComposite {
private:
    LifetimeTracker member1_;
    LifetimeTracker member2_;

public:
    ParentComposite(int baseId, const string& parentTag)
        : member1_(baseId, parentTag + "::Member1"),
          member2_(baseId + 1, parentTag + "::Member2") {
        cout << "    [PARENT CTOR BODY] ParentComposite object fully initialized.\n";
    }

    ~ParentComposite() {
        cout << "    [PARENT DTOR BODY] ParentComposite destructor body executing...\n";
    }

    void executeMembers() const {
        member1_.execute();
        member2_.execute();
    }
};

// =====================================================================================
// HELPER FUNCTIONS FOR TEMPORARY AND EXCEPTION DEMONSTRATIONS
// =====================================================================================
LifetimeTracker createTemporaryTracker(int id, const string& tag) {
    return LifetimeTracker(id, tag);
}

void exceptionTriggeringScope(int baseId) {
    cout << "    [SCOPE ENTER] Creating stack objects before exception...\n";
    LifetimeTracker obj1(baseId, "StackObjBeforeError1");
    LifetimeTracker obj2(baseId + 1, "StackObjBeforeError2");

    obj1.execute();
    obj2.execute();

    cout << "    [EXCEPTION THROW] Throwing exception during scope execution...\n";
    throw runtime_error("Simulated execution failure during scope unwinding");

    // Code below is unreachable, but stack unwinding will destroy obj2 then obj1
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for object lifetime tracking (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. AUTOMATIC (STACK) STORAGE DURATION & LIFO ORDER
    // =====================================================================================
    cout << "\n================ 1. AUTOMATIC STACK LIFETIME (LIFO DESTRUCTION) ================\n";

    {
        cout << "  - Entering local block scope...\n";
        LifetimeTracker first(userInputValue, "StackFirst");
        LifetimeTracker second(userInputValue + 1, "StackSecond");
        LifetimeTracker third(userInputValue + 2, "StackThird");

        cout << "  - Inside block scope (All 3 objects alive).\n";
        cout << "  - Leaving local block scope (Notice LIFO Destruction Order: Third -> Second -> First):\n";
    } // Local objects destroyed HERE in reverse construction order (LIFO)

    cout << "  - Active Instances Post-Scope: " << LifetimeTracker::activeCount << "\n";

    // =====================================================================================
    // 2. MEMBER SUB-OBJECT LIFETIME & PARENT COMPOSITION
    // =====================================================================================
    cout << "\n================ 2. MEMBER SUB-OBJECT CONSTRUCTION & TEARDOWN ================\n";

    {
        cout << "  - Instantiating ParentComposite object...\n";
        ParentComposite parent(userInputValue + 10, "ParentObj");

        cout << "  - Executing methods on ParentComposite...\n";
        parent.executeMembers();

        cout << "  - Exiting scope containing ParentComposite...\n";
        // Destructor sequence:
        // 1. Parent Dtor Body executes
        // 2. member2_ Dtor executes (reverse order of declaration)
        // 3. member1_ Dtor executes
    }

    cout << "  - Active Instances Post-Parent Exit: " << LifetimeTracker::activeCount << "\n";

    // =====================================================================================
    // 3. TEMPORARY OBJECT LIFETIME & LIFETIME EXTENSION
    // =====================================================================================
    cout << "\n================ 3. TEMPORARY OBJECT LIFETIME EXTENSION ================\n";

    {
        cout << "  - Case A: Unbound temporary object (Destructed at end of full expression):\n";
        createTemporaryTracker(userInputValue + 20, "UnboundTemp").execute();
        cout << "  - Expression finished. Notice unbound temporary was already destructed above!\n";

        cout << "\n  - Case B: Binding temporary to `const LifetimeTracker&` (Lifetime Extended!):\n";
        const LifetimeTracker& boundRef = createTemporaryTracker(userInputValue + 21, "BoundTempRef");
        boundRef.execute();

        cout << "  - Reached end of block scope containing `boundRef`...\n";
    } // Bound temporary destructed HERE when reference goes out of scope!

    cout << "  - Active Instances Post-Lifetime Extension: " << LifetimeTracker::activeCount << "\n";

    // =====================================================================================
    // 4. DYNAMIC (HEAP) STORAGE DURATION MANAGED BY RAII
    // =====================================================================================
    cout << "\n================ 4. DYNAMIC HEAP LIFETIME & RAII WRAPPERS ================\n";

    {
        cout << "  - Binding dynamic heap allocation to `std::unique_ptr`...\n";
        auto smartHeapObj = std::make_unique<LifetimeTracker>(userInputValue + 30, "RaiiHeapObj");
        smartHeapObj->execute();

        cout << "  - Exiting scope containing smart pointer...\n";
    } // std::unique_ptr dtor runs HERE, automatically deleting dynamic object!

    cout << "  - Active Instances Post-Heap Exit: " << LifetimeTracker::activeCount << "\n";

    // =====================================================================================
    // 5. LIFETIME DURING EXCEPTION STACK UNWINDING
    // =====================================================================================
    cout << "\n================ 5. LIFETIME DURING EXCEPTION STACK UNWINDING ================\n";

    try {
        cout << "  - Entering try block...\n";
        exceptionTriggeringScope(userInputValue + 40);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Instances Post-Exception Unwind: " << LifetimeTracker::activeCount 
         << " (RAII GUARANTEE: Zero leaks!)\n";

    // =====================================================================================
    // 6. STATIC STORAGE DURATION LIFETIME
    // =====================================================================================
    cout << "\n================ 6. STATIC STORAGE DURATION ================\n";

    auto demonstrateStatic = [userInputValue]() {
        cout << "    [INSIDE FUNCTION] Accessing function-local static object...\n";
        static LifetimeTracker staticObj(userInputValue + 50, "StaticFunctionObj");
        staticObj.execute();
    };

    cout << "  - First call to `demonstrateStatic()` (Static object initializes):\n";
    demonstrateStatic();

    cout << "  - Second call to `demonstrateStatic()` (Static object reused, no second ctor):\n";
    demonstrateStatic();

    cout << "  - Exiting `main()` (Static object will persist until program termination)...\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ OBJECT LIFETIME SUMMARY ================\n";
    cout << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Storage Duration      | Construction Point    | Destruction Point     | Primary RAII Operational Trait    |\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n"
         << "| Automatic (Stack)     | Variable declaration  | Lexical scope exit    | Deterministic LIFO teardown order |\n"
         << "| Member Sub-Objects    | Before Parent Ctor    | After Parent Dtor     | Teardown in reverse declaration   |\n"
         << "| Unbound Temporary     | Expression evaluation | End of full-statement | Extremely short-lived lifetime    |\n"
         << "| Bound Temp Reference  | Reference binding     | Reference scope exit  | Extended lifetime via const/rvalue|\n"
         << "| RAII Heap (`unique_ptr`)| Wrapper instantiation | Wrapper scope exit    | Automated heap memory lifecycle   |\n"
         << "| Exception Unwinding   | Prior to throw        | Stack unwind exit     | Guaranteed leak-free unwinding    |\n"
         << "| Static Storage        | First execution path  | Program termination   | Single initialization, global scope|\n"
         << "+-----------------------+-----------------------+-----------------------+-----------------------------------+\n";

    return 0;
}