/*
 * =====================================================================================
 * CONCEPT        : Advanced OOP Features in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  advanced object-oriented language features in Modern C++:
 *
 *                  1. Nested Classes           : Classes declared within the scope of another 
 *                                                enclosing class for modular encapsulation.
 *                  2. Local Classes            : Classes defined directly inside a function body, 
 *                                                scoped strictly to that function's stack lifetime.
 *                  3. Anonymous Objects        : Unnamed temporary instances (`Type(args)`) created 
 *                                                inline for single-use operations or method chaining.
 *                  4. Object Slicing           : The phenomenon and bug where assigning a derived instance 
 *                                                to a base value object strips derived attributes and 
 *                                                vtable dynamic dispatch.
 *                  5. Dynamic Object Creation  : Heap object allocation using legacy `new`/`delete` 
 *                                                vs. Modern RAII smart pointers (`std::make_unique` 
 *                                                and `std::make_shared`).
 *
 * TIME COMPLEXITY  : Static / Stack Operations  : O(1) constant time.
 *                    Heap Allocation / Dispatch : O(1) constant time (+ allocator overhead).
 * SPACE COMPLEXITY : Instance Footprint        : Base/Derived structural sizes + dynamic heap buffers.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <limits>

using namespace std;

// =====================================================================================
// 1. NESTED CLASSES (INNER / OUTER CLASS ENCAPSULATION)
// Inner class declared inside outer class to represent modular sub-components.
// =====================================================================================
class OuterNetworkNode {
private:
    int nodeId_;
    string nodeLocation_;

public:
    // Public Nested Class: Scope is scoped within OuterNetworkNode::
    class NestedIPAddress {
    private:
        string ipAddress_;
        int port_;

    public:
        NestedIPAddress(string ip, int port)
            : ipAddress_(std::move(ip)), port_(port) {}

        void displayAddress(const OuterNetworkNode& parentNode) const {
            // Nested class methods can access parent class private members if passed a reference
            cout << "    [NESTED CLASS] Address: " << ipAddress_ << ":" << port_ 
                 << " | Belongs to Parent Node ID: " << parentNode.nodeId_ 
                 << " ('" << parentNode.nodeLocation_ << "')\n";
        }

        [[nodiscard]] const string& getIpAddress() const noexcept { return ipAddress_; }
        [[nodiscard]] int getPort() const noexcept { return port_; }
    };

    OuterNetworkNode(int id, string location, string ip, int port)
        : nodeId_(id), nodeLocation_(std::move(location)), address_(std::move(ip), port) {}

    void process() const {
        address_.displayAddress(*this);
    }

private:
    NestedIPAddress address_; // Nested class used as a private member variable
};

// =====================================================================================
// 2. LOCAL CLASSES (CLASSES DEFINED INSIDE FUNCTION BODY)
// =====================================================================================
void demonstrateLocalClass(int baseId) {
    // Local Class defined entirely inside function scope
    class LocalAuditFormatter {
    private:
        int auditId_;

    public:
        explicit LocalAuditFormatter(int id) : auditId_(id) {}

        void printFormattedAudit(const string& logMessage) const {
            cout << "    [LOCAL CLASS] Audit #" << auditId_ 
                 << " Formatted Output -> [ " << logMessage << " ]\n";
        }
    };

    // Instantiate and use local class inside function scope
    LocalAuditFormatter formatter(baseId + 500);
    formatter.printFormattedAudit("Stack frame execution active");
} // LocalAuditFormatter class type scope ends HERE

// =====================================================================================
// 3. ANONYMOUS OBJECTS (UNNAMED TEMPORARIES)
// =====================================================================================
class TemporaryLogger {
private:
    string sessionTag_;

public:
    explicit TemporaryLogger(string tag) : sessionTag_(std::move(tag)) {
        cout << "    [ANONYMOUS CTOR] TemporaryLogger instantiated with tag: '" << sessionTag_ << "'\n";
    }

    ~TemporaryLogger() noexcept {
        cout << "    [ANONYMOUS DTOR] TemporaryLogger instance destroyed at end of statement.\n";
    }

    void logMessage(const string& msg) const {
        cout << "    [ANONYMOUS EXECUTION] [" << sessionTag_ << "] Message: " << msg << "\n";
    }
};

// Function accepting an anonymous object by value or rvalue reference
void processAnonymousLog(const TemporaryLogger& logger) {
    logger.logMessage("Processed inside function parameter scope.");
}

// =====================================================================================
// 4. OBJECT SLICING DEMONSTRATION & PREVENTION
// =====================================================================================
class BaseEntity {
protected:
    int entityId_;

public:
    explicit BaseEntity(int id) : entityId_(id) {}
    virtual ~BaseEntity() noexcept = default;

    virtual void displayDetails() const {
        cout << "    [BASE ENTITY] ID: " << entityId_ << "\n";
    }

    [[nodiscard]] int getEntityId() const noexcept { return entityId_; }
};

class DerivedTelemetryEntity : public BaseEntity {
private:
    string telemetryPayload_;

public:
    DerivedTelemetryEntity(int id, string payload)
        : BaseEntity(id), telemetryPayload_(std::move(payload)) {}

    void displayDetails() const override {
        cout << "    [DERIVED TELEMETRY] ID: " << entityId_ 
             << " | Payload: '" << telemetryPayload_ << "'\n";
    }

    [[nodiscard]] const string& getPayload() const noexcept { return telemetryPayload_; }
};

// Function causing Object Slicing (Pass-by-Value)
void causeObjectSlicing(BaseEntity slicedValue) {
    cout << "      -> Inside Function (Receiving BaseEntity BY VALUE):\n";
    // Derived part (telemetryPayload_) is SLICED OFF!
    // Dynamic dispatch IS LOST (BaseEntity::displayDetails is called)!
    slicedValue.displayDetails();
}

// Function preventing Object Slicing (Pass-by-Reference)
void preventObjectSlicing(const BaseEntity& refEntity) {
    cout << "      -> Inside Function (Receiving BaseEntity BY REFERENCE):\n";
    // Derived part preserved! Dynamic dispatch executes DerivedTelemetryEntity::displayDetails!
    refEntity.displayDetails();
}

// =====================================================================================
// 5. DYNAMIC OBJECT CREATION (RAW POINTERS VS SMART POINTERS)
// =====================================================================================
void demonstrateDynamicObjectCreation(int baseId) {
    cout << "  - 1. Legacy Dynamic Allocation (`new` / `delete` raw pointers):\n";
    BaseEntity* rawPtr = new DerivedTelemetryEntity(baseId + 10, "RawHeapPayload_0x1");
    rawPtr->displayDetails();
    delete rawPtr; // Manual delete required!

    cout << "\n  - 2. Modern Dynamic Allocation (`std::make_unique` RAII ownership):\n";
    auto uniquePtr = std::make_unique<DerivedTelemetryEntity>(baseId + 20, "UniqueHeapPayload_0x2");
    uniquePtr->displayDetails(); // Automatic heap cleanup when uniquePtr leaves scope

    cout << "\n  - 3. Modern Dynamic Allocation (`std::make_shared` reference counting):\n";
    auto sharedPtr = std::make_shared<DerivedTelemetryEntity>(baseId + 30, "SharedHeapPayload_0x3");
    sharedPtr->displayDetails();
    cout << "    Shared Owner Use Count: " << sharedPtr.use_count() << "\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Advanced OOP analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. NESTED CLASSES DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. NESTED CLASSES ================\n";

    cout << "  - Creating OuterNetworkNode with NestedIPAddress...\n";
    OuterNetworkNode outerNode(userInputValue, "US-EAST-DC1", "192.168.1.50", 8080);
    outerNode.process();

    cout << "  - Direct instantiation of public Nested Class (`OuterNetworkNode::NestedIPAddress`):\n";
    OuterNetworkNode::NestedIPAddress externalAddress("10.0.0.1", 443);
    cout << "    Direct Address: " << externalAddress.getIpAddress() << ":" << externalAddress.getPort() << "\n";

    // =====================================================================================
    // 2. LOCAL CLASSES DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 2. LOCAL CLASSES ================\n";

    cout << "  - Invoking `demonstrateLocalClass()`...\n";
    demonstrateLocalClass(userInputValue);

    // =====================================================================================
    // 3. ANONYMOUS OBJECTS DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 3. ANONYMOUS OBJECTS (TEMPORARIES) ================\n";

    cout << "  - Case A: Executing inline anonymous object method call:\n";
    // Anonymous object created, method called, and object destroyed at end of expression ';'
    TemporaryLogger("INLINE_TEMP_SESSION").logMessage("One-off diagnostic logging call");

    cout << "\n  - Case B: Passing anonymous object as function argument:\n";
    processAnonymousLog(TemporaryLogger("ARGUMENT_TEMP_SESSION"));

    // =====================================================================================
    // 4. OBJECT SLICING DEMONSTRATION & PREVENTION
    // =====================================================================================
    cout << "\n================ 4. OBJECT SLICING ================\n";

    DerivedTelemetryEntity derivedObj(userInputValue + 200, "CriticalTelemetryData");

    cout << "  - Original Derived Object State:\n";
    derivedObj.displayDetails();

    cout << "\n  - Scenario A: Passing BY VALUE (Triggers Object Slicing!):\n";
    causeObjectSlicing(derivedObj);

    cout << "\n  - Scenario B: Passing BY REFERENCE (Prevents Object Slicing!):\n";
    preventObjectSlicing(derivedObj);

    // =====================================================================================
    // 5. DYNAMIC OBJECT CREATION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 5. DYNAMIC OBJECT CREATION ================\n";

    demonstrateDynamicObjectCreation(userInputValue);

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ ADVANCED OOP FEATURES SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Concept     | Implementation Syntax             | Primary Behavioral Characteristic |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Nested Class          | `class Outer { class Inner {}; };`| Scoped encapsulation inside Outer |\n"
         << "| Local Class           | Class defined inside function body| Scoped strictly to function scope |\n"
         << "| Anonymous Object      | `TemporaryLogger(\"TAG\").log();`   | Unnamed temporary destroyed at `;`|\n"
         << "| Object Slicing        | `Base obj = derivedInstance;`     | Strips derived state & dynamic vtbl|\n"
         << "| Slicing Prevention    | `const Base& ref = derived;`      | Preserves polymorphic dispatch    |\n"
         << "| Dynamic Creation (Raw)| `Base* p = new Derived();`        | Manual memory management required |\n"
         << "| Dynamic Creation RAII | `auto p = std::make_unique<D>();` | Safe automatic RAII heap teardown |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}