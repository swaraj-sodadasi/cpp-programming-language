/*
 * =====================================================================================
 * CONCEPT        : The `this` Pointer in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the mechanics, applications, and memory semantics of the `this` pointer:
 *
 *                  1. Implicit Pointer Parameter : An implicit hidden parameter passed to all 
 *                                                  non-static member functions pointing to the 
 *                                                  calling object (`Type* const this`).
 *                  2. Parameter Disambiguation    : Resolving name collisions between member variables 
 *                                                  and constructor/method parameters (`this->var = var`).
 *                  3. Method Chaining (Fluent)   : Returning `*this` by reference (`Type&`) to allow 
 *                                                  cascading method calls (`obj.setA().setB().run()`).
 *                  4. Self-Assignment Guard      : Checking `if (this == &other)` inside copy and 
 *                                                  move assignment operators (`operator=`).
 *                  5. Const-Correctness & Type   : `Type* const this` in mutable methods vs. 
 *                                                  `const Type* const this` in `const` methods.
 *                  6. Passing `this` to Externals : Passing the current instance pointer to external 
 *                                                  callbacks, helper classes, or event listeners.
 *                  7. Safe Self-Destruction      : The `delete this` pattern for heap-allocated 
 *                                                  self-managed reference-counted handles.
 *
 * TIME COMPLEXITY  : Member Access / Method Chaining / Inspection: O(1) constant time.
 * SPACE COMPLEXITY : Overhead per Object Instance: 0 bytes (passed implicitly via CPU register).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// Forward declaration for passing `this` to external inspectors
class TaskNode;

// =====================================================================================
// HELPER CLASS: EXTERNAL INSPECTOR
// Demonstrates passing the invoking object's `this` pointer to external functions.
// =====================================================================================
class ExternalInspector {
public:
    static void logTaskDetails(const TaskNode* taskPtr);
};

// =====================================================================================
// 1. FLUENT BUILDER CLASS DEMONSTRATING PARAMETER DISAMBIGUATION & METHOD CHAINING
// =====================================================================================
class TaskNode {
private:
    int taskId_;
    string taskTitle_;
    int priorityLevel_;
    bool isCompleted_;

public:
    // A. Disambiguating Shadowed Member Variables (Parameter name == Member name)
    TaskNode(int taskId, string taskTitle, int priorityLevel) {
        // Without 'this->', parameter 'taskId' shadows member 'taskId_'
        this->taskId_ = taskId;
        this->taskTitle_ = std::move(taskTitle);
        this->priorityLevel_ = priorityLevel;
        this->isCompleted_ = false;

        cout << "    [TaskNode Ctor] Initialized object at address (this): " 
             << static_cast<const void*>(this) << "\n";
    }

    ~TaskNode() noexcept {
        cout << "    [TaskNode Dtor] Destroying object at address (this): " 
             << static_cast<const void*>(this) << "\n";
    }

    // B. Method Chaining (Fluent Interface): Return *this by reference
    TaskNode& setTaskTitle(string taskTitle) {
        this->taskTitle_ = std::move(taskTitle); // Disambiguation + Mutation
        return *this; // Return reference to current object
    }

    TaskNode& setPriorityLevel(int priorityLevel) {
        if (priorityLevel < 1 || priorityLevel > 10) {
            throw invalid_argument("Priority level must be between 1 and 10!");
        }
        this->priorityLevel_ = priorityLevel;
        return *this;
    }

    TaskNode& setCompleted(bool isCompleted) {
        this->isCompleted_ = isCompleted;
        return *this;
    }

    // C. Demonstrating `this` Pointer Const-Correctness Types
    void inspectThisPointerType() {
        // Inside non-const method, type of `this` is `TaskNode* const`
        cout << "    [NON-CONST METHOD] `this` pointer address: " << static_cast<void*>(this) 
             << " | Type: TaskNode* const\n";
    }

    void inspectThisPointerTypeConst() const {
        // Inside const method, type of `this` is `const TaskNode* const`
        cout << "    [CONST METHOD]     `this` pointer address: " << static_cast<const void*>(this) 
             << " | Type: const TaskNode* const\n";
        // this->priorityLevel_ = 5; // COMPILE ERROR: Cannot mutate through const `this` pointer!
    }

    // D. Passing `this` to External Functions
    void registerWithInspector() const {
        cout << "    [PASSING THIS] Delegating current instance pointer (`this`) to ExternalInspector...\n";
        ExternalInspector::logTaskDetails(this); // Passes `const TaskNode*`
    }

    void displayStatus() const {
        cout << "    [TASK STATUS] Address: " << static_cast<const void*>(this)
             << " | ID: " << setw(4) << taskId_ 
             << " | Title: " << setw(20) << left << taskTitle_ 
             << " | Priority: " << priorityLevel_ 
             << " | Completed: " << (isCompleted_ ? "YES" : "NO") << "\n";
    }

    [[nodiscard]] int getTaskId() const noexcept { return this->taskId_; }
    [[nodiscard]] const string& getTaskTitle() const noexcept { return this->taskTitle_; }
    [[nodiscard]] int getPriorityLevel() const noexcept { return this->priorityLevel_; }
    [[nodiscard]] bool isCompleted() const noexcept { return this->isCompleted_; }
};

// Definition of External Inspector member function
void ExternalInspector::logTaskDetails(const TaskNode* taskPtr) {
    if (taskPtr != nullptr) {
        cout << "    [EXTERNAL INSPECTOR] Received object pointer: " << static_cast<const void*>(taskPtr) 
             << " -> Task ID: " << taskPtr->getTaskId() 
             << " ('" << taskPtr->getTaskTitle() << "')\n";
    }
}

// =====================================================================================
// 2. CLASS DEMONSTRATING SELF-ASSIGNMENT GUARD & SAFE SELF-DESTRUCTION (`delete this`)
// =====================================================================================
class SelfManagedResource {
private:
    int resourceId_;
    int* dataBuffer_;

    // Private constructor enforces heap allocation via factory method for `delete this` safety
    SelfManagedResource(int id, int initialValue) 
        : resourceId_(id), dataBuffer_(new int(initialValue)) {
        cout << "    [SelfManaged Ctor] Created heap instance ID " << resourceId_ 
             << " at 'this': " << static_cast<const void*>(this) << "\n";
    }

    // Private destructor prevents stack allocation (ensures object is only destroyed via `destroy()`)
    ~SelfManagedResource() noexcept {
        cout << "    [SelfManaged Dtor] Releasing memory at 'this': " 
             << static_cast<const void*>(this) << "\n";
        delete dataBuffer_;
    }

public:
    // Factory function for dynamic allocation
    [[nodiscard]] static SelfManagedResource* create(int id, int initialValue) {
        return new SelfManagedResource(id, initialValue);
    }

    // Copy Assignment Operator demonstrating Self-Assignment Check (`if (this == &other)`)
    SelfManagedResource& operator=(const SelfManagedResource& other) {
        cout << "    [COPY ASSIGN] Checking self-assignment guard (this == &other)...\n";
        
        // SELF-ASSIGNMENT GUARD: Prevents self-destruction of resources during `a = a;`
        if (this == &other) {
            cout << "    [SELF-ASSIGNMENT DETECTED] `this` (" << static_cast<void*>(this) 
                 << ") matches `&other` (" << static_cast<const void*>(&other) 
                 << "). Skipping operation safely!\n";
            return *this;
        }

        // Allocate new resource first (Exception Safety)
        int* newBuffer = new int(*other.dataBuffer_);
        delete dataBuffer_;
        dataBuffer_ = newBuffer;
        resourceId_ = other.resourceId_ + 1000;

        cout << "    [COPY ASSIGN COMPLETED] Successfully copied state from source.\n";
        return *this; // Return *this reference for method chaining
    }

    // E. Safe Self-Destruction Pattern (`delete this`)
    void destroySelf() noexcept {
        cout << "    [SELF-DESTRUCTION] Invoking `delete this` on object address: " 
             << static_cast<const void*>(this) << "...\n";
        delete this; // Safely calls destructor and deallocates heap memory
        // CRITICAL NOTE: Never access member variables or `this` after executing `delete this`!
    }

    void process() const {
        if (dataBuffer_) {
            cout << "    [PROCESSING] Resource ID: " << resourceId_ 
                 << " | Buffer Value: " << *dataBuffer_ << "\n";
        }
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for `this` pointer analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. PARAMETER DISAMBIGUATION & METHOD CHAINING (FLUENT INTERFACE)
    // =====================================================================================
    cout << "\n================ 1. PARAMETER DISAMBIGUATION & METHOD CHAINING ================\n";

    {
        cout << "  - Creating TaskNode instance with shadowed parameters...\n";
        TaskNode task(userInputValue, "Initial_Title", 3);
        task.displayStatus();

        cout << "\n  - Executing Method Chaining via `return *this;`...\n";
        // Method chaining: Each setter mutates the object and returns *this reference
        task.setTaskTitle("Refactored_Title")
            .setPriorityLevel(9)
            .setCompleted(true);

        cout << "  - Post-Chaining Object Status:\n";
        task.displayStatus();

        cout << "\n  - Inspecting `this` Pointer Types (Const vs Non-Const Context):\n";
        task.inspectThisPointerType();
        task.inspectThisPointerTypeConst();

        cout << "\n  - Passing `this` pointer to External Function:\n";
        task.registerWithInspector();

        cout << "\n  - Exiting local scope...\n";
    } // `task` destroyed automatically HERE

    // =====================================================================================
    // 2. SELF-ASSIGNMENT GUARD (`if (this == &other)`)
    // =====================================================================================
    cout << "\n================ 2. SELF-ASSIGNMENT GUARD (`if (this == &other)`) ================\n";

    {
        cout << "  - Creating heap-allocated SelfManagedResource instances...\n";
        SelfManagedResource* resA = SelfManagedResource::create(userInputValue + 10, 42);
        SelfManagedResource* resB = SelfManagedResource::create(userInputValue + 20, 99);

        resA->process();
        resB->process();

        cout << "\n  - Case A: Normal Assignment (`*resA = *resB`)...\n";
        *resA = *resB;
        resA->process();

        cout << "\n  - Case B: Triggering Self-Assignment (`*resA = *resA`)...\n";
        *resA = *resA; // Triggers `if (this == &other)` check

        // Clean up heap objects using `delete this` pattern
        cout << "\n  - Cleaning up dynamic instances via `destroySelf()` (`delete this`)...\n";
        resA->destroySelf();
        resB->destroySelf();
    }

    // =====================================================================================
    // 3. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 3. INVARIANT ENFORCEMENT ================\n";

    try {
        cout << "  - Attempting invalid priority assignment during method chaining...\n";
        TaskNode invalidTask(userInputValue + 30, "FaultyTask", 5);
        invalidTask.setPriorityLevel(99); // Triggers exception
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ THE `this` POINTER SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Application / Use Case| Technical Implementation Syntax   | Primary Purpose & Benefit         |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Parameter Disambig.   | `this->var = var;`                | Resolves member/parameter shadow  |\n"
         << "| Method Chaining       | `Type& setX() { ... return *this;}`| Enables fluent API syntax         |\n"
         << "| Self-Assignment Guard | `if (this == &other) return *this;`| Prevents resource corruption      |\n"
         << "| External Delegation   | `ExternalFunc(this);`             | Passes current object reference   |\n"
         << "| Const Member Function | `const Type* const this;`         | Guarantees read-only state safety |\n"
         << "| Non-Const Method      | `Type* const this;`               | Allows object state mutation      |\n"
         << "| Self-Destruction      | `delete this;`                    | Heap reference-counted teardown   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}