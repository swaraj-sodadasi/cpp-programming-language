/*
 * =====================================================================================
 * CONCEPT        : Rule of Zero in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the C++ "Rule of Zero" architectural principle.
 *
 *                  THE RULE OF ZERO:
 *                  Classes that do not directly manage raw resources (like raw pointers,
 *                  file descriptors, or OS handles) SHOULD NOT DECLARE ANY of the 5 special
 *                  member functions (Destructor, Copy Ctor, Copy Assign, Move Ctor, Move Assign).
 *
 *                  Instead, delegate resource ownership entirely to standard RAII types 
 *                  (e.g., `std::string`, `std::vector`, `std::unique_ptr`, `std::shared_ptr`).
 *                  The C++ compiler automatically synthesizes optimal, exception-safe special 
 *                  member functions for copy, move, and cleanup operations!
 *
 *                  BENEFITS OF THE RULE OF ZERO:
 *                  1. Zero Boilerplate     : Eliminates repetitive, error-prone manual memory code.
 *                  2. Complete Safety      : Leverages tested RAII wrappers (prevents memory leaks, 
 *                                            dangling pointers, and double-free bugs).
 *                  3. Automatic Semantics  : - If all members are copyable/movable -> Class is copyable & movable.
 *                                            - If a member is move-only (`std::unique_ptr`) -> Class automatically
 *                                              becomes move-only with zero code required!
 *                  4. Strong Exception Safe: RAII guarantees full stack-unwinding resource cleanup if 
 *                                            an exception is thrown during construction.
 *
 * TIME COMPLEXITY  : Copy Operations (Vector/String): O(N) element copies.
 *                    Move Operations (Smart Ptr/Vec): O(1) constant time pointer transfers.
 *                    Destructor / Getters           : O(1) constant time.
 * SPACE COMPLEXITY : Automatic Footprint            : Exact sum of member RAII wrapper sizes.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <iomanip>
#include <limits>
#include <type_traits>

using namespace std;

// =====================================================================================
// 1. COPYABLE & MOVABLE RULE OF ZERO CLASS
// Delegates member memory management to std::string, std::vector, and std::shared_ptr.
// DECLARES EXACTLY ZERO SPECIAL MEMBER FUNCTIONS!
// =====================================================================================
class AutomaticCopyableNode {
private:
    int nodeId_{0};
    string nodeName_{"DefaultNode"};
    vector<int> payloadBuffer_;
    shared_ptr<double> sharedWeight_; // Shared ownership RAII wrapper

public:
    // Regular Parameterized Constructor (NOT a special member function)
    AutomaticCopyableNode(int id, string name, vector<int> buffer, double weight)
        : nodeId_(id), 
          nodeName_(std::move(name)), 
          payloadBuffer_(std::move(buffer)), 
          sharedWeight_(make_shared<double>(weight)) {
        cout << "    [CopyableNode Ctor] Created Node ID: " << nodeId_ 
             << " ('" << nodeName_ << "') | Shared Ref Count: " << sharedWeight_.use_count() << "\n";
    }

    // --- ZERO SPECIAL MEMBER FUNCTIONS DECLARED HERE ---
    // Destructor                 -> Compiler synthesized automatically
    // Copy Constructor          -> Compiler synthesized automatically (Deep copy vector/string, share shared_ptr)
    // Copy Assignment Operator  -> Compiler synthesized automatically
    // Move Constructor          -> Compiler synthesized automatically (O(1) pointer steals)
    // Move Assignment Operator  -> Compiler synthesized automatically

    void displayState() const {
        cout << "    [COPYABLE STATE] ID: " << setw(4) << nodeId_ 
             << " | Name: " << setw(18) << left << nodeName_ 
             << " | Buffer Size: " << setw(4) << payloadBuffer_.size() 
             << " | Shared Weight: " << (sharedWeight_ ? to_string(*sharedWeight_) : "NULL")
             << " (Ref Count: " << (sharedWeight_ ? sharedWeight_.use_count() : 0) << ")\n";
    }

    void modifyFirstBufferElement(int val) {
        if (!payloadBuffer_.empty()) {
            payloadBuffer_[0] = val;
        }
    }

    [[nodiscard]] int getNodeId() const noexcept { return nodeId_; }
    [[nodiscard]] const string& getNodeName() const noexcept { return nodeName_; }
};

// =====================================================================================
// 2. MOVE-ONLY RULE OF ZERO CLASS
// Containing a `std::unique_ptr` automatically makes this class MOVE-ONLY.
// Compiler implicitly disables copy operations and synthesizes move operations!
// DECLARES EXACTLY ZERO SPECIAL MEMBER FUNCTIONS!
// =====================================================================================
class AutomaticMoveOnlyTask {
private:
    int taskId_{0};
    string taskTitle_{"UntitledTask"};
    unique_ptr<string> exclusiveResource_; // Move-only member forces class to be Move-Only!

public:
    AutomaticMoveOnlyTask(int id, string title, string exclusivePayload)
        : taskId_(id), 
          taskTitle_(std::move(title)), 
          exclusiveResource_(make_unique<string>(std::move(exclusivePayload))) {
        cout << "    [MoveOnlyTask Ctor] Created Task ID: " << taskId_ 
             << " ('" << taskTitle_ << "')\n";
    }

    // --- ZERO SPECIAL MEMBER FUNCTIONS DECLARED HERE ---
    // The presence of std::unique_ptr automatically:
    // 1. Deletes Copy Constructor & Copy Assignment Operator at compile-time!
    // 2. Synthesizes Move Constructor & Move Assignment Operator automatically!
    // 3. Synthesizes Destructor automatically to clean up exclusiveResource_!

    void displayState() const {
        if (exclusiveResource_) {
            cout << "    [MOVE-ONLY STATE] ID: " << setw(4) << taskId_ 
                 << " | Title: " << setw(18) << left << taskTitle_ 
                 << " | Exclusive Resource Payload: '" << *exclusiveResource_ << "'\n";
        } else {
            cout << "    [MOVE-ONLY STATE] ID: " << setw(4) << taskId_ 
                 << " | Title: " << setw(18) << left << taskTitle_ 
                 << " | DISARMED / MOVED-FROM (unique_ptr is nullptr)\n";
        }
    }

    [[nodiscard]] int getTaskId() const noexcept { return taskId_; }
};

// Factory helper returning temporary move-only instance
AutomaticMoveOnlyTask createTemporaryTask(int id, string title) {
    return AutomaticMoveOnlyTask(id, std::move(title), "Factory_Generated_Exclusive_Payload");
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Rule of Zero analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // Verify Type Traits at compile time
    static_assert(std::is_copy_constructible_v<AutomaticCopyableNode>, 
                  "AutomaticCopyableNode must be copy constructible under Rule of Zero!");
    static_assert(!std::is_copy_constructible_v<AutomaticMoveOnlyTask>, 
                  "AutomaticMoveOnlyTask must NOT be copy constructible (contains unique_ptr)!");
    static_assert(std::is_move_constructible_v<AutomaticMoveOnlyTask>, 
                  "AutomaticMoveOnlyTask must be move constructible under Rule of Zero!");

    // =====================================================================================
    // 1. AUTOMATIC COPY SEMANTICS (RULE OF ZERO)
    // =====================================================================================
    cout << "\n================ 1. AUTOMATIC COPY SEMANTICS (RULE OF ZERO) ================\n";

    {
        cout << "  - Creating primary `AutomaticCopyableNode` (nodeA)...\n";
        AutomaticCopyableNode nodeA(userInputValue, "PrimaryNode_A", {10, 20, 30, 40}, 99.5);
        nodeA.displayState();

        cout << "\n  - Copying nodeA into nodeB (`AutomaticCopyableNode nodeB = nodeA;`):\n";
        cout << "  - [COMPILER MAGIC]: Copy Ctor synthesized automatically without custom code!\n";
        AutomaticCopyableNode nodeB = nodeA; // Implicit Copy Constructor
        
        nodeB.displayState();

        cout << "\n  - Modifying nodeB buffer to prove independent value semantics for vector:\n";
        nodeB.modifyFirstBufferElement(999);
        
        cout << "    * nodeA (Original) state:\n";
        nodeA.displayState();
        cout << "    * nodeB (Cloned)   state:\n";
        nodeB.displayState();

        cout << "\n  - Copy assigning nodeA into nodeC...\n";
        AutomaticCopyableNode nodeC(userInputValue + 1, "TempNode", {1}, 1.0);
        nodeC = nodeA; // Implicit Copy Assignment Operator
        nodeC.displayState();

        cout << "  - Leaving copy scope (Destructors synthesized automatically for all nodes)...\n";
    }

    // =====================================================================================
    // 2. AUTOMATIC MOVE SEMANTICS (RULE OF ZERO)
    // =====================================================================================
    cout << "\n================ 2. AUTOMATIC MOVE SEMANTICS (RULE OF ZERO) ================\n";

    {
        cout << "  - Creating source node for move operation...\n";
        AutomaticCopyableNode sourceNode(userInputValue + 10, "MovableNode", {100, 200, 300}, 42.0);
        sourceNode.displayState();

        cout << "\n  - Moving sourceNode into targetNode via `std::move()`:\n";
        cout << "  - [COMPILER MAGIC]: Move Ctor synthesized automatically (O(1) vector/string pointer swap)!\n";
        AutomaticCopyableNode targetNode = std::move(sourceNode); // Implicit Move Constructor

        cout << "  - States post-move:\n";
        targetNode.displayState();
        sourceNode.displayState(); // Disarmed source
    }

    // =====================================================================================
    // 3. AUTOMATIC MOVE-ONLY TYPES (`std::unique_ptr` INCLUSION)
    // =====================================================================================
    cout << "\n================ 3. AUTOMATIC MOVE-ONLY CLASS (RULE OF ZERO) ================\n";

    {
        cout << "  - Creating `AutomaticMoveOnlyTask` (contains `std::unique_ptr`)...\n";
        AutomaticMoveOnlyTask task1(userInputValue + 20, "CriticalJob_1", "Secret_API_Key_0x8F3");
        task1.displayState();

        // AutomaticMoveOnlyTask taskCopy = task1;
        // COMPILE ERROR: Copy constructor is automatically DELETED because of std::unique_ptr!

        cout << "\n  - Moving task1 into task2 (`AutomaticMoveOnlyTask task2 = std::move(task1);`):\n";
        AutomaticMoveOnlyTask task2 = std::move(task1); // Move Constructor
        task2.displayState();
        task1.displayState(); // Disarmed task1

        cout << "\n  - Acquiring temporary task from factory function:\n";
        AutomaticMoveOnlyTask task3 = createTemporaryTask(userInputValue + 30, "FactoryTask");
        task3.displayState();

        cout << "  - Leaving move-only scope...\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RULE OF ZERO SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Principle / Concept   | Rule of Zero Approach             | Architectural Benefit             |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Special Member Funcs  | Write ZERO custom ctors/dtors     | Eliminates manual memory bugs     |\n"
         << "| Resource Delegation   | Use `string`, `vector`, `smart_ptr`| Automatic RAII ownership lifecycle|\n"
         << "| Copy Semantics        | Synthesized by compiler if safe   | Deep copies value types           |\n"
         << "| Move Semantics        | Synthesized by compiler if safe   | Zero-cost pointer transfers       |\n"
         << "| Move-Only Behavior    | Add `unique_ptr` field to class   | Automatically deletes copy ops    |\n"
         << "| Exception Safety      | Guaranteed stack unwinding        | No resource leaks on throw        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}