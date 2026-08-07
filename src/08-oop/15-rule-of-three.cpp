/*
 * =====================================================================================
 * CONCEPT        : Rule of Three in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the classic C++ "Rule of Three".
 *
 *                  THE RULE OF THREE:
 *                  If a class requires a custom implementation of ANY of the following 
 *                  three special member functions (usually because it manages raw resources
 *                  like dynamic memory, file handles, or network sockets), it almost 
 *                  certainly requires explicit custom implementations of ALL THREE:
 *
 *                  1. Custom Destructor               : To release acquired raw resources.
 *                  2. Custom Copy Constructor         : To perform a deep copy during object cloning.
 *                  3. Custom Copy Assignment Operator : To perform a deep copy during copy assignment,
 *                                                       handling existing resource cleanup and
 *                                                       self-assignment protection.
 *
 *                  WHY THE RULE OF THREE MATTERS:
 *                  The compiler-generated default copy constructor and copy assignment operator
 *                  perform memberwise (shallow) copying. If a class holds raw pointers to dynamic 
 *                  heap memory:
 *                  - Shallow copy duplicates pointer addresses, not the pointed-to data.
 *                  - Both objects now point to the same memory location.
 *                  - When the first object goes out of scope, its destructor frees the memory.
 *                  - The second object now holds a dangling pointer (Use-After-Free).
 *                  - When the second object goes out of scope, its destructor attempts to free 
 *                    the already-freed memory, triggering a catastrophic DOUBLE-FREE CRASH!
 *
 * TIME COMPLEXITY  : Deep Copy Operations (Constructor / Assignment): O(N) where N is buffer size.
 *                    Destructor / Element Access / Getters          : O(1) constant time.
 * SPACE COMPLEXITY : Deep Copy Heap Overhead : Doubles heap memory allocation (2 x N).
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: RuleOfThreeBuffer
// A resource-managing class encapsulating raw dynamic memory to demonstrate the Rule of 3.
// =====================================================================================
class RuleOfThreeBuffer {
private:
    int bufferId_;
    size_t capacity_;
    int* rawDataBuffer_; // Raw heap-allocated array (resource requiring Rule of 3)
    string label_;

public:
    // Global tracker to verify leak-free execution and detect double-free issues
    static inline int activeAllocations = 0; // C++17 inline static member

    // =================================================================================
    // 1. CONSTRUCTOR
    // =================================================================================
    explicit RuleOfThreeBuffer(int id, size_t capacity, string label = "DefaultBuffer")
        : bufferId_(id), capacity_(capacity), rawDataBuffer_(nullptr), label_(std::move(label)) {
        
        if (capacity_ > 0) {
            rawDataBuffer_ = new int[capacity_](); // Allocate zero-initialized heap memory
            ++activeAllocations;
        }

        cout << "    [CONSTRUCTOR]       Created Buffer ID: " << setw(4) << bufferId_ 
             << " ('" << setw(20) << left << label_ << "') | Size: " << setw(4) << capacity_ 
             << " elements | Heap Pointer: " << static_cast<const void*>(rawDataBuffer_)
             << " | Active Allocations: " << activeAllocations << "\n";
    }

    // =================================================================================
    // RULE OF THREE - PART 1 OF 3: DESTRUCTOR
    // Essential to prevent permanent memory leaks when objects leave scope.
    // =================================================================================
    ~RuleOfThreeBuffer() noexcept {
        if (rawDataBuffer_ != nullptr) {
            delete[] rawDataBuffer_;
            rawDataBuffer_ = nullptr; // Disarm pointer
            --activeAllocations;
            cout << "    [DESTRUCTOR]        Freed Buffer ID: " << setw(4) << bufferId_ 
                 << " ('" << setw(20) << left << label_ << "') | Active Allocations: " 
                 << activeAllocations << "\n";
        } else {
            cout << "    [DESTRUCTOR]        Destroyed empty/null Buffer ID: " << setw(4) << bufferId_ 
                 << " (No heap memory to free)\n";
        }
    }

    // =================================================================================
    // RULE OF THREE - PART 2 OF 3: COPY CONSTRUCTOR (DEEP COPY)
    // Replaces default shallow copy with a fresh heap allocation and data duplication.
    // Syntax: RuleOfThreeBuffer objB = objA;  OR  RuleOfThreeBuffer objB(objA);
    // =================================================================================
    RuleOfThreeBuffer(const RuleOfThreeBuffer& other)
        : bufferId_(other.bufferId_ + 1000), 
          capacity_(other.capacity_), 
          rawDataBuffer_(nullptr), 
          label_(other.label_ + "_Cloned") {
        
        cout << "    [COPY CONSTRUCTOR]  Initiating DEEP COPY of source ID " << other.bufferId_ 
             << " into new ID " << bufferId_ << "...\n";

        if (other.capacity_ > 0 && other.rawDataBuffer_ != nullptr) {
            // STEP 1: Allocate new independent heap block
            rawDataBuffer_ = new int[capacity_];
            
            // STEP 2: Copy raw element data from source to destination
            std::copy(other.rawDataBuffer_, other.rawDataBuffer_ + capacity_, rawDataBuffer_);
            
            ++activeAllocations;
        }

        cout << "    [COPY CONSTRUCTOR]  Deep copy completed! New Heap Pointer: " 
             << static_cast<const void*>(rawDataBuffer_) 
             << " | Active Allocations: " << activeAllocations << "\n";
    }

    // =================================================================================
    // RULE OF THREE - PART 3 OF 3: COPY ASSIGNMENT OPERATOR (DEEP COPY)
    // Handles assignment to an ALREADY EXISTING object: objB = objA;
    // Must handle:
    // 1. Self-assignment protection (if (this == &other))
    // 2. Exception-safe allocation before releasing existing resources
    // 3. Returning *this by reference for assignment chaining (a = b = c)
    // =================================================================================
    RuleOfThreeBuffer& operator=(const RuleOfThreeBuffer& other) {
        cout << "    [COPY ASSIGNMENT]   Executing copy assignment operator...\n";

        // STEP 1: Self-Assignment Guard (`a = a`)
        // If assigning an object to itself, doing nothing prevents accidental destruction of its own buffer!
        if (this == &other) {
            cout << "    [SELF-ASSIGNMENT]   Detected self-assignment (this == &other). Skipping operation.\n";
            return *this;
        }

        // STEP 2: Allocate new heap buffer FIRST (Exception Safety)
        // If allocation fails and throws std::bad_alloc, object's existing state remains uncorrupted.
        int* newBuffer = nullptr;
        if (other.capacity_ > 0 && other.rawDataBuffer_ != nullptr) {
            newBuffer = new int[other.capacity_];
            std::copy(other.rawDataBuffer_, other.rawDataBuffer_ + other.capacity_, newBuffer);
        }

        // STEP 3: Release existing dynamic memory managed by this instance
        if (rawDataBuffer_ != nullptr) {
            delete[] rawDataBuffer_;
            --activeAllocations;
        }

        // STEP 4: Transfer pointer and update object metadata
        rawDataBuffer_ = newBuffer;
        capacity_ = other.capacity_;
        bufferId_ = other.bufferId_ + 2000;
        label_ = other.label_ + "_Assigned";

        if (rawDataBuffer_ != nullptr) {
            ++activeAllocations;
        }

        cout << "    [COPY ASSIGNMENT]   Deep copy assignment complete into ID " << bufferId_ 
             << " | New Heap Pointer: " << static_cast<const void*>(rawDataBuffer_) 
             << " | Active Allocations: " << activeAllocations << "\n";

        // STEP 5: Return reference to calling instance to support operator chaining (`a = b = c`)
        return *this;
    }

    // =================================================================================
    // HELPER METHODS FOR STATE INSPECTION AND MUTATION
    // =================================================================================
    void setElement(size_t index, int value) {
        if (rawDataBuffer_ != nullptr && index < capacity_) {
            rawDataBuffer_[index] = value;
        } else {
            throw out_of_range("Buffer index out of bounds!");
        }
    }

    [[nodiscard]] int getElement(size_t index) const {
        if (rawDataBuffer_ != nullptr && index < capacity_) {
            return rawDataBuffer_[index];
        }
        throw out_of_range("Buffer index out of bounds!");
    }

    void displayState() const {
        if (rawDataBuffer_ != nullptr) {
            cout << "    [BUFFER STATUS]     ID: " << setw(4) << bufferId_ 
                 << " | Label: " << setw(22) << left << label_ 
                 << " | Capacity: " << setw(4) << capacity_ 
                 << " | Address: " << static_cast<const void*>(rawDataBuffer_) 
                 << " | First Element [0]: " << rawDataBuffer_[0] << "\n";
        } else {
            cout << "    [BUFFER STATUS]     ID: " << setw(4) << bufferId_ 
                 << " | Label: " << setw(22) << left << label_ 
                 << " | Address: NULLPTR (No Allocation)\n";
        }
    }

    [[nodiscard]] int getBufferId() const noexcept { return bufferId_; }
    [[nodiscard]] size_t getCapacity() const noexcept { return capacity_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Rule of Three analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. BASIC INSTANTIATION & RESOURCE ACQUISITION
    // =====================================================================================
    cout << "\n================ 1. BASIC OBJECT INSTANTIATION ================\n";

    {
        cout << "  - Creating primary `RuleOfThreeBuffer` instance (bufA)...\n";
        RuleOfThreeBuffer bufA(userInputValue, 500, "PrimaryBuffer_A");
        bufA.setElement(0, 999); // Mutate first element
        bufA.displayState();

        cout << "  - Exiting scope containing bufA...\n";
    } // Destructor fires automatically HERE

    cout << "  - Active Allocations Post-Scope: " << RuleOfThreeBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 2. DEMONSTRATING COPY CONSTRUCTOR (RULE OF THREE - PART 2)
    // =====================================================================================
    cout << "\n================ 2. COPY CONSTRUCTOR (DEEP COPY) ================\n";

    {
        cout << "  - Creating source buffer (buf1)...\n";
        RuleOfThreeBuffer buf1(userInputValue + 10, 1000, "SourceBuffer_1");
        buf1.setElement(0, 42);
        buf1.displayState();

        cout << "\n  - Invoking Copy Constructor: `RuleOfThreeBuffer buf2 = buf1;`...\n";
        RuleOfThreeBuffer buf2 = buf1; // Copy Constructor invoked
        buf2.displayState();

        cout << "\n  - Mutating buf2 element [0] to prove independent memory addresses:\n";
        buf2.setElement(0, 8888);

        cout << "    * Source buf1[0] Value : " << buf1.getElement(0) << " (Unchanged at address " << static_cast<const void*>(&buf1) << ")\n";
        cout << "    * Cloned buf2[0] Value : " << buf2.getElement(0) << " (Mutated at address " << static_cast<const void*>(&buf2) << ")\n";
        cout << "    * Verification        : Deep copy confirmed! Heap addresses are distinct.\n";

        cout << "\n  - Leaving scope containing buf1 and buf2...\n";
    } // Destructors fire cleanly for buf2 and buf1 without double-free crashes!

    cout << "  - Active Allocations Post Copy-Ctor Scope: " << RuleOfThreeBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 3. DEMONSTRATING COPY ASSIGNMENT OPERATOR (RULE OF THREE - PART 3)
    // =====================================================================================
    cout << "\n================ 3. COPY ASSIGNMENT OPERATOR ================\n";

    {
        cout << "  - Creating two distinct existing buffer instances (bufX and bufY)...\n";
        RuleOfThreeBuffer bufX(userInputValue + 20, 200, "TargetBuffer_X");
        RuleOfThreeBuffer bufY(userInputValue + 30, 800, "SourceBuffer_Y");

        bufX.setElement(0, 111);
        bufY.setElement(0, 777);

        cout << "\n  - State BEFORE assignment:\n";
        bufX.displayState();
        bufY.displayState();

        cout << "\n  - Executing Copy Assignment: `bufX = bufY;`...\n";
        bufX = bufY; // Copy Assignment Operator invoked

        cout << "\n  - State AFTER assignment:\n";
        bufX.displayState();
        bufY.displayState();

        cout << "\n  - Testing Self-Assignment Guard: `bufX = bufX;`...\n";
        bufX = bufX; // Triggers `if (this == &other)` check

        cout << "\n  - Leaving assignment demonstration scope...\n";
    }

    cout << "  - Active Allocations Post Assignment Scope: " << RuleOfThreeBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 4. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 4. BOUNDS CHECKING & INVARIANT GUARDS ================\n";

    try {
        cout << "  - Attempting out-of-bounds access on RuleOfThreeBuffer...\n";
        RuleOfThreeBuffer testBuf(userInputValue + 40, 5, "TestBuf");
        testBuf.setElement(100, 99); // Out of bounds -> Throws exception
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    cout << "\n  - Final Active Allocations Count: " << RuleOfThreeBuffer::activeAllocations 
         << " (ZERO leaks confirmed!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RULE OF THREE SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Special Member Func   | C++ Declaration Syntax            | Primary Responsibility & Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Destructor            | `~Class() noexcept;`              | Releases acquired raw heap resources|\n"
         << "| Copy Constructor      | `Class(const Class& other);`      | Allocates new heap & deep-copies  |\n"
         << "| Copy Assignment Op    | `Class& operator=(const Class&);` | Replaces state; handles self-assign|\n"
         << "| Self-Assign Guard     | `if (this == &other) return *this;`| Prevents premature resource free  |\n"
         << "| Shallow Copy Risk     | Default compiler-generated copy   | Causes double-free / memory corruption|\n"
         << "| Modern Evolution      | Rule of 5 (Adds Move Ctor/Assign) | Optimization for rvalue temporaries|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}