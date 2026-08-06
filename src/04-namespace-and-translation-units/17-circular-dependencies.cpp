/*
 * =====================================================================================
 * CONCEPT        : Circular Dependencies in C++ (Header Loops & Reference Cycles)
 * DESCRIPTION    : Comprehensive implementation detailing Circular Dependencies:
 *                  1. What is a Circular Dependency? : Class A depends on Class B, and 
 *                     Class B depends on Class A, creating a structural or inclusion loop.
 *                  2. Compiler Failure Mechanism     : Header loop (#include "B.h" in A.h and 
 *                     #include "A.h" in B.h) leads to incomplete type errors or macro guard skips.
 *                  3. Compilation Resolution        :
 *                     - Forward Declare the dependent class (`class B;`).
 *                     - Store pointers (`B*`, `std::unique_ptr<B>`, `std::weak_ptr<B>`) or references (`B&`).
 *                     - Separate declarations (class body) from definitions (member function bodies).
 *                  4. Memory Leak Resolution (RAII)  : Circular `std::shared_ptr` references 
 *                     prevent destruction (ref count never hits 0). Fix: Use `std::weak_ptr`.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution; eliminates infinite compilation loop.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame footprint with weak/shared pointer ownership.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <utility>

using namespace std;

// =====================================================================================
// 1. FORWARD DECLARATION TO BREAK COMPILATION CIRCULAR DEPENDENCY
// Class Window needs WindowManager, and WindowManager needs Window.
// =====================================================================================

// Step 1: Forward declare Window so WindowManager knows it exists
class Window;

// Step 2: Define WindowManager class using pointer to incomplete type 'Window*'
class WindowManager {
private:
    string managerTitle_;
    Window* managedWindow_; // Pointer allows incomplete type (size of pointer is fixed!)

public:
    explicit WindowManager(string title) 
        : managerTitle_(std::move(title)), managedWindow_(nullptr) {}

    void registerWindow(Window* win);
    void triggerWindowRefresh();
    [[nodiscard]] const string& getTitle() const { return managerTitle_; }
};

// Step 3: Define Window class (WindowManager is fully declared above)
class Window {
private:
    string windowName_;
    WindowManager* manager_; // Pointer to WindowManager

public:
    Window(string name, WindowManager* mgr) 
        : windowName_(std::move(name)), manager_(mgr) {
        if (manager_) {
            manager_->registerWindow(this); // Cross-reference setup
        }
    }

    void redraw() const {
        cout << "  - [Window] Redrawing Window: '" << windowName_ << "'\n";
    }

    [[nodiscard]] const string& getName() const { return windowName_; }
};

// Step 4: Implement WindowManager methods AFTER Window is fully defined
void WindowManager::registerWindow(Window* win) {
    managedWindow_ = win;
    if (managedWindow_) {
        cout << "  - [WindowManager] Manager '" << managerTitle_ 
             << "' registered Window '" << managedWindow_->getName() << "'\n";
    }
}

void WindowManager::triggerWindowRefresh() {
    if (managedWindow_) {
        cout << "  - [WindowManager] Triggering refresh from Manager '" << managerTitle_ << "'...\n";
        managedWindow_->redraw(); // Valid because Window class is fully defined here!
    }
}


// =====================================================================================
// 2. BREAKING SMART POINTER CIRCULAR MEMORY LEAKS (shared_ptr + weak_ptr)
// If NodeA holds shared_ptr<NodeB> AND NodeB holds shared_ptr<NodeA>, memory LEAKS!
// Fix: One side must hold std::weak_ptr to break the ownership cycle.
// =====================================================================================

class NodeB; // Forward declaration for smart pointer node loop

class NodeA {
public:
    string name_;
    shared_ptr<NodeB> ptrB_; // Strong ownership to NodeB

    explicit NodeA(string name) : name_(std::move(name)) {
        cout << "  - [Constructed] NodeA '" << name_ << "'\n";
    }
    ~NodeA() {
        cout << "  - [Destructed ] NodeA '" << name_ << "' freed from memory!\n";
    }
};

class NodeB {
public:
    string name_;
    // CRITICAL FIX: Use weak_ptr instead of shared_ptr to prevent circular ref count leak!
    weak_ptr<NodeA> ptrA_; // Non-owning reference back to NodeA

    explicit NodeB(string name) : name_(std::move(name)) {
        cout << "  - [Constructed] NodeB '" << name_ << "'\n";
    }
    ~NodeB() {
        cout << "  - [Destructed ] NodeB '" << name_ << "' freed from memory!\n";
    }

    void pingNodeA() {
        // Lock weak_ptr to safely obtain a temporary shared_ptr
        if (auto lockedA = ptrA_.lock()) {
            cout << "  - [NodeB] Successfully pinged NodeA '" << lockedA->name_ << "' via weak_ptr!\n";
        } else {
            cout << "  - [NodeB] NodeA no longer exists (Expired weak_ptr).\n";
        }
    }
};


// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string windowTitleInput;

    // Dynamic input collection with stream flushing
    cout << "Enter Window Title name (e.g., MainWindow_GUI): " << flush;
    getline(cin, windowTitleInput);
    if (windowTitleInput.empty()) {
        windowTitleInput = "MainWindow_GUI";
    }

    // 1. COMPILATION CIRCULAR DEPENDENCY RESOLUTION (FORWARD DECLARATIONS)
    cout << "\n================ 1. CLASS DEPENDENCY LOOP RESOLUTION ================" << endl;
    WindowManager manager("DesktopManager_v1");
    Window win(windowTitleInput, &manager);

    manager.triggerWindowRefresh();

    // 2. SMART POINTER CIRCULAR LEAK RESOLUTION (shared_ptr + weak_ptr)
    cout << "\n================ 2. SMART POINTER CYCLIC LEAK RESOLUTION ================" << endl;
    cout << "Creating scope block for NodeA and NodeB mutual linkage:\n";
    {
        auto nodeA = make_shared<NodeA>("AlphaNode");
        auto nodeB = make_shared<NodeB>("BetaNode");

        // Establish mutual links
        nodeA->ptrB_ = nodeB; // NodeA strongly owns NodeB
        nodeB->ptrA_ = nodeA; // NodeB weakly references NodeA (breaks ownership cycle!)

        cout << "  - NodeA Ref Count: " << nodeA.use_count() << "\n";
        cout << "  - NodeB Ref Count: " << nodeB.use_count() << "\n";

        nodeB->pingNodeA();
        cout << "Exiting scope block (Destructors should run automatically)...\n";
    }
    cout << "Scope block exited successfully!\n";

    // 3. SUMMARY OF SOLUTIONS FOR CIRCULAR DEPENDENCIES
    cout << "\n================ CIRCULAR DEPENDENCY BEST PRACTICES ================" << endl;
    cout << "+---------------------------------+---------------------------------------------------+\n"
         << "| Problem Type                    | Recommended Resolution Strategy                   |\n"
         << "+---------------------------------+---------------------------------------------------+\n"
         << "| Header File Loop (#include)     | Use Forward Declarations (`class X;`) in headers; |\n"
         << "|                                 | push `#include` statements to `.cpp` files.       |\n"
         << "| Member Object Loop (Value T)    | Store pointers (`T*`), references (`T&`), or      |\n"
         << "|                                 | smart pointers instead of complete value types.   |\n"
         << "| Circular Ownership (Shared Ptr) | Use `std::weak_ptr<T>` for back-references to     |\n"
         << "|                                 | break reference cycles and avoid memory leaks.    |\n"
         << "| Code Organization               | Keep header files clean and minimal; place method  |\n"
         << "|                                 | bodies inside source (.cpp) files.                |\n"
         << "+---------------------------------+---------------------------------------------------+\n";

    return 0;
}