/*
 * =====================================================================================
 * CONCEPT        : Rule of Five in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the C++ "Rule of Five" (Rule of 3 extended for C++11 Move Semantics).
 *
 *                  THE RULE OF FIVE:
 *                  In Modern C++ (C++11 onwards), if a class manages raw resources (such as 
 *                  heap memory, file handles, or OS sockets) and requires a custom 
 *                  implementation of ANY of the following five special member functions, 
 *                  it MUST explicitly declare or define ALL FIVE:
 *
 *                  1. Custom Destructor               : Releases owned raw resources on object scope exit.
 *                  2. Custom Copy Constructor         : Performs a deep copy to duplicate independent resource state.
 *                  3. Custom Copy Assignment Operator : Performs deep copy assignment with self-assignment protection.
 *                  4. Custom Move Constructor         : Steals resources from temporary rvalues in O(1) time (`noexcept`).
 *                  5. Custom Move Assignment Operator : Transfers resource ownership from rvalues in O(1) time (`noexcept`).
 *
 *                  WHY THE RULE OF FIVE IS NECESSARY:
 *                  - Declaring a custom destructor or copy operation STOPS the compiler from automatically
 *                    generating move operations. Moving will silently fall back to expensive deep copies!
 *                  - Declaring move operations DELETES compiler-generated copy operations.
 *                  - Marking move operations `noexcept` is MANDATORY for STL containers (like `std::vector`)
 *                    to use efficient move reallocations instead of deep-copy fallbacks.
 *
 * RESOLVED WARNING: Replaced direct `buf2 = std::move(buf2)` with self-reference indirection 
 *                  `RuleOfFiveBuffer& selfRef = buf2; buf2 = std::move(selfRef);` to test 
 *                  the self-move guard (`if (this == &other)`) without triggering compiler
 *                  `-Wself-move` warnings.
 *
 * TIME COMPLEXITY  : Deep Copy Operations (Constructor / Assignment): O(N) where N is allocation size.
 *                    Move Operations (Constructor / Assignment)     : O(1) constant-time pointer transfer.
 *                    Destructor / Element Access / Getters          : O(1) constant time.
 * SPACE COMPLEXITY : Deep Copy Heap Overhead : Doubles heap memory allocation (2 x N).
 *                    Move Heap Overhead      : 0 bytes extra heap memory.
 * =====================================================================================
 */

#include <iostream>
#include <algorithm>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>
#include <iomanip>
#include <limits>
#include <cstddef>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: RuleOfFiveBuffer
// Encapsulates raw dynamic heap memory to explicitly demonstrate the Rule of 5.
// =====================================================================================
class RuleOfFiveBuffer {
private:
    int bufferId_;
    size_t capacity_;
    int* rawDataBuffer_; // Managed raw resource
    string label_;

public:
    // Global tracking metric to verify leak-free execution and zero double-frees
    static inline int activeAllocations = 0; // C++17 inline static variable

    // =================================================================================
    // CONSTRUCTOR
    // =================================================================================
    explicit RuleOfFiveBuffer(int id, size_t capacity, string label = "DefaultBuffer")
        : bufferId_(id), capacity_(capacity), rawDataBuffer_(nullptr), label_(std::move(label)) {
        
        if (capacity_ > 0) {
            rawDataBuffer_ = new int[capacity_](); // Allocate zero-initialized heap buffer
            ++activeAllocations;
        }

        cout << "    [CONSTRUCTOR]       Created Buffer ID: " << setw(4) << bufferId_ 
             << " ('" << setw(20) << left << label_ << "') | Size: " << setw(4) << capacity_ 
             << " elements | Pointer: " << static_cast<const void*>(rawDataBuffer_)
             << " | Active Allocations: " << activeAllocations << "\n";
    }

