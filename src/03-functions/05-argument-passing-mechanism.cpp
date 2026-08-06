/*
 * =====================================================================================
 * CONCEPT        : Argument Passing Mechanisms in C++
 * DESCRIPTION    : Comprehensive implementation detailing parameter passing semantics:
 *                  1. Pass-by-Value            : Creates independent copies; isolates caller data.
 *                  2. Pass-by-Lvalue-Ref (T&)  : Passes alias; enables caller object mutation.
 *                  3. Pass-by-Const-Ref (const T&): Zero-copy, read-only; accepts lvalues & rvalues.
 *                  4. Pass-by-Pointer (T*)     : Explicit memory address passing; nullable & reassigned.
 *                  5. Pass-by-Rvalue-Ref (T&&) : Move semantics; transfers resource ownership without copy.
 *
 * TIME COMPLEXITY  : Value/Copy: O(N) where N is object size. Reference/Pointer/Move: O(1).
 * SPACE COMPLEXITY : Value/Copy: O(N) stack/heap copy. Reference/Pointer/Move: O(1) stack frame.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER CLASS: Resource Tracker
// Visualizes construction, copy, move, and destruction during parameter passing.
// =====================================================================================
class ResourceTracker {
private:
    string name_;
    size_t payloadSize_;

public:
    // Standard Constructor
    explicit ResourceTracker(string name, size_t size = 100) 
        : name_(std::move(name)), payloadSize_(size) {
        cout << "    [Construct] Created '" << name_ << "' (Size: " << payloadSize_ << ")\n";
    }

    // Copy Constructor (Triggers during Pass-by-Value)
    ResourceTracker(const ResourceTracker& other) 
        : name_(other.name_ + "_copy"), payloadSize_(other.payloadSize_) {
        cout << "    [COPY CONST] Copied '" << other.name_ << "' -> '" << name_ << "' (Deep Copy)\n";
    }

    // Move Constructor (Triggers during Pass-by-Rvalue-Reference)
    ResourceTracker(ResourceTracker&& other) noexcept 
        : name_(std::move(other.name_) + "_moved"), payloadSize_(other.payloadSize_) {
        other.payloadSize_ = 0;
        cout << "    [MOVE CONST] Transferred ownership -> '" << name_ << "' (Zero-Copy)\n";
    }

    // Destructor
    ~ResourceTracker() {
        cout << "    [Destruct ] Destroyed '" << name_ << "'\n";
    }

    // Member methods
    void modifyName(const string& newName) { name_ = newName; }
    [[nodiscard]] const string& getName() const { return name_; }
    [[nodiscard]] size_t getPayloadSize() const { return payloadSize_; }
};

// =====================================================================================
// ARGUMENT PASSING DEMONSTRATION FUNCTIONS
// =====================================================================================

/**
 * 1. PASS-BY-VALUE
 * Invokes copy constructor. Modifications inside do not alter caller's object.
 */
void demonstratePassByValue(ResourceTracker resourceCopy) {
    cout << "  -> Inside Function Scope: Received object '" << resourceCopy.getName() << "'\n";
    resourceCopy.modifyName("Modified_Locally");
    cout << "  -> Inside Function Scope: Modified local name to '" << resourceCopy.getName() << "'\n";
}

/**
 * 2. PASS-BY-LVALUE-REFERENCE (T&)
 * Passes memory alias. Direct access allows caller object mutation without copying.
 */
void demonstratePassByLvalueReference(ResourceTracker& resourceRef) {
    cout << "  -> Inside Function Scope: Referencing '" << resourceRef.getName() 
         << "' at address (" << &resourceRef << ")\n";
    resourceRef.modifyName(resourceRef.getName() + "_MutatedInPlace");
}

/**
 * 3. PASS-BY-CONST-REFERENCE (const T&)
 * Read-only alias. Avoids copy overhead; can bind to both lvalues and temporary rvalues.
 */
void demonstratePassByConstReference(const ResourceTracker& constRef) {
    cout << "  -> Inside Function Scope: Read-only access to '" << constRef.getName() 
         << "' at address (" << &constRef << ")\n";
    // constRef.modifyName("Error"); // COMPILER ERROR: Cannot modify through const reference
}

/**
 * 4. PASS-BY-POINTER (T*)
 * Explicit memory address passing. Supports null pointers and address reassignment.
 */
