/*
 * =====================================================================================
 * CONCEPT        : Constructors in RAII (Resource Acquisition Is Initialization)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the role of constructors in the RAII paradigm:
 *
 *                  1. Acquisition In Constructors : Binding resource allocation directly to 
 *                                                   object initialization (`Ctor`).
 *                  2. `explicit` Constructor Guard: Preventing dangerous implicit type 
 *                                                   conversions that cause unintended temporaries.
 *                  3. Delegating Constructors     : Reusing constructor initialization logic 
 *                                                   without code duplication.
 *                  4. Exception Safety in Ctors    : Function try blocks, sub-object automatic 
 *                                                   destruction, and constructor rollback.
 *                  5. Rule of 5 Ctor Mechanics    : Suppressing copy constructors (`= delete`) and 
 *                                                   implementing `noexcept` move constructors.
 *                  6. Factory Constructors        : Static named constructors for validated or 
 *                                                   complex RAII object instantiation.
 *
 * TIME COMPLEXITY  : Construction / Move / Deallocation: O(1) constant time.
 * SPACE COMPLEXITY : RAII Handle Overhead: Uniform 8 bytes (raw pointer/handle size).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: RAW HANDLE TRACKER
// Tracks active resource allocations to visually verify clean acquisition/rollback.
// =====================================================================================
class RawResourceNode {
private:
    int id_;
    string label_;

public:
    static inline int activeCount = 0; // C++17 inline static tracker

    RawResourceNode(int id, string label) : id_(id), label_(std::move(label)) {
        ++activeCount;
        cout << "    [RAW RESOURCE ALLOCATED] ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Count: " << activeCount << "\n";
    }

    ~RawResourceNode() {
        --activeCount;
        cout << "    [RAW RESOURCE FREED]     ID: " << id_ << " (" << label_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active Count: " << activeCount << "\n";
    }

    void execute() const {
        cout << "    [EXECUTE] Processing node ID: " << id_ << " (" << label_ << ")\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getLabel() const { return label_; }
};

// =====================================================================================
// HELPER CLASS 2: EXPLICIT & DELEGATING RAII CONSTRUCTOR DEMO
// =====================================================================================
class ExplicitRaiiBuffer {
private:
    RawResourceNode* resource_;
    size_t capacity_;

public:
    // 1. Primary Constructor: Direct Acquisition
    explicit ExplicitRaiiBuffer(int id, string label, size_t capacity)
        : resource_(new RawResourceNode(id, std::move(label))), capacity_(capacity) {
        cout << "    [ExplicitRaiiBuffer Primary Ctor] Resource acquired with capacity " 
             << capacity_ << ".\n";
    }

    // 2. Delegating Constructor: Delegates default capacity to primary constructor
    explicit ExplicitRaiiBuffer(int id, string label)
        : ExplicitRaiiBuffer(id, std::move(label), 1024) { // Delegates to primary ctor
        cout << "    [ExplicitRaiiBuffer Delegating Ctor] Delegated construction completed.\n";
    }

    // Destructor
    ~ExplicitRaiiBuffer() {
        cout << "    [ExplicitRaiiBuffer Dtor] Cleaning up resource buffer...\n";
        delete resource_;
    }

    // Rule of 5: Delete copies, allow moves
    ExplicitRaiiBuffer(const ExplicitRaiiBuffer&) = delete;
    ExplicitRaiiBuffer& operator=(const ExplicitRaiiBuffer&) = delete;

    ExplicitRaiiBuffer(ExplicitRaiiBuffer&& other) noexcept 
        : resource_(other.resource_), capacity_(other.capacity_) {
        other.resource_ = nullptr;
        other.capacity_ = 0;
        cout << "    [ExplicitRaiiBuffer Move Ctor] Resource ownership moved.\n";
    }

    ExplicitRaiiBuffer& operator=(ExplicitRaiiBuffer&& other) noexcept {
        if (this != &other) {
            delete resource_;
            resource_ = other.resource_;
            capacity_ = other.capacity_;
            other.resource_ = nullptr;
            other.capacity_ = 0;
            cout << "    [ExplicitRaiiBuffer Move Assign] Previous freed, ownership moved.\n";
        }
        return *this;
    }

    void inspect() const {
        if (resource_) {
            resource_->execute();
        }
    }
};

// =====================================================================================
// HELPER CLASS 3: CONSTRUCTOR EXCEPTION SAFETY & FUNCTION TRY BLOCK
// Demonstrates automatic cleanup during constructor failure using RAII member sub-objects.
// =====================================================================================
class MultiResourceRaiiHolder {
private:
    std::unique_ptr<RawResourceNode> primaryRes_;
    std::unique_ptr<RawResourceNode> secondaryRes_;

public:
    // Constructor utilizing Function Try Block to log/translate acquisition failures.
    // Sub-objects (std::unique_ptr) are automatically destroyed BEFORE entering the catch block!
    MultiResourceRaiiHolder(int id1, int id2, bool simulateFailure) try
        : primaryRes_(std::make_unique<RawResourceNode>(id1, "PrimaryCtorRes")),
          secondaryRes_(nullptr) {
        
        cout << "    [MultiResource Ctor Body] Primary acquired. Attempting secondary acquisition...\n";
        if (simulateFailure) {
            throw std::runtime_error("Simulated failure during secondary resource acquisition!");
        }
        secondaryRes_ = std::make_unique<RawResourceNode>(id2, "SecondaryCtorRes");
        cout << "    [MultiResource Ctor Body] All resources acquired successfully.\n";

    } catch (...) {
        cout << "    [FUNCTION TRY BLOCK CATCH] Exception caught inside constructor function try block!\n";
        cout << "    [AUTOMATIC RAII ROLLBACK] Sub-objects (primaryRes_) were already destructed before catch block!\n";
        throw; // Function try blocks in constructors MUST rethrow exceptions
    }

    ~MultiResourceRaiiHolder() {
        cout << "    [MultiResource Dtor] Releasing multi-resource holder...\n";
    }
};

// =====================================================================================
// HELPER CLASS 4: STATIC FACTORY CONSTRUCTOR (NAMED CONSTRUCTOR IDIOM)
// Provides clean, validated acquisition interfaces for complex RAII setups.
// =====================================================================================
class FactoryRaiiResource {
private:
    std::unique_ptr<RawResourceNode> managedNode_;

    // Private constructor forces instantiation through static factory functions
    explicit FactoryRaiiResource(int id, string tag)
        : managedNode_(std::make_unique<RawResourceNode>(id, std::move(tag))) {
        cout << "    [Private Ctor] Initialized via Factory method.\n";
    }

public:
    // Static Factory Method: Validates parameters BEFORE invoking constructor
    static std::unique_ptr<FactoryRaiiResource> createValidated(int id, const string& tag) {
        if (id <= 0) {
            throw std::invalid_argument("Factory Validation Error: Resource ID must be positive!");
        }
        cout << "    [FACTORY METHOD] Arguments validated. Creating RAII instance...\n";
        return std::unique_ptr<FactoryRaiiResource>(new FactoryRaiiResource(id, tag));
    }

    void process() const {
        if (managedNode_) {
            managedNode_->execute();
        }
    }
};

// Function accepting explicit RAII handle to demonstrate `explicit` guard
void processBufferDirectly(const ExplicitRaiiBuffer& buffer) {
    buffer.inspect();
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for RAII constructor analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC CONSTRUCTOR ACQUISITION & DELEGATION
    // =====================================================================================
    cout << "\n================ 1. ACQUISITION IN CTORS & DELEGATION ================\n";

    {
        cout << "  - Instantiating `ExplicitRaiiBuffer` via Delegating Constructor...\n";
        ExplicitRaiiBuffer delegatingBuf(userInputValue, "DelegatedBuffer");
        delegatingBuf.inspect();

        cout << "  - Leaving scope (Buffer automatically destructs):\n";
    }

    cout << "  - Active Raw Resources Post-Scope: " << RawResourceNode::activeCount << "\n";

    // =====================================================================================
    // 2. THE IMPORTANCE OF `explicit` CONSTRUCTORS
    // =====================================================================================
    cout << "\n================ 2. THE `explicit` CONSTRUCTOR GUARD ================\n";

    ExplicitRaiiBuffer validBuf(userInputValue + 1, "ExplicitBuffer", 2048);

    // Modern C++ Best Practice: Direct explicit invocation
    processBufferDirectly(validBuf);

    // HAZARD PREVENTED BY `explicit`:
    // processBufferDirectly(userInputValue + 1); 
    // COMPILE ERROR: Implicit conversion from `int` to `ExplicitRaiiBuffer` is BLOCKED by `explicit`!
    // Without `explicit`, an invisible temporary object would be created and immediately destroyed.

    cout << "  - [SAFETY GUARANTEE]: `explicit` prevents implicit temporal RAII conversions.\n";

    // =====================================================================================
    // 3. CONSTRUCTOR EXCEPTION SAFETY & FUNCTION TRY BLOCKS
    // =====================================================================================
    cout << "\n================ 3. CONSTRUCTOR EXCEPTION SAFETY & ROLLBACK ================\n";

    cout << "  - [Case A]: Successful multi-resource construction...\n";
    {
        MultiResourceRaiiHolder safeHolder(userInputValue + 10, userInputValue + 11, false);
    }
    cout << "    Post-Case A Active Resources: " << RawResourceNode::activeCount << "\n";

    cout << "\n  - [Case B]: Failed constructor acquisition (Exception during construction)...\n";
    try {
        MultiResourceRaiiHolder failingHolder(userInputValue + 20, userInputValue + 21, true);
    } catch (const exception& e) {
        cout << "  - Caught Exception in main: \"" << e.what() << "\"\n";
    }

    cout << "  - [ROLLBACK GUARANTEE]: Active Resources Post-Ctor Failure = " 
         << RawResourceNode::activeCount << " (Primary resource cleaned up during ctor exception!)\n";

    // =====================================================================================
    // 4. MOVE CONSTRUCTOR MECHANICS IN RAII
    // =====================================================================================
    cout << "\n================ 4. MOVE CONSTRUCTOR & OWNERSHIP TRANSFER ================\n";

    {
        cout << "  - Creating initial RAII object `buf1`...\n";
        ExplicitRaiiBuffer buf1(userInputValue + 30, "MovableBuffer", 512);

        cout << "  - Invoking Move Constructor `buf2 = std::move(buf1)`...\n";
        ExplicitRaiiBuffer buf2 = std::move(buf1);

        cout << "  - Executing payload on new owner `buf2`:\n";
        buf2.inspect();

        cout << "  - Exiting scope (Only `buf2` holds active resource to free):\n";
    }

    cout << "  - Active Raw Resources Post-Move Scope: " << RawResourceNode::activeCount << "\n";

    // =====================================================================================
    // 5. FACTORY CONSTRUCTORS (NAMED CONSTRUCTOR IDIOM)
    // =====================================================================================
    cout << "\n================ 5. FACTORY CONSTRUCTORS (STATIC NAMED CTORS) ================\n";

    try {
        cout << "  - Attempting factory creation with valid ID (" << userInputValue + 40 << ")...\n";
        auto factoryObj = FactoryRaiiResource::createValidated(userInputValue + 40, "ValidFactoryRes");
        factoryObj->process();

        cout << "\n  - Attempting factory creation with invalid ID (-5)...\n";
        auto invalidObj = FactoryRaiiResource::createValidated(-5, "InvalidFactoryRes");
        (void)invalidObj;
    } catch (const exception& e) {
        cout << "  - Caught Factory Error: \"" << e.what() << "\"\n";
    }

    cout << "  - Final Active Raw Resources Count: " << RawResourceNode::activeCount << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CONSTRUCTORS IN RAII SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Constructor Feature   | Syntax / Implementation Pattern   | Primary RAII Benefit & Safety     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Acquisition Ctor      | `Type(Args...) : res_(new Res)`   | Guarantees valid object state     |\n"
         << "| `explicit` Guard      | `explicit Type(Param)`            | Blocks accidental implicit temp   |\n"
         << "| Delegating Ctor       | `Type(id) : Type(id, defaultVal)` | Eliminates duplicated setup logic |\n"
         << "| Function Try Block    | `Type(...) try : res1_(...) {}`   | Catches and translates ctor errors|\n"
         << "| Move Ctor (`noexcept`)| `Type(Type&&) noexcept`           | Transfers handle without re-alloc |\n"
         << "| Copy Ctor (`= delete`)| `Type(const Type&) = delete`      | Blocks double-free copy hazards   |\n"
         << "| Static Factory Ctor   | `static auto create(args...)`     | Pre-constructor state validation  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}