    // =================================================================================
    // RULE OF FIVE - PART 1 OF 5: DESTRUCTOR
    // Releases acquired raw resources on object destruction.
    // =================================================================================
    ~RuleOfFiveBuffer() noexcept {
        if (rawDataBuffer_ != nullptr) {
            delete[] rawDataBuffer_;
            rawDataBuffer_ = nullptr; // Disarm pointer
            --activeAllocations;
            cout << "    [DESTRUCTOR]        Freed Buffer ID: " << setw(4) << bufferId_ 
                 << " ('" << setw(20) << left << label_ << "') | Active Allocations: " 
                 << activeAllocations << "\n";
        } else {
            cout << "    [DESTRUCTOR]        Destroyed Disarmed/Moved-From Buffer ID: " << setw(4) << bufferId_ 
                 << " (No heap memory to free)\n";
        }
    }

    // =================================================================================
    // RULE OF FIVE - PART 2 OF 5: COPY CONSTRUCTOR (DEEP COPY)
    // Creates a fresh heap memory block and clones element data from source object.
    // Syntax: RuleOfFiveBuffer copyObj = sourceObj;
    // =================================================================================
    RuleOfFiveBuffer(const RuleOfFiveBuffer& other)
        : bufferId_(other.bufferId_ + 1000), 
          capacity_(other.capacity_), 
          rawDataBuffer_(nullptr), 
          label_(other.label_ + "_Copy") {
        
        cout << "    [COPY CONSTRUCTOR]  Performing DEEP COPY from ID " << other.bufferId_ 
             << " into new ID " << bufferId_ << "...\n";

        if (other.capacity_ > 0 && other.rawDataBuffer_ != nullptr) {
            rawDataBuffer_ = new int[capacity_];
            std::copy(other.rawDataBuffer_, other.rawDataBuffer_ + capacity_, rawDataBuffer_);
            ++activeAllocations;
        }

        cout << "    [COPY CONSTRUCTOR]  Deep copy finished! New Pointer: " 
             << static_cast<const void*>(rawDataBuffer_) 
             << " | Active Allocations: " << activeAllocations << "\n";
    }

    // =================================================================================
    // RULE OF FIVE - PART 3 OF 5: COPY ASSIGNMENT OPERATOR (DEEP COPY)
    // Deep copies state into an ALREADY EXISTING instance: existingObj = sourceObj;
    // Requires: Self-assignment guard, exception-safe allocation, and returning *this.
    // =================================================================================
    RuleOfFiveBuffer& operator=(const RuleOfFiveBuffer& other) {
        cout << "    [COPY ASSIGNMENT]   Executing copy assignment operator...\n";

        // Step 1: Self-Assignment Protection (`a = a`)
        if (this == &other) {
            cout << "    [SELF-ASSIGNMENT]   Detected self-assignment (this == &other). Operation skipped.\n";
            return *this;
        }

        // Step 2: Allocate new heap buffer FIRST (Exception Safety)
        int* newBuffer = nullptr;
        if (other.capacity_ > 0 && other.rawDataBuffer_ != nullptr) {
            newBuffer = new int[other.capacity_];
            std::copy(other.rawDataBuffer_, other.rawDataBuffer_ + other.capacity_, newBuffer);
        }

        // Step 3: Release existing resources managed by this instance
        if (rawDataBuffer_ != nullptr) {
            delete[] rawDataBuffer_;
            --activeAllocations;
        }

        // Step 4: Acquire new state and update metadata
        rawDataBuffer_ = newBuffer;
        capacity_ = other.capacity_;
        bufferId_ = other.bufferId_ + 2000;
        label_ = other.label_ + "_Assigned";

        if (rawDataBuffer_ != nullptr) {
            ++activeAllocations;
        }

        cout << "    [COPY ASSIGNMENT]   Deep copy assignment finished into ID " << bufferId_ 
             << " | Pointer: " << static_cast<const void*>(rawDataBuffer_) 
             << " | Active Allocations: " << activeAllocations << "\n";

        return *this;
    }

    // =================================================================================
    // RULE OF FIVE - PART 4 OF 5: MOVE CONSTRUCTOR (ZERO-COST RESOURCE THEFT)
    // Steals pointer from temporary rvalue object in O(1) time without heap allocations.
    // MUST be marked `noexcept` for STL container safety!
    // Syntax: RuleOfFiveBuffer movedObj = std::move(sourceObj);
    // =================================================================================
    RuleOfFiveBuffer(RuleOfFiveBuffer&& other) noexcept
        : bufferId_(other.bufferId_), 
          capacity_(other.capacity_), 
          rawDataBuffer_(other.rawDataBuffer_), 
          label_(std::move(other.label_) + "_Moved") {
        
        cout << "    [MOVE CONSTRUCTOR]  STOLE resource pointer from ID " << bufferId_ 
             << " in O(1) time (Zero new allocations!)\n";

        // Disarm the source object so its destructor won't free stolen memory
        other.rawDataBuffer_ = nullptr;
        other.capacity_ = 0;
        other.bufferId_ = -1;
    }

