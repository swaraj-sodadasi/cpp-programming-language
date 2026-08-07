/*
 * =====================================================================================
 * CONCEPT        : Member Functions in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the complete taxonomy, semantics, and qualifiers of Member Functions:
 *
 *                  1. Instance Member Functions   : Standard accessors (getters) and mutators (setters)
 *                                                   operating on individual object state.
 *                  2. Const Member Functions       : Const-correct functions guaranteeing read-only access
 *                                                   to object state.
 *                  3. Static Member Functions      : Class-scoped functions that operate on static data or
 *                                                   act as factory functions without a `this` pointer.
 *                  4. Virtual & Pure Virtual Funcs : Dynamic dispatch mechanisms enabling polymorphism,
 *                                                   `override`, and `final` specifiers.
 *                  5. Ref-Qualified Functions      : Overloading member functions based on whether the
 *                                                   calling object is an lvalue (`&`) or rvalue (`&&`).
 *                  6. Inline, Constexpr & Noexcept : Performance qualifiers for compile-time evaluation,
 *                                                   inlining, and non-throwing exception contracts.
 *                  7. Special Function Control     : Explicitly defaulting (`= default`) or deleting (`= delete`)
 *                                                   member functions, and enforcing `explicit` constructors.
 *                  8. Friend Functions             : Non-member functions granted access to private class members.
 *
 * TIME COMPLEXITY  : Direct / Inline Calls         : O(1) constant time.
 *                    Virtual Member Function Calls : O(1) constant time (via vtable indirection).
 * SPACE COMPLEXITY : Overhead per Object Instance  : 0 bytes for member functions (+8 bytes vptr if virtual).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// 1. INSTANCE, CONST, REF-QUALIFIED, AND SPECIAL MEMBER FUNCTIONS
// Demonstrates accessors, mutators, ref-qualifiers (& / &&), noexcept, and constexpr.
// =====================================================================================
class DataBuffer {
private:
    int bufferId_;
    vector<int> payload_;

public:
    // Explicit Parameterized Constructor
    explicit DataBuffer(int id, vector<int> payload = {})
        : bufferId_(id), payload_(std::move(payload)) {
        if (id < 0) {
            throw invalid_argument("Buffer ID cannot be negative!");
        }
        cout << "    [DataBuffer Ctor] Created Buffer ID: " << bufferId_ 
             << " with " << payload_.size() << " elements.\n";
    }

    // Defaulted Destructor
    ~DataBuffer() noexcept = default;

    // Deleted Copy Operations (Enforcing move-only semantics)
    DataBuffer(const DataBuffer&) = delete;
    DataBuffer& operator=(const DataBuffer&) = delete;

    // Defaulted Move Operations
    DataBuffer(DataBuffer&&) noexcept = default;
    DataBuffer& operator=(DataBuffer&&) noexcept = default;

    // Instance Mutator Member Function
    void appendData(int value) {
        payload_.push_back(value);
        cout << "    [MUTATOR] Appended " << value << " to Buffer ID " << bufferId_ << "\n";
    }

    // Const Accessor Member Function (Read-only guarantee)
    [[nodiscard]] int getBufferId() const noexcept {
        return bufferId_;
    }

    [[nodiscard]] size_t getSize() const noexcept {
        return payload_.size();
    }

    // Constexpr Member Function (Evaluated at compile-time when possible)
    [[nodiscard]] constexpr static int getMinCapacity() noexcept {
        return 16;
    }

    // =================================================================================
    // REF-QUALIFIED MEMBER FUNCTIONS (& vs &&)
    // Overloads behavior depending on whether object is an lvalue or temporary rvalue.
    // =================================================================================

    // Called when the object is an LVALUE (Returns const reference)
    [[nodiscard]] const vector<int>& getPayload() const & {
        cout << "    [REF-QUALIFIER] Executing getPayload() & (Lvalue Context - Copying Avoided)\n";
        return payload_;
    }

    // Called when the object is an RVALUE / Temporary (Moves data out directly)
    [[nodiscard]] vector<int> getPayload() && noexcept {
        cout << "    [REF-QUALIFIER] Executing getPayload() && (Rvalue Context - Moving Payload Out)\n";
        return std::move(payload_);
    }
};

// =====================================================================================
// 2. STATIC MEMBER FUNCTIONS & FACTORY PATTERN
// Demonstrates static functions operating without a `this` pointer and friend access.
// =====================================================================================
class SystemService {
private:
    int serviceId_;
    string serviceName_;
    bool isActive_;

    static inline int totalServicesCreated = 0; // C++17 inline static member

    // Private constructor enforcing creation through Factory Member Function
    SystemService(int id, string name)
        : serviceId_(id), serviceName_(std::move(name)), isActive_(false) {
        ++totalServicesCreated;
    }

public:
    ~SystemService() noexcept {
        cout << "    [SystemService Dtor] Terminated Service ID: " << serviceId_ << "\n";
    }

    // Static Factory Member Function (Static dispatch, no `this` pointer)
    [[nodiscard]] static SystemService createAndInitialize(int id, const string& name) {
        if (id <= 0) {
            throw invalid_argument("Service ID must be strictly positive!");
        }
        cout << "    [STATIC FACTORY] Constructing SystemService ID: " << id << " ('" << name << "')\n";
        SystemService service(id, name);
        service.isActive_ = true; // Initialize service state
        return service;
    }

    // Static Query Member Function
    [[nodiscard]] static int getTotalServicesCreated() noexcept {
        return totalServicesCreated;
    }

    // Inline Instance Member Function
    inline void executeTask(const string& taskName) const {
        if (isActive_) {
            cout << "    [INLINE METHOD] Service '" << serviceName_ << "' executing: " << taskName << "\n";
        }
    }

    // Declaration of Friend Function (Non-member function with private access)
    friend void inspectPrivateServiceDetails(const SystemService& service);
};

// Friend Function Definition
void inspectPrivateServiceDetails(const SystemService& service) {
    cout << "    [FRIEND FUNCTION] Direct Access -> ID: " << service.serviceId_ 
         << " | Name: " << service.serviceName_ 
         << " | Active: " << (service.isActive_ ? "TRUE" : "FALSE") << "\n";
}

// =====================================================================================
// 3. VIRTUAL, PURE VIRTUAL, OVERRIDE, AND FINAL MEMBER FUNCTIONS
// Demonstrates runtime polymorphism, dynamic dispatch, and interface contracts.
// =====================================================================================
class AbstractProcessor {
protected:
    int processorId_;

public:
    explicit AbstractProcessor(int id) : processorId_(id) {}

    // Essential Virtual Destructor
    virtual ~AbstractProcessor() noexcept {
        cout << "    [AbstractProcessor Dtor] Cleaned base processor ID: " << processorId_ << "\n";
    }

    // Pure Virtual Member Function (Defines Interface Contract)
    virtual void processDataBatch() = 0;

    // Virtual Member Function with Default Implementation
    virtual void printStatus() const {
        cout << "    [VIRTUAL BASE] AbstractProcessor ID: " << processorId_ << " Ready.\n";
    }
};

class AudioProcessor : public AbstractProcessor {
private:
    double sampleRateHz_;

public:
    AudioProcessor(int id, double sampleRate)
        : AbstractProcessor(id), sampleRateHz_(sampleRate) {}

    ~AudioProcessor() noexcept override {
        cout << "    [AudioProcessor Dtor] Releasing audio hardware buffers...\n";
    }

    // Overridden Virtual Member Function
    void processDataBatch() override {
        cout << "    [OVERRIDDEN VIRTUAL] AudioProcessor ID " << processorId_ 
             << " processing audio stream at " << sampleRateHz_ << " Hz\n";
    }

    // Overridden Virtual Member Function marked 'final' (Prevents further overriding)
    void printStatus() const override final {
        cout << "    [FINAL OVERRIDE] AudioProcessor ID: " << processorId_ 
             << " | Sample Rate: " << sampleRateHz_ << " Hz\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for member functions analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. INSTANCE, CONST, AND REF-QUALIFIED MEMBER FUNCTIONS
    // =====================================================================================
    cout << "\n================ 1. INSTANCE, CONST, AND REF-QUALIFIED FUNCTIONS ================\n";

    {
        cout << "  - Creating Lvalue DataBuffer object...\n";
        DataBuffer bufLvalue(userInputValue, {10, 20, 30});

        // Calling Mutator
        bufLvalue.appendData(40);

        // Calling Const Accessor
        cout << "    Buffer ID   : " << bufLvalue.getBufferId() << "\n";
        cout << "    Buffer Size : " << bufLvalue.getSize() << "\n";

        cout << "\n  - Demonstrating Ref-Qualified Overloading (& vs &&):\n";
        
        // Scenario A: Calling on Lvalue (Triggers getPayload() &)
        const auto& lvalueData = bufLvalue.getPayload();
        cout << "    Lvalue payload size: " << lvalueData.size() << " elements\n";

        // Scenario B: Calling on Rvalue / Temporary (Triggers getPayload() &&)
        cout << "    Extracting payload from temporary Rvalue DataBuffer...\n";
        vector<int> movedData = DataBuffer(userInputValue + 1, {100, 200, 300}).getPayload();
        cout << "    Moved Rvalue payload size: " << movedData.size() << " elements\n";

        cout << "\n  - Constexpr Member Function Evaluation:\n";
        constexpr int minCap = DataBuffer::getMinCapacity();
        cout << "    Compile-Time Minimum Capacity: " << minCap << " units\n";
    }

    // =====================================================================================
    // 2. STATIC MEMBER FUNCTIONS & FRIEND FUNCTIONS
    // =====================================================================================
    cout << "\n================ 2. STATIC FACTORY & FRIEND FUNCTIONS ================\n";

    {
        cout << "  - Invoking Static Factory Function `SystemService::createAndInitialize()`...\n";
        SystemService service = SystemService::createAndInitialize(userInputValue + 10, "DatabaseService");

        // Invoking Inline Instance Function
        service.executeTask("Flush Transaction Logs");

        // Invoking Friend Function
        cout << "  - Invoking Friend Function with private access privileges:\n";
        inspectPrivateServiceDetails(service);

        cout << "  - Static Query: Total Services Created = " 
             << SystemService::getTotalServicesCreated() << "\n";
        cout << "  - Exiting Service scope...\n";
    }

    // =====================================================================================
    // 3. VIRTUAL, PURE VIRTUAL, OVERRIDE, AND FINAL MEMBER FUNCTIONS
    // =====================================================================================
    cout << "\n================ 3. VIRTUAL FUNCTIONS & DYNAMIC DISPATCH ================\n";

    {
        cout << "  - Storing derived objects in `vector<unique_ptr<AbstractProcessor>>`...\n";
        vector<unique_ptr<AbstractProcessor>> processors;

        processors.push_back(make_unique<AudioProcessor>(userInputValue + 20, 44100.0));
        processors.push_back(make_unique<AudioProcessor>(userInputValue + 21, 96000.0));

        cout << "\n  - Dynamic Dispatch Execution through Base Class Pointers:\n";
        for (const auto& proc : processors) {
            proc->printStatus();       // Calls final overridden method
            proc->processDataBatch();  // Calls pure virtual implementation
        }

        cout << "\n  - Exiting polymorphic vector scope (Virtual destructors run in sequence)...\n";
    }

    // =====================================================================================
    // 4. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 4. MEMBER FUNCTION INVARIANT GUARDS ================\n";

    try {
        cout << "  - Attempting to pass invalid arguments to Static Factory Function...\n";
        auto invalidService = SystemService::createAndInitialize(-50, "InvalidService");
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ MEMBER FUNCTIONS IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Member Function Type  | Syntax / Qualifiers               | Primary Behavioral Characteristic |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Instance Function     | `void update();`                  | Mutates object-specific state     |\n"
         << "| Const Function        | `int get() const;`                | Read-only state inspection        |\n"
         << "| Static Function       | `static Type create();`           | Class-scoped, no `this` pointer   |\n"
         << "| Pure Virtual Function | `virtual void f() = 0;`           | Defines mandatory interface contract|\n"
         << "| Overridden Virtual    | `void f() override;`              | Runtime dynamic dispatch          |\n"
         << "| Final Overridden      | `void f() override final;`        | Blocks further derived overrides  |\n"
         << "| Ref-Qualified (&)     | `Type get() const &;`             | Lvalue callers (avoids copying)   |\n"
         << "| Ref-Qualified (&&)    | `Type get() &&;`                  | Rvalue callers (moves state out)  |\n"
         << "| Constexpr Function    | `constexpr int f();`              | Executable at compile-time        |\n"
         << "| Deleted Function      | `Type(const Type&) = delete;`     | Blocks invocation at compile-time |\n"
         << "| Defaulted Function    | `~Type() = default;`              | Requests compiler default logic   |\n"
         << "| Friend Function       | `friend void f(const Type&);`     | Non-member with private access    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}