void demonstratePassByPointer(ResourceTracker* resourcePtr) {
    if (resourcePtr != nullptr) {
        cout << "  -> Inside Function Scope: Dereferenced pointer pointing to '" 
             << resourcePtr->getName() << "' at address (" << resourcePtr << ")\n";
        resourcePtr->modifyName(resourcePtr->getName() + "_ViaPointer");
    } else {
        cout << "  -> Inside Function Scope: Received nullptr (Safe check passed)\n";
    }
}

/**
 * 5. PASS-BY-RVALUE-REFERENCE (T&&)
 * Binds to expiring temporary objects or std::move resources to steal internal buffers.
 */
void demonstratePassByRvalueReference(ResourceTracker&& rvalRef) {
    cout << "  -> Inside Function Scope: Preparing to take ownership of '" << rvalRef.getName() << "'\n";
    ResourceTracker localResource = std::move(rvalRef); // Move assignment / construction
    cout << "  -> Inside Function Scope: Ownership transferred to local variable '" 
         << localResource.getName() << "'\n";
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    size_t dynamicSize = 100;

    // 1. DYNAMIC INPUT COLLECTION WITH STREAM FLUSHING
    cout << "Enter resource payload size allocation (e.g., 250): " << flush;
    if (!(cin >> dynamicSize) || dynamicSize == 0) {
        cout << "Invalid input. Defaulting allocation size to 100." << endl;
        dynamicSize = 100;
    }

    cout << "\n================ 1. PASS-BY-VALUE ================" << endl;
    {
        ResourceTracker primaryObj("PrimaryVal", dynamicSize);
        cout << "Calling demonstratePassByValue():\n";
        demonstratePassByValue(primaryObj);
        cout << "After Call: Primary object name = '" << primaryObj.getName() << "' (Unchanged in caller)\n";
    }

    cout << "\n================ 2. PASS-BY-LVALUE-REFERENCE (T&) ================" << endl;
    {
        ResourceTracker primaryObj("PrimaryRef", dynamicSize);
        cout << "Calling demonstratePassByLvalueReference():\n";
        demonstratePassByLvalueReference(primaryObj);
        cout << "After Call: Primary object name = '" << primaryObj.getName() << "' (Mutated in caller)\n";
    }

    cout << "\n================ 3. PASS-BY-CONST-REFERENCE (const T&) ================" << endl;
    {
        ResourceTracker primaryObj("PrimaryConstRef", dynamicSize);
        cout << "Calling demonstratePassByConstReference() with lvalue:\n";
        demonstratePassByConstReference(primaryObj);

        cout << "Calling demonstratePassByConstReference() with temporary rvalue:\n";
        demonstratePassByConstReference(ResourceTracker("TempRvalue", 50));
    }

    cout << "\n================ 4. PASS-BY-POINTER (T*) ================" << endl;
    {
        ResourceTracker primaryObj("PrimaryPtr", dynamicSize);
        cout << "Calling demonstratePassByPointer() with memory address (&primaryObj):\n";
        demonstratePassByPointer(&primaryObj);
        cout << "After Call: Primary object name = '" << primaryObj.getName() << "'\n";

        cout << "Calling demonstratePassByPointer() with nullptr:\n";
        demonstratePassByPointer(nullptr);
    }

    cout << "\n================ 5. PASS-BY-RVALUE-REFERENCE (T&&) ================" << endl;
    {
        ResourceTracker primaryObj("PrimaryMove", dynamicSize);
        cout << "Calling demonstratePassByRvalueReference() with std::move():\n";
        demonstratePassByRvalueReference(std::move(primaryObj));
        cout << "After Call: Primary object state payload size = " << primaryObj.getPayloadSize() 
             << " (Ownership moved out)\n";
    }

    cout << "\n================ ARGUMENT PASSING MECHANISMS SUMMARY ================" << endl;
    cout << "1. Pass-by-Value      : Copy overhead O(N); safe for primitives & small structs." << endl;
    cout << "2. Pass-by-Lvalue Ref : Zero-copy O(1); allows output parameter modification." << endl;
    cout << "3. Pass-by-Const Ref  : Zero-copy O(1); DEFAULT choice for objects & strings." << endl;
    cout << "4. Pass-by-Pointer     : Zero-copy O(1); use when nullability or address syntax is required." << endl;
    cout << "5. Pass-by-Rvalue Ref : Zero-copy resource transfer; optimal for move-only types (std::unique_ptr)." << endl;

    return 0;
}