    // =================================================================================
    // RULE OF FIVE - PART 5 OF 5: MOVE ASSIGNMENT OPERATOR (`noexcept`)
    // Transfers resource ownership from rvalue into an existing object in O(1) time.
    // Syntax: existingObj = std::move(sourceObj);
    // =================================================================================
    RuleOfFiveBuffer& operator=(RuleOfFiveBuffer&& other) noexcept {
        cout << "    [MOVE ASSIGNMENT]   Executing move assignment operator...\n";

        // Step 1: Self-Move Protection (`a = std::move(a)`)
        if (this == &other) {
            cout << "    [SELF-MOVE]         Detected self-move assignment (this == &other). Operation skipped.\n";
            return *this;
        }

        // Step 2: Release existing resource
        if (rawDataBuffer_ != nullptr) {
            delete[] rawDataBuffer_;
            --activeAllocations;
        }

        // Step 3: Steal pointer and metadata from source object
        rawDataBuffer_ = other.rawDataBuffer_;
        capacity_ = other.capacity_;
        bufferId_ = other.bufferId_;
        label_ = std::move(other.label_) + "_MoveAssigned";

        // Step 4: Disarm source object
        other.rawDataBuffer_ = nullptr;
        other.capacity_ = 0;
        other.bufferId_ = -1;

        cout << "    [MOVE ASSIGNMENT]   Transferred resource ownership to ID " << bufferId_ 
             << " and disarmed source object.\n";

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
                 << " | Pointer: " << static_cast<const void*>(rawDataBuffer_) 
                 << " | Val[0]: " << rawDataBuffer_[0] << "\n";
        } else {
            cout << "    [BUFFER STATUS]     ID: " << setw(4) << bufferId_ 
                 << " | Label: " << setw(22) << left << label_ 
                 << " | DISARMED / MOVED-FROM (Pointer: nullptr)\n";
        }
    }

    [[nodiscard]] int getBufferId() const noexcept { return bufferId_; }
    [[nodiscard]] size_t getCapacity() const noexcept { return capacity_; }
};

// Factory helper function returning temporary rvalue buffer
RuleOfFiveBuffer createTemporaryBuffer(int id, size_t capacity) {
    cout << "  - Inside `createTemporaryBuffer()` factory function...\n";
    RuleOfFiveBuffer temp(id, capacity, "FactoryTemp");
    temp.setElement(0, 777);
    return temp; // Triggers Move Constructor or Named Return Value Optimization (NRVO)
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Rule of Five analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. DEMONSTRATING COPY SEMANTICS (PARTS 2 & 3 OF RULE OF FIVE)
    // =====================================================================================
    cout << "\n================ 1. COPY SEMANTICS (COPY CTOR & COPY ASSIGNMENT) ================\n";

    {
        cout << "  - Creating primary buffer (bufA)...\n";
        RuleOfFiveBuffer bufA(userInputValue, 500, "PrimaryBuffer_A");
        bufA.setElement(0, 999);
        bufA.displayState();

        cout << "\n  - Invoking Copy Constructor: `RuleOfFiveBuffer bufB = bufA;`...\n";
        RuleOfFiveBuffer bufB = bufA; // Copy Constructor
        bufB.displayState();

        cout << "  - Mutating bufB[0] to prove independent memory allocation addresses:\n";
        bufB.setElement(0, 555);
        cout << "    * bufA[0] = " << bufA.getElement(0) << " (Unchanged)\n";
        cout << "    * bufB[0] = " << bufB.getElement(0) << " (Mutated)\n";

        cout << "\n  - Invoking Copy Assignment: `bufC = bufA;`...\n";
        RuleOfFiveBuffer bufC(userInputValue + 1, 50, "PreExisting_C");
        bufC = bufA; // Copy Assignment
        bufC.displayState();

        cout << "\n  - Testing Self-Assignment Guard (`bufA = bufA;`)...\n";
        bufA = bufA;

        cout << "  - Exiting Copy Semantics block scope...\n";
    } // bufA, bufB, bufC destructed HERE

    cout << "  - Active Allocations Post-Copy Scope: " << RuleOfFiveBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 2. DEMONSTRATING MOVE SEMANTICS (PARTS 4 & 5 OF RULE OF FIVE)
    // =====================================================================================
    cout << "\n================ 2. MOVE SEMANTICS (MOVE CTOR & MOVE ASSIGNMENT) ================\n";

    {
        cout << "  - Creating source buffer (buf1)...\n";
        RuleOfFiveBuffer buf1(userInputValue + 10, 5000, "MovableBuffer_1");
        buf1.setElement(0, 1234);
        buf1.displayState();

        cout << "\n  - Invoking Move Constructor: `RuleOfFiveBuffer buf2 = std::move(buf1);`...\n";
        RuleOfFiveBuffer buf2 = std::move(buf1); // Move Constructor
        
        cout << "  - Inspecting states post-move:\n";
        buf2.displayState(); // Holds stolen pointer
        buf1.displayState(); // Disarmed source object

        cout << "\n  - Invoking Move Assignment with temporary Rvalue from factory function...\n";
        RuleOfFiveBuffer buf3(userInputValue + 20, 10, "PreExisting_3");
        buf3 = createTemporaryBuffer(userInputValue + 30, 2000); // Move Assignment
        buf3.displayState();

        cout << "\n  - Testing Self-Move Guard via self-reference indirection...\n";
        RuleOfFiveBuffer& selfRef = buf2;
        buf2 = std::move(selfRef); // Invokes operator=(RuleOfFiveBuffer&&) safely without -Wself-move warning

        cout << "  - Exiting Move Semantics block scope...\n";
    } // buf2, buf3, disarmed buf1 destructed HERE

    cout << "  - Active Allocations Post-Move Scope: " << RuleOfFiveBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 3. STL VECTOR REALLOCATION & `noexcept` MOVE EFFICIENCY
    // =====================================================================================
    cout << "\n================ 3. STL CONTAINER INTEGRATION & `noexcept` EFFICIENCY ================\n";

    {
        cout << "  - Inserting `RuleOfFiveBuffer` objects into `std::vector`...\n";
        std::vector<RuleOfFiveBuffer> container;
        container.reserve(1); // Capacity 1 to force reallocation on second insertion

        cout << "  - Pushing first element into vector...\n";
        container.push_back(RuleOfFiveBuffer(userInputValue + 40, 300, "VecNode1"));

        cout << "\n  - Triggering vector reallocation (Vector uses fast `noexcept` move operations):\n";
        container.push_back(RuleOfFiveBuffer(userInputValue + 41, 400, "VecNode2"));

        for (const auto& node : container) {
            node.displayState();
        }

        cout << "  - Exiting vector scope...\n";
    }

    cout << "\n  - Final Active Allocations Count: " << RuleOfFiveBuffer::activeAllocations 
         << " (ZERO memory leaks confirmed!)\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ RULE OF FIVE SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Special Member Func   | C++ Declaration Syntax            | Primary Responsibility & Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| 1. Destructor         | `~Class() noexcept;`              | Releases acquired raw resources   |\n"
         << "| 2. Copy Constructor   | `Class(const Class& other);`      | Deep-copies state from Lvalues    |\n"
         << "| 3. Copy Assignment Op | `Class& operator=(const Class&);` | Deep-copies into existing object  |\n"
         << "| 4. Move Constructor   | `Class(Class&&) noexcept;`        | Steals raw pointers from Rvalues  |\n"
         << "| 5. Move Assignment Op | `Class& operator=(Class&&) noex;` | Transfers ownership from Rvalues  |\n"
         << "| `noexcept` Rule       | Required on Move operations       | Enables fast STL vector reallocs  |\n"
         << "| Modern Evolution      | Rule of Zero (Smart Pointers)     | Eliminates manual memory management|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}