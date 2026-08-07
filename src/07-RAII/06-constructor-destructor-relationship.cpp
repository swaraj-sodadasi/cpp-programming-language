/*
 * =====================================================================================
 * CONCEPT        : Constructor-Destructor Relationship in RAII (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the strict dual symmetry between constructors and destructors in RAII:
 *
 *                  1. Dual Symmetry Principle     : Constructor acquires the resource (Resource 
 *                                                   Acquisition Is Initialization), Destructor 
 *                                                   releases the resource upon scope exit.
 *                  2. LIFO Stack Execution Order  : Objects constructed in sequence (A -> B -> C) 
 *                                                   are strictly destructed in reverse order 
 *                                                   (C -> B -> A).
 *                  3. Class Composition Symmetry  : Member sub-objects constructed first-to-last, 
 *                                                   destructed last-to-first relative to the parent.
 *                  4. Exception Handling Symmetry : Stack unwinding ensures every fully constructed 
 *                                                   object's destructor is executed when an 
 *                                                   exception is thrown.
 *                  5. Non-Memory Resource Pairs   : Symmetrical operations like Lock/Unlock, 
 *                                                   Open/Close, Connect/Disconnect.
 *
 * TIME COMPLEXITY  : Construction / Destruction / Scope Traversal: O(1) constant time.
 * SPACE COMPLEXITY : RAII Object Footprint: Uniform 8-16 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: SYMMETRICAL RESOURCE TRACKER
// Demonstrates exact constructor acquisition vs destructor release pairing.
// =====================================================================================
class SymmResource {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    // CONSTRUCTOR: ACQUISITION PHASE
    SymmResource(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [1. CONSTRUCTOR (Acquire)]  ID: " << setw(3) << id_ << " (" << setw(16) << left << label_ 
             << ") created at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    // DESTRUCTOR: RELEASE PHASE (Strict Dual of Constructor)
    ~SymmResource() noexcept {
        --activeCount;
        cout << "    [2. DESTRUCTOR  (Release)]  ID: " << setw(3) << id_ << " (" << setw(16) << left << label_ 
             << ") destroyed at " << static_cast<const void*>(this) 
             << " | Active: " << activeCount << "\n";
    }

    // Disable copies to preserve strict 1:1 Ctor/Dtor symmetry
    SymmResource(const SymmResource&) = delete;
    SymmResource& operator=(const SymmResource&) = delete;

    // Move Constructor (Transfers resource ownership)
    SymmResource(SymmResource&& other) noexcept 
        : id_(other.id_), label_(std::move(other.label_) + "_Moved") {
        other.id_ = -1;
        cout << "    [MOVE CONSTRUCTOR]          Resource ID: " << id_ << " ownership transferred.\n";
    }

    SymmResource& operator=(SymmResource&& other) noexcept {
        if (this != &other) {
            id_ = other.id_;
            label_ = std::move(other.label_) + "_Moved";
            other.id_ = -1;
            cout << "    [MOVE ASSIGNMENT]           Resource ID: " << id_ << " ownership transferred.\n";
        }
        return *this;
    }

    void performWork() const {
        if (id_ != -1) {
            cout << "    [EXECUTE]                   ID: " << id_ << " (" << label_ << ") performing action.\n";
        }
    }

    [[nodiscard]] int getId() const { return id_; }
};

// =====================================================================================
// HELPER CLASS 2: COMPOSITE RESOURCE (SUB-OBJECT CTOR/DTOR SYMMETRY)
// Demonstrates nested constructor and destructor call order in object composition.
// =====================================================================================
class CompositeRaiiPair {
private:
    SymmResource memberA_;
    SymmResource memberB_;

public:
    CompositeRaiiPair(int baseId, const string& parentTag)
        : memberA_(baseId, parentTag + "_MemberA"),
          memberB_(baseId + 1, parentTag + "_MemberB") {
        cout << "  --> [PARENT CTOR BODY]        Composite parent fully initialized.\n";
    }

    ~CompositeRaiiPair() noexcept {
        cout << "  <-- [PARENT DTOR BODY]        Composite parent teardown starting...\n";
    }

    void execute() const {
        memberA_.performWork();
        memberB_.performWork();
    }
};

// =====================================================================================
// HELPER CLASS 3: NON-MEMORY SYMMETRICAL OPERATION WRAPPER
// Demonstrates stateful acquire/release pairings (e.g., Lock/Unlock or Open/Close).
// =====================================================================================
class SymmetricalStateLock {
private:
    string lockName_;

public:
    // CONSTRUCTOR: Locks the stateful entity
    explicit SymmetricalStateLock(string lockName) : lockName_(std::move(lockName)) {
        cout << "    [LOCK CTOR]   Acquired lock on '" << lockName_ << "'\n";
    }

    // DESTRUCTOR: Unlocks the stateful entity
    ~SymmetricalStateLock() noexcept {
        cout << "    [LOCK DTOR]   Released lock on '" << lockName_ << "'\n";
    }

    void executeCriticalSection() const {
        cout << "    [CRITICAL REGION] Executing thread-safe operation under '" << lockName_ << "'...\n";
    }

    // Prevent copies and moves for scoped lock invariants
    SymmetricalStateLock(const SymmetricalStateLock&) = delete;
    SymmetricalStateLock& operator=(const SymmetricalStateLock&) = delete;
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Ctor-Dtor relationship testing (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THE BASIC SYMMETRY RULE (ACQUISITION vs RELEASE)
    // =====================================================================================
    cout << "\n================ 1. BASIC SYMMETRY: CTOR ACQUIRES, DTOR RELEASES ================\n";

    {
        cout << "  - Entering local block scope...\n";
        SymmResource singleRes(userInputValue, "SingleScopeRes");
        singleRes.performWork();

        cout << "  - Leaving local block scope (Destructor will run immediately):\n";
    } // Constructor acquired resource at entry; Destructor releases resource HERE!

    cout << "  - Active Resources Post-Scope: " << SymmResource::activeCount << "\n";

    // =====================================================================================
    // 2. LIFO EXECUTION SYMMETRY (STACK CONSTRUCTION vs DESTRUCTION ORDER)
    // =====================================================================================
    cout << "\n================ 2. LIFO EXECUTION SYMMETRY (STACK REVERSAL) ================\n";

    {
        cout << "  - Constructing 3 objects sequentially: ResA -> ResB -> ResC...\n";
        SymmResource resA(userInputValue + 10, "ResA_First");
        SymmResource resB(userInputValue + 11, "ResB_Second");
        SymmResource resC(userInputValue + 12, "ResC_Third");

        cout << "  - Exiting scope (Watch destruction order: ResC -> ResB -> ResA):\n";
    } // Strict LIFO (Last-In, First-Out) reversal guarantee

    cout << "  - Active Resources Post-LIFO Scope: " << SymmResource::activeCount << "\n";

    // =====================================================================================
    // 3. NESTED SUB-OBJECT CONSTRUCTION & DESTRUCTION SYMMETRY
    // =====================================================================================
    cout << "\n================ 3. COMPOSITION SYMMETRY (MEMBER SUB-OBJECTS) ================\n";

    {
        cout << "  - Instantiating CompositeRaiiPair object...\n";
        CompositeRaiiPair composite(userInputValue + 20, "CompGroup");
        composite.execute();

        cout << "  - Exiting scope containing composite object...\n";
        // Order of events:
        // 1. MemberA Ctor -> MemberB Ctor -> Parent Ctor Body
        // 2. Parent Dtor Body -> MemberB Dtor -> MemberA Dtor
    }

    cout << "  - Active Resources Post-Composite Scope: " << SymmResource::activeCount << "\n";

    // =====================================================================================
    // 4. EXCEPTION HANDLING SYMMETRY (STACK UNWINDING)
    // =====================================================================================
    cout << "\n================ 4. EXCEPTION HANDLING SYMMETRY (STACK UNWINDING) ================\n";

    try {
        cout << "  - Entering try block...\n";
        SymmResource unwindA(userInputValue + 30, "UnwindA");
        SymmResource unwindB(userInputValue + 31, "UnwindB");

        cout << "  - Throwing exception... Stack unwinding will destroy unwindB then unwindA before catch block!\n";
        throw runtime_error("Simulated error forcing stack unwind");

        // Unreachable code
    } catch (const exception& e) {
        cout << "  - [CATCH BLOCK EXECUTED] Caught Exception: \"" << e.what() << "\"\n";
    }

    cout << "  - Active Resources Post-Exception: " << SymmResource::activeCount 
         << " (RAII SYMMETRY GUARANTEE: Zero leaks!)\n";

    // =====================================================================================
    // 5. NON-MEMORY SYMMETRICAL PAIRINGS (LOCK / UNLOCK)
    // =====================================================================================
    cout << "\n================ 5. NON-MEMORY SYMMETRICAL PAIRINGS ================\n";

    {
        cout << "  - Entering thread-safe critical section...\n";
        SymmetricalStateLock lock("DatabaseTransactionMutex");
        lock.executeCriticalSection();

        cout << "  - Exiting critical section block...\n";
    } // Destructor automatically unlocks mutex

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CONSTRUCTOR-DESTRUCTOR RELATIONSHIP SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Symmetry Phase        | Constructor Role (`Ctor`)          | Destructor Role (`Dtor`)          |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Resource Lifecycle    | ACQUIRES resource (`new`, `open`) | RELEASES resource (`delete`, `close`)|\n"
         << "| Execution Timing      | Executed on object initialization | Executed automatically on scope exit|\n"
         << "| Stack Order (LIFO)    | Executed First-to-Last (A -> B)   | Executed Last-to-First (B -> A)   |\n"
         << "| Composition Order     | Member Ctors run BEFORE Parent Body| Member Dtors run AFTER Parent Body|\n"
         << "| Exception Behavior    | Establishes object invariants     | Auto-cleans fully constructed objects|\n"
         << "| Stateful Operations   | Locks mutex / Opens connection    | Unlocks mutex / Closes connection |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}