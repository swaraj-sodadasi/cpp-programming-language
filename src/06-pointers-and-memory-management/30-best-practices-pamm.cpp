/*
 * =====================================================================================
 * CONCEPT        : Best Practices in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  industry-standard best practices for memory management, safety,
 *                  and pointer mechanics in Modern C++:
 *
 *                  1. Favor RAII & Smart Pointers : Eliminate manual `new`/`delete` by default using 
 *                                                   `std::unique_ptr` and `std::shared_ptr`.
 *                  2. Use Factory Functions        : Prefer `std::make_unique` and `std::make_shared` 
 *                                                   for exception safety and performance.
 *                  3. Explicit Ownership Contracts: Pass by value/move for ownership transfer (sink), 
 *                                                   by reference (`const T&`) or raw pointer (`const T*`) 
 *                                                   for non-owning observation.
 *                  4. Rule of Zero / Rule of Five  : Utilize smart members to achieve Rule of Zero, 
 *                                                   or explicitly implement Rule of Five when managing raw handles.
 *                  5. Containers Over Raw Arrays   : Replace `new T[]` with `std::vector<T>` or `std::array<T, N>`.
 *                  6. Pointer Disarming & Safety  : Always initialize raw pointers (preferably `nullptr`), 
 *                                                   and check validity before dereferencing.
 *                  7. Const Correctness            : Enforce read-only guarantees on pointees and pointers.
 *
 * TIME COMPLEXITY  : Smart Pointer Construction / Destruction / Access: O(1) constant time.
 * SPACE COMPLEXITY : `std::unique_ptr`: 8 bytes (zero overhead over raw pointer).
 *                    `std::shared_ptr`: 16 bytes (raw pointer + control block pointer).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// HELPER CLASS FOR LIFECYCLE AND BEST PRACTICE DEMONSTRATIONS
// =====================================================================================
class NetworkSession {
private:
    int sessionId_;
    string clientAddress_;

public:
    static inline int activeSessions = 0; // C++17 inline static counter

    NetworkSession(int id, string address)
        : sessionId_(id), clientAddress_(std::move(address)) {
        ++activeSessions;
        cout << "    [SESSION OPENED] ID: " << sessionId_ << " | Client: " << clientAddress_ 
             << " | Active Sessions: " << activeSessions << "\n";
    }

    ~NetworkSession() {
        --activeSessions;
        cout << "    [SESSION CLOSED] ID: " << sessionId_ << " | Client: " << clientAddress_ 
             << " | Remaining Active: " << activeSessions << "\n";
    }

    void processData(const string& payload) const {
        cout << "    [DATA PROCESSING] Session " << sessionId_ << " processing payload: \"" 
             << payload << "\"\n";
    }

    [[nodiscard]] int getId() const { return sessionId_; }
    [[nodiscard]] const string& getAddress() const { return clientAddress_; }
};

// =====================================================================================
// RULE OF ZERO CLASS DEMONSTRATION
// By relying on smart pointers, the class needs no custom destructors or copy/move ops.
// =====================================================================================
class ServiceHandler {
private:
    std::unique_ptr<NetworkSession> primarySession_;

public:
    explicit ServiceHandler(std::unique_ptr<NetworkSession> session)
        : primarySession_(std::move(session)) {}

    // RULE OF ZERO: Default copy/move semantics handled correctly by member types.
    // std::unique_ptr automatically disables unsafe copies and enables clean moves.

    void execute() const {
        if (primarySession_) {
            primarySession_->processData("ServiceHandler Execution Event");
        }
    }
};

// =====================================================================================
// FUNCTION CONTRACT BEST PRACTICES
// =====================================================================================

// Best Practice 1: Sink Function (Explicit Ownership Transfer via std::unique_ptr)
void archiveSession(std::unique_ptr<NetworkSession> session) {
    if (session) {
        cout << "    [OWNERSHIP SINK] Archiving session ID: " << session->getId() << "\n";
        session->processData("Archival Log Entry");
    }
} // `session` goes out of scope and automatically frees heap memory HERE.

// Best Practice 2: Non-Owning Observer Function (Borrowing view via Raw Pointer / Nullable)
void inspectSessionNullable(const NetworkSession* sessionObserver) {
    if (sessionObserver != nullptr) { // Defensive check
        cout << "    [NON-OWNING OBSERVER] Inspecting Session ID: " << sessionObserver->getId() 
             << " at address: " << static_cast<const void*>(sessionObserver) << "\n";
    } else {
        cout << "    [NON-OWNING OBSERVER] Received nullptr observer.\n";
    }
}

// Best Practice 3: Non-Owning Guarantee (Borrowing view via Reference / Non-Nullable)
void inspectSessionRequired(const NetworkSession& sessionRef) {
    cout << "    [NON-NULL OBSERVER] Reading Session ID: " << sessionRef.getId() 
         << " (Guaranteed non-null reference access)\n";
}

// Best Practice 4: Factory Function (Returns Ownership via std::unique_ptr)
std::unique_ptr<NetworkSession> createSessionFactory(int id, const string& address) {
    // Prefer std::make_unique for exception safety and concise allocation
    return std::make_unique<NetworkSession>(id, address);
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base session ID for memory best practice demonstrations (e.g., 500): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 500." << endl;
        userInputValue = 500;
    }

    // =====================================================================================
    // 1. BEST PRACTICE 1: FAVOR SMART POINTERS OVER RAW NEW / DELETE
    // =====================================================================================
    cout << "\n================ 1. FAVOR SMART POINTERS (`std::make_unique`) ================\n";

    // BAD PRACTICE:  NetworkSession* raw = new NetworkSession(...); delete raw;
    // GOOD PRACTICE: Use std::make_unique to encapsulate allocation and lifetime.

    cout << "  - Creating session using `std::make_unique<NetworkSession>`...\n";
    auto smartSession1 = std::make_unique<NetworkSession>(userInputValue, "192.168.1.10");

    smartSession1->processData("Initial Ping");
    cout << "  - Active sessions count: " << NetworkSession::activeSessions << "\n";
    cout << "  - [BEST PRACTICE ADVANTAGE]: Zero risk of memory leaks; cleanup is automatic on scope exit.\n";

    // =====================================================================================
    // 2. BEST PRACTICE 2: EXPRESS CLEAR OWNERSHIP IN FUNCTION CONTRACTS
    // =====================================================================================
    cout << "\n================ 2. EXPLICIT FUNCTION OWNERSHIP CONTRACTS ================\n";

    // A. Non-Owning Borrowing (Raw pointer inspection)
    cout << "  - Borrowing non-owning view via raw pointer parameter (`const T*`):\n";
    inspectSessionNullable(smartSession1.get());

    // B. Non-Owning Borrowing (Reference parameter)
    cout << "  - Borrowing non-owning view via reference parameter (`const T&`):\n";
    inspectSessionRequired(*smartSession1);

    // C. Ownership Transfer (Sink Pattern via std::move)
    cout << "\n  - Transferring ownership into `archiveSession(std::move(smartSession1))`...\n";
    archiveSession(std::move(smartSession1));

    cout << "  - Post-Move `smartSession1` state : " 
         << (smartSession1 == nullptr ? "NULLPTR (Ownership Relinquished)" : "VALID") << "\n";
    cout << "  - Active sessions post-archival   : " << NetworkSession::activeSessions << "\n";

    // =====================================================================================
    // 3. BEST PRACTICE 3: PREFER `std::vector` OVER DYNAMIC ARRAYS (`new T[]`)
    // =====================================================================================
    cout << "\n================ 3. PREFER `std::vector` OVER RAW DYNAMIC ARRAYS ================\n";

    // BAD PRACTICE:  int* arr = new int[N]; delete[] arr;
    // GOOD PRACTICE: std::vector<T> handles allocation, sizing, capacity, bounds checks, and cleanup.

    cout << "  - Managing dynamic collection using `std::vector<std::unique_ptr<NetworkSession>>`...\n";
    vector<std::unique_ptr<NetworkSession>> sessionPool;

    sessionPool.push_back(createSessionFactory(userInputValue + 1, "10.0.0.1"));
    sessionPool.push_back(createSessionFactory(userInputValue + 2, "10.0.0.2"));

    cout << "  - Iterating through vector pool:\n";
    for (const auto& sessionPtr : sessionPool) {
        inspectSessionRequired(*sessionPtr);
    }

    cout << "  - Clearing vector pool...\n";
    sessionPool.clear(); // Destructs all contained unique_ptrs automatically
    cout << "  - Active sessions after pool clear: " << NetworkSession::activeSessions << "\n";

    // =====================================================================================
    // 4. BEST PRACTICE 4: CONST CORRECTNESS WITH POINTERS
    // =====================================================================================
    cout << "\n================ 4. CONST CORRECTNESS WITH POINTERS ================\n";

    int mutableVal = 100;
    int targetVal = 200;

    // A. Pointer to Const Data: Target cannot be mutated, address can be rebound
    const int* ptrToConst = &mutableVal;
    cout << "  - `const int*` Inspected Value : " << *ptrToConst << "\n";
    ptrToConst = &targetVal; // ALLOWED: Rebinding pointer address

    // B. Const Pointer to Mutable Data: Target can be mutated, address is frozen
    int* const constPtr = &mutableVal;
    *constPtr = 150; // ALLOWED: Mutating pointee data
    cout << "  - `int* const` Mutated Value   : " << mutableVal << "\n";

    // C. Smart Pointer Const Data: Managed object is read-only
    std::unique_ptr<const NetworkSession> constSmartSession = 
        std::make_unique<const NetworkSession>(userInputValue + 10, "172.16.0.1");

    // constSmartSession->processData("Attempt"); // Allowed if method is const
    cout << "  - Read-only smart session ID   : " << constSmartSession->getId() << "\n";

    // =====================================================================================
    // 5. BEST PRACTICE 5: ADHERE TO RULE OF ZERO
    // =====================================================================================
    cout << "\n================ 5. RULE OF ZERO / RULE OF FIVE ================\n";

    {
        cout << "  - Instantiating `ServiceHandler` managing internal `std::unique_ptr`...\n";
        ServiceHandler handler(createSessionFactory(userInputValue + 20, "127.0.0.1"));
        handler.execute();

        // Moving handler safely using default generated move constructor
        ServiceHandler movedHandler = std::move(handler);
        movedHandler.execute();

        cout << "  - Exiting scope (ServiceHandler cleanup handled automatically by Rule of Zero):\n";
    }

    cout << "  - Final Active Sessions Count = " << NetworkSession::activeSessions << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ POINTERS BEST PRACTICES SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Principle / Guideline | Recommended Modern C++ Pattern    | Avoided Anti-Pattern / Hazard     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Prefer Smart Pointers | `auto p = std::make_unique<T>();` | `T* p = new T();` (Manual delete) |\n"
         << "| Dynamic Array Buffer  | `std::vector<T>` or `std::array`  | `T* arr = new T[N];` (`delete[]`) |\n"
         << "| Sink Ownership        | `void func(std::unique_ptr<T> p)` | Unclear ownership raw pointers    |\n"
         << "| Non-Owning Observer   | `void func(const T&)` / `const T*`| Passing owning pointers needlessly|\n"
         << "| Factory Return        | `std::unique_ptr<T> create()`     | Returning raw heap pointers       |\n"
         << "| Object Lifetime       | Rule of Zero (Smart member types) | Manual destructor/copy/move code  |\n"
         << "| Pointer Disarming     | `p = nullptr;` post-move or free  | Uninitialized or dangling raw ptrs|\n"
         << "| Const Guarantees      | `unique_ptr<const T>` / `const T*`| Mutable pointers for read-only data|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}