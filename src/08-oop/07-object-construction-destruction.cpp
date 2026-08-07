/*
 * =====================================================================================
 * CONCEPT        : Object Construction and Destruction in OOP (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the exact mechanics, guarantees, and ordering rules governing 
 *                  Object Construction and Destruction in Modern C++:
 *
 *                  1. Construction Order Mechanics : 
 *                     - Base class sub-objects (top-down in inheritance order).
 *                     - Member sub-objects (in exact order of class DECLARATION).
 *                     - Derived class constructor body execution.
 *
 *                  2. Destruction Order Mechanics  :
 *                     - Derived class destructor body execution.
 *                     - Member sub-objects in REVERSE order of declaration.
 *                     - Base class sub-objects in REVERSE order of inheritance.
 *
 *                  3. Exception during Construction :
 *                     - If a constructor throws an exception, the object's OWN destructor 
 *                       is NEVER called.
 *                     - Fully constructed member and base sub-objects are destructed in 
 *                       REVERSE order of their construction to prevent resource leaks.
 *
 *                  4. Virtual Destructors           :
 *                     - Guarantees correct polymorphic destruction sequence when deleting 
 *                       derived objects via base class pointers.
 *
 *                  5. Lifetime Categories           :
 *                     - Automatic (Stack LIFO order), Dynamic (Heap/RAII), and Static.
 *
 * RESOLVED WARNING: Member initializer list for `DerivedObject` matches the exact declaration
 *                  order of member variables (`member1_` then `member2_`), resolving GCC/Clang 
 *                  `-Wreorder` compiler warnings.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Access : O(1) constant time.
 * SPACE COMPLEXITY : Object Footprint : Sum of member sizes (+ 8 bytes vptr if virtual functions exist).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MEMBER SUB-OBJECT
// Used to trace member construction/destruction ordering relative to class declarations.
// =====================================================================================
class MemberSubObject {
private:
    string memberName_;

public:
    explicit MemberSubObject(string name) : memberName_(std::move(name)) {
        cout << "      [Member Ctor] Constructing member: '" << memberName_ << "'\n";
    }

    ~MemberSubObject() noexcept {
        cout << "      [Member Dtor] Destructing member: '" << memberName_ << "'\n";
    }
};

// =====================================================================================
// 1. BASE AND DERIVED CLASSES DEMONSTRATING HIERARCHICAL CONSTRUCTION & DESTRUCTION
// =====================================================================================
class BaseObject {
protected:
    int baseId_;

public:
    explicit BaseObject(int id) : baseId_(id) {
        cout << "    [1. Base Ctor] Base sub-object initialized (ID: " << baseId_ << ")\n";
    }

    // Essential Virtual Destructor for safe polymorphic teardown
    virtual ~BaseObject() noexcept {
        cout << "    [3. Base Dtor] Base sub-object destructed (ID: " << baseId_ << ")\n";
    }

    virtual void executeWork() const {
        cout << "    [Base Work] Base ID: " << baseId_ << " processing.\n";
    }
};

class DerivedObject : public BaseObject {
private:
    // Memory Layout Order: member1_ is declared FIRST, member2_ is declared SECOND.
    // Modern C++ constructs members strictly in DECLARATION order!
    MemberSubObject member1_;
    MemberSubObject member2_;
    double payloadData_;

public:
    // Initializer list order matches declaration order (`member1_` then `member2_`)
    // to comply with C++ standards and eliminate -Wreorder warnings.
    DerivedObject(int id, string m1Name, string m2Name, double data)
        : BaseObject(id), 
          member1_(m1Name), // Constructed FIRST matching declaration order
          member2_(m2Name), // Constructed SECOND matching declaration order
          payloadData_(data) {
        cout << "    [2. Derived Ctor Body] Derived object fully constructed with payload: " 
             << payloadData_ << "\n";
    }

    ~DerivedObject() noexcept override {
        cout << "    [1. Derived Dtor Body] Derived destructor body executing...\n";
        // Destructor executes in exact REVERSE order of declaration:
        // 1. member2_ destructs FIRST
        // 2. member1_ destructs SECOND
        // 3. BaseObject destructs THIRD
    }

    void executeWork() const override {
        cout << "    [Derived Work] Base ID: " << baseId_ 
             << " | Payload Data: " << payloadData_ << "\n";
    }
};

// =====================================================================================
// 2. CLASS DEMONSTRATING EXCEPTION HANDLING DURING CONSTRUCTION
// Shows stack unwinding of partially constructed objects.
// =====================================================================================
class FailingConstructionDemo {
private:
    MemberSubObject m1_;
    MemberSubObject m2_;

public:
    FailingConstructionDemo(bool throwError)
        : m1_("SubMember_1 (Constructed First)"), 
          m2_("SubMember_2 (Constructed Second)") {
        cout << "    [FailingDemo Ctor Body] Entering constructor body...\n";
        
        if (throwError) {
            cout << "    [FailingDemo Exception] Throwing exception inside Ctor body!\n";
            throw runtime_error("Construction aborted midway!");
        }

        cout << "    [FailingDemo Ctor Body] Construction completed successfully.\n";
    }

    ~FailingConstructionDemo() noexcept {
        // CRITICAL C++ RULE: If constructor throws, destructor is NEVER called!
        cout << "    [FailingDemo Dtor] THIS WILL NEVER RUN IF CTOR THROWS!\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Construction/Destruction analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. CONSTRUCTION AND DESTRUCTION ORDERING (BASE -> MEMBERS -> DERIVED)
    // =====================================================================================
    cout << "\n================ 1. HIERARCHICAL CONSTRUCTION & DESTRUCTION ORDER ================\n";

    {
        cout << "  - Instantiating `DerivedObject` on stack...\n";
        cout << "  - [CONSTRUCTION STEPS]: Base Ctor -> Member Decl Order (m1 then m2) -> Derived Body\n\n";

        DerivedObject derivedObj(userInputValue, "FirstDeclaredMember", "SecondDeclaredMember", 3.14159);

        cout << "\n  - Executing object method...\n";
        derivedObj.executeWork();

        cout << "\n  - Reaching end of block scope...\n";
        cout << "  - [DESTRUCTION STEPS]: Derived Body -> Members Reverse Order (m2 then m1) -> Base Dtor\n\n";
    } // Derived object destructs HERE in exact reverse order

    // =====================================================================================
    // 2. STACK LIFETIME & LIFO (LAST-IN, FIRST-OUT) TEARDOWN
    // =====================================================================================
    cout << "\n================ 2. STACK LIFO LIFECYCLE ================\n";

    {
        cout << "  - Creating 3 stack-bound objects in sequence: ObjA -> ObjB -> ObjC...\n";
        MemberSubObject objA("StackObj_A");
        MemberSubObject objB("StackObj_B");
        MemberSubObject objC("StackObj_C");

        cout << "\n  - Exiting stack scope (Watch LIFO reverse destruction order: ObjC -> ObjB -> ObjA):\n";
    }

    // =====================================================================================
    // 3. POLYMORPHIC DYNAMIC LIFETIME (`unique_ptr` & VIRTUAL DESTRUCTORS)
    // =====================================================================================
    cout << "\n================ 3. POLYMORPHIC DYNAMIC DESTRUCTION ================\n";

    {
        cout << "  - Allocating `DerivedObject` via Base Pointer (`unique_ptr<BaseObject>`):\n";
        unique_ptr<BaseObject> polyPtr = make_unique<DerivedObject>(
            userInputValue + 10, "PolyMember1", "PolyMember2", 99.9
        );

        polyPtr->executeWork();

        cout << "\n  - Releasing polymorphic object via smart pointer...\n";
        cout << "  - [VIRTUAL DTOR GUARANTEE]: Virtual destructor triggers full derived cleanup sequence:\n";
    } // Memory freed and destructors executed cleanly HERE

    // =====================================================================================
    // 4. EXCEPTION DURING CONSTRUCTION & CLEANUP GUARANTEES
    // =====================================================================================
    cout << "\n================ 4. EXCEPTION HANDLING DURING CONSTRUCTION ================\n";

    try {
        cout << "  - Attempting to construct `FailingConstructionDemo` with error flag = TRUE...\n";
        FailingConstructionDemo failingObj(true);
    } catch (const exception& e) {
        cout << "\n  - [EXCEPTION CAUGHT IN MAIN]: \"" << e.what() << "\"\n";
        cout << "  - [RAII GUARANTEE]: Member sub-objects m2_ and m1_ were destructed in reverse order,\n"
             << "    but FailingConstructionDemo's destructor was SKIPPED completely!\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ OBJECT CONSTRUCTION AND DESTRUCTION SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Phase / Scenario      | Sequence / Rule                   | Architectural Guarantee           |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Construction Order    | Base -> Members -> Derived Body   | Sub-objects ready before usage    |\n"
         << "| Member Ctor Order     | Order of Class DECLARATION        | Matches declaration order         |\n"
         << "| Destruction Order     | Derived Body -> Members -> Base   | Exact reverse of construction     |\n"
         << "| Member Dtor Order     | REVERSE of Class DECLARATION      | Teardown in reverse dependency    |\n"
         << "| Stack Lifecycle       | LIFO (Last-In, First-Out)         | Deterministic scope teardown      |\n"
         << "| Ctor Exception        | Partial sub-objects destructed    | Dtor skipped; ZERO memory leaks   |\n"
         << "| Polymorphic Teardown  | Require `virtual ~Base() noexcept`| Prevents incomplete derived leak  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}