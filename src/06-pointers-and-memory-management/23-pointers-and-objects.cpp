/*
 * =====================================================================================
 * CONCEPT        : Pointers and Objects in Pointers and Memory Management (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the interplay between object-oriented programming, object lifetime,
 *                  polymorphic dynamic memory, and raw/smart pointers:
 *
 *                  1. Stack vs Heap Object Creation : Stack lifecycle vs `new` / `delete`.
 *                  2. Member Access Mechanics       : Dot (`(*ptr).member`) vs Arrow (`ptr->member`).
 *                  3. Object Member Pointers        : Data member pointers (`T Class::*`) and 
 *                                                     member function pointers (`R (Class::*)(Args)`).
 *                  4. Object Array Layouts          : Contiguous object array (`Object[]`) vs 
 *                                                     array of object pointers (`Object*[]`).
 *                  5. Polymorphism & Base Pointers  : Base class pointers (`Base*`), dynamic dispatch, 
 *                                                     and the critical role of `virtual ~Base()`.
 *                  6. Modern RAII Object Management : `std::unique_ptr<Object>` and 
 *                                                     `std::shared_ptr<Object>` with `std::make_unique`.
 *
 * TIME COMPLEXITY  : Object Creation / Access / Virtual Dispatch: O(1) constant time.
 * SPACE COMPLEXITY : Raw Object Pointer size: Uniform 8 bytes (64-bit) / 4 bytes (32-bit).
 *                    Virtual Table Pointer (`vptr`): Adds 8 bytes per polymorphic object.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <utility>

using namespace std;

// =====================================================================================
// HELPER CLASSES FOR OBJECT LIFECYCLE & MEMBER POINTER DEMONSTRATIONS
// =====================================================================================

class Player {
public:
    string name;
    int health;

    Player() : name("DefaultPlayer"), health(100) {
        cout << "    [Player Default Ctor] '" << name << "' created at address: " 
             << static_cast<const void*>(this) << "\n";
    }

    Player(string n, int h) : name(std::move(n)), health(h) {
        cout << "    [Player Param Ctor] '" << name << "' created (HP: " << health 
             << ") at address: " << static_cast<const void*>(this) << "\n";
    }

    ~Player() {
        cout << "    [Player Dtor] '" << name << "' destroyed at address: " 
             << static_cast<const void*>(this) << "\n";
    }

    void takeDamage(int damage) {
        health -= damage;
        cout << "    [Player Action] " << name << " took " << damage 
             << " damage. Health remaining: " << health << "\n";
    }

    void displayInfo() const {
        cout << "    [Player Info] Name: " << setw(12) << left << name 
             << " | Health: " << health << "\n";
    }
};

// =====================================================================================
// POLYMORPHIC CLASS HIERARCHY FOR DYNAMIC DISPATCH DEMONSTRATION
// =====================================================================================

class GameObject {
public:
    GameObject() {
        cout << "    [GameObject Ctor] Base class constructed at: " 
             << static_cast<const void*>(this) << "\n";
    }

    // CRITICAL: Virtual destructor ensures proper destruction order for derived objects!
    virtual ~GameObject() {
        cout << "    [GameObject Dtor] Base class destructed at: " 
             << static_cast<const void*>(this) << "\n";
    }

    virtual void render() const {
        cout << "    [GameObject Render] Rendering generic base game object.\n";
    }
};

class BossEnemy : public GameObject {
private:
    int bossPower_;

public:
    explicit BossEnemy(int power) : bossPower_(power) {
        cout << "    [BossEnemy Ctor] Derived Boss class constructed (Power: " << bossPower_ << ")\n";
    }

    ~BossEnemy() override {
        cout << "    [BossEnemy Dtor] Derived Boss class destructed.\n";
    }

    void render() const override {
        cout << "    [BossEnemy Render] Rendering massive Boss Enemy with attack power: " 
             << bossPower_ << "!\n";
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base health modifier for player objects (e.g., 50): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting health modifier to 50." << endl;
        userInputValue = 50;
    }

    // =====================================================================================
    // 1. STACK VS HEAP OBJECT CREATION & LIFECYCLE
    // =====================================================================================
    cout << "\n================ 1. STACK VS HEAP OBJECT LIFECYCLE ================\n";

    {
        cout << "  - [Stack Object Scope]: Constructing stack object `stackPlayer`...\n";
        Player stackPlayer("StackHero", 100 + userInputValue);
        stackPlayer.displayInfo();
        cout << "  - Leaving local block scope (Stack object destructor triggers automatically):\n";
    } // `stackPlayer` destroyed automatically HERE!

    cout << "\n  - [Heap Object]: Constructing dynamic object `heapPlayer` via `new`...\n";
    Player* heapPlayer = new Player("HeapHero", 200 + userInputValue);

    cout << "  - Heap Object Address Stored in Pointer: " 
         << static_cast<const void*>(heapPlayer) << "\n";

    cout << "  - Explicitly deleting `heapPlayer` via `delete` operator...\n";
    delete heapPlayer; // Invokes ~Player() then frees heap bytes
    heapPlayer = nullptr; // Safe disarm

    // =====================================================================================
    // 2. MEMBER ACCESS VIA POINTERS (`(*ptr).member` vs `ptr->member`)
    // =====================================================================================
    cout << "\n================ 2. MEMBER ACCESS MECHANICS ================\n";

    Player* playerPtr = new Player("CyberKnight", 150);

    // Method A: Dereference first, then apply dot operator
    cout << "  - Method A `(*playerPtr).name` : " << (*playerPtr).name << "\n";

    // Method B: Arrow operator shorthand (Preferred readable syntax)
    cout << "  - Method B `playerPtr->name`   : " << playerPtr->name << "\n";

    // Invoking member function via arrow operator
    playerPtr->takeDamage(userInputValue);

    delete playerPtr;
    playerPtr = nullptr;

    // =====================================================================================
    // 3. POINTERS TO CLASS DATA MEMBERS & MEMBER FUNCTIONS
    // =====================================================================================
    cout << "\n================ 3. OBJECT MEMBER POINTERS (`Class::*`) ================\n";

    Player memberDemoObj("MemberTarget", 300);

    // A. Pointer to Class Data Member
    int Player::* healthMemberPtr = &Player::health;
    cout << "  - Accessing `Player::health` via data member pointer: " 
         << memberDemoObj.*healthMemberPtr << "\n";

    // Mutating member through data member pointer
    memberDemoObj.*healthMemberPtr = 500;
    cout << "  - Mutated `Player::health` via member ptr: " << memberDemoObj.health << "\n";

    // B. Pointer to Class Member Function
    void (Player::* actionFuncPtr)(int) = &Player::takeDamage;

    cout << "  - Invoking `takeDamage()` via member function pointer (`.*` operator):\n";
    (memberDemoObj.*actionFuncPtr)(40);

    // =====================================================================================
    // 4. OBJECT ARRAYS VS ARRAY OF OBJECT POINTERS
    // =====================================================================================
    cout << "\n================ 4. OBJECT ARRAYS VS ARRAY OF OBJECT POINTERS ================\n";

    constexpr size_t ARRAY_SIZE = 2;

    // Layout A: Contiguous Dynamic Object Array (`Player[]`)
    cout << "  - [Layout A] Allocating contiguous array of objects (`new Player[N]`):\n";
    Player* objectArray = new Player[ARRAY_SIZE]; // Requires default constructors!

    cout << "  - Deallocating contiguous object array via `delete[]`...\n";
    delete[] objectArray;
    objectArray = nullptr;

    // Layout B: Dynamic Array of Object Pointers (`Player*[]`)
    cout << "\n  - [Layout B] Allocating array of object pointers (`new Player*[N]`):\n";
    Player** ptrArray = new Player*[ARRAY_SIZE];

    ptrArray[0] = new Player("ArrayHero1", 120);
    ptrArray[1] = new Player("ArrayHero2", 140);

    cout << "  - Deallocating array of object pointers (Requires element-by-element delete):\n";
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        delete ptrArray[i]; // Delete individual objects
    }
    delete[] ptrArray; // Delete pointer header array
    ptrArray = nullptr;

    // =====================================================================================
    // 5. POLYMORPHISM, BASE CLASS POINTERS & VIRTUAL DESTRUCTORS
    // =====================================================================================
    cout << "\n================ 5. POLYMORPHISM & VIRTUAL DESTRUCTORS ================\n";

    cout << "  - Creating Derived `BossEnemy` object via Base `GameObject*` pointer...\n";
    GameObject* basePtr = new BossEnemy(999);

    cout << "\n  - Invoking virtual function `basePtr->render()` (Dynamic Dispatch):\n";
    basePtr->render(); // Calls BossEnemy::render() due to vtable lookup

    cout << "\n  - Deallocating polymorphic object via Base pointer `delete basePtr`...\n";
    // CRITICAL: Because ~GameObject() is virtual, destructor calls derived ~BossEnemy() first!
    delete basePtr;
    basePtr = nullptr;

    // =====================================================================================
    // 6. MODERN RAII OBJECT MANAGEMENT (`std::unique_ptr<Object>`)
    // =====================================================================================
    cout << "\n================ 6. MODERN RAII OBJECT MANAGEMENT ================\n";

    {
        cout << "  - Creating `std::unique_ptr<Player>` via `std::make_unique`...\n";
        std::unique_ptr<Player> smartPlayer = std::make_unique<Player>("SmartHero", 250);

        smartPlayer->takeDamage(30);
        smartPlayer->displayInfo();

        cout << "  - Leaving scope (Smart pointer automatically destructs object and frees heap memory):\n";
    } // Automatic object destruction HERE!

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ POINTERS AND OBJECTS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Paradigm    | Syntax Example                    | Operational Trait & Memory Impact |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Stack Object          | `Player obj(\"A\", 100);`           | Auto stack allocation & scope cleanup|\n"
         << "| Heap Object           | `Player* p = new Player();`       | Persistent heap memory, manual free|\n"
         << "| Member Access Arrow   | `ptr->member;`                    | Equivalent to `(*ptr).member`     |\n"
         << "| Member Data Pointer   | `int Player::*p = &Player::hp;`   | Offset pointer to class member    |\n"
         << "| Member Func Pointer   | `void (Player::*p)(int);`         | Invokes method via `(obj.*p)(arg)`|\n"
         << "| Object Array          | `Player* arr = new Player[N];`    | Contiguous storage, default ctors |\n"
         << "| Array of Object Ptrs  | `Player** arr = new Player*[N];`  | Flexible individual object allocations|\n"
         << "| Polymorphic Base Ptr  | `Base* p = new Derived();`        | Dynamic dispatch requires `virtual`|\n"
         << "| Smart Object Pointer  | `auto p = std::make_unique<T>();` | Modern RAII zero-leak object handling|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}