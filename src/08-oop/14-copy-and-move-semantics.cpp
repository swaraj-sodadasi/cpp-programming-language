/*
 * =====================================================================================
 * CONCEPT        : Copy Semantics vs. Move Semantics in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  resource management, deep-copy cloning, and zero-cost move transfers:
 *
 *                  1. Deep Copy Semantics      : Duplicate allocations ensuring independent memory
 *                                                 ownership (`Copy Constructor` & `Copy Assignment`).
 *                  2. Move Semantics (C++11+)  : Resource theft from temporary rvalues (`Move Ctor`
 *                                                 & `Move Assignment`), avoiding heap re-allocations.
 *                  3. Rvalue References (`&&`) : Binding to temporary rvalues to transfer ownership safely.
 *                  4. Rule of 5 Compliance     : Cohesive implementation of Destructor, Copy Ctor, 
 *                                                 Copy Assign, Move Ctor, and Move Assign.
 *                  5. `noexcept` Guarantee     : Essential for STL container optimizations (e.g., 
 *                                                 `std::vector` reallocation safety).
 *                  6. Self-Assignment Protection: Safe handling of `a = a` and `a = std::move(a)`.
 *
 * RESOLVED WARNING: Replaced direct `bufB = std::move(bufB)` with self-reference indirection 
 *                  `DynamicBuffer& selfRef = bufB; bufB = std::move(selfRef);` to test 
 *                  the self-move guard (`if (this == &other)`) without triggering compiler
 *                  `-Wself-move` warnings.
 *
 * TIME COMPLEXITY  : Deep Copy Operations     : O(N) allocation and element copy.
 *                    Move Operations          : O(1) constant-time pointer exchange.
 * SPACE COMPLEXITY : Deep Copy Footprint      : Doubles heap memory footprint (2 x N).
 *                    Move Footprint           : 0 bytes extra heap memory.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>
#include <limits>
#include <cstddef>
#include <algorithm>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: DynamicBuffer (Rule of 5 Implementation)
// Encapsulates dynamic heap memory to explicitly trace Copy vs Move semantics.
// =====================================================================================
class DynamicBuffer {
private:
    int bufferId_;
    size_t capacity_;
    int* dataBuffer_;
    string label_;

public:
    static inline int activeAllocations = 0; // Tracks active dynamic heap buffers

    // =================================================================================
    // 1. PARAMETERIZED CONSTRUCTOR & DESTRUCTOR
    // =================================================================================
    explicit DynamicBuffer(int id, size_t capacity, string label = "DefaultBuffer")
        : bufferId_(id), capacity_(capacity), dataBuffer_(nullptr), label_(std::move(label)) {
        if (capacity_ > 0) {
            dataBuffer_ = new int[capacity_](); // Zero-initialized heap allocation
            ++activeAllocations;
        }
        cout << "    [CTOR]          Created Buffer ID: " << setw(4) << bufferId_ 
             << " ('" << setw(18) << left << label_ << "') | Size: " << setw(4) << capacity_ 
             << " elements | Active Allocations: " << activeAllocations << "\n";
    }

    ~DynamicBuffer() noexcept {
        if (dataBuffer_ != nullptr) {
            delete[] dataBuffer_;
            dataBuffer_ = nullptr;
            --activeAllocations;
            cout << "    [DESTRUCTOR]    Freed Buffer ID: " << setw(4) << bufferId_ 
                 << " ('" << setw(18) << left << label_ << "') | Active Allocations: " 
                 << activeAllocations << "\n";
        } else {
            cout << "    [DESTRUCTOR]    Destroyed Disarmed/Moved-From Buffer ID: " 
                 << setw(4) << bufferId_ << " (No Heap Free Needed)\n";
        }
    }

    // =================================================================================
    // 2. COPY SEMANTICS (DEEP COPYING)
    // =================================================================================

    // A. Copy Constructor (Deep Copy)
    DynamicBuffer(const DynamicBuffer& other)
        : bufferId_(other.bufferId_ + 1000), 
          capacity_(other.capacity_), 
          dataBuffer_(nullptr), 
          label_(other.label_ + "_Copy") {
        
        if (other.capacity_ > 0 && other.dataBuffer_ != nullptr) {
            dataBuffer_ = new int[capacity_];
            std::copy(other.dataBuffer_, other.dataBuffer_ + capacity_, dataBuffer_);
            ++activeAllocations;
        }

        cout << "    [COPY CTOR]     Deep-copied " << capacity_ << " elements from ID " 
             << other.bufferId_ << " into new Buffer ID " << bufferId_ << "\n";
    }

    // B. Copy Assignment Operator (Deep Copy with Exception Safety & Self-Assignment Check)
    DynamicBuffer& operator=(const DynamicBuffer& other) {
        cout << "    [COPY ASSIGN]   Executing copy assignment operator...\n";
        if (this == &other) { // Self-assignment guard: a = a
            cout << "    [SELF-ASSIGN]   Detected self-assignment (this == &other). Operation skipped.\n";
            return *this;
        }

        // Allocate new memory first before releasing old memory (Exception Safety)
        int* newBuffer = nullptr;
        if (other.capacity_ > 0 && other.dataBuffer_ != nullptr) {
            newBuffer = new int[other.capacity_];
            std::copy(other.dataBuffer_, other.dataBuffer_ + other.capacity_, newBuffer);
        }

        // Release existing resource
        if (dataBuffer_ != nullptr) {
            delete[] dataBuffer_;
            --activeAllocations;
        }

        // Acquire new state
        dataBuffer_ = newBuffer;
        capacity_ = other.capacity_;
        bufferId_ = other.bufferId_ + 2000;
        label_ = other.label_ + "_Assigned";

        if (dataBuffer_ != nullptr) {
            ++activeAllocations;
        }

        cout << "    [COPY ASSIGN]   Replaced buffer state with deep copy of ID " 
             << other.bufferId_ << " into ID " << bufferId_ << "\n";
        return *this;
    }

    // =================================================================================
    // 3. MOVE SEMANTICS (ZERO-COST RESOURCE STEALING)
    // =================================================================================

    // A. Move Constructor (Must be marked `noexcept` for STL optimizations)
    DynamicBuffer(DynamicBuffer&& other) noexcept
        : bufferId_(other.bufferId_), 
          capacity_(other.capacity_), 
          dataBuffer_(other.dataBuffer_), 
          label_(std::move(other.label_) + "_Moved") {
        
        // Disarm the source object (Leave in valid, empty state)
        other.dataBuffer_ = nullptr;
        other.capacity_ = 0;
        other.bufferId_ = -1;

        cout << "    [MOVE CTOR]     STOLE buffer pointer from source ID " 
             << bufferId_ << " (Zero heap allocations!)\n";
    }

    // B. Move Assignment Operator (`noexcept`)
    DynamicBuffer& operator=(DynamicBuffer&& other) noexcept {
        cout << "    [MOVE ASSIGN]   Executing move assignment operator...\n";
        if (this == &other) { // Self-move guard: a = std::move(a)
            cout << "    [SELF-MOVE]     Detected self-move assignment (this == &other). Operation skipped.\n";
            return *this;
        }

        // Release existing managed memory
        if (dataBuffer_ != nullptr) {
            delete[] dataBuffer_;
            --activeAllocations;
        }

        // Steal pointer and metadata from source
        dataBuffer_ = other.dataBuffer_;
        capacity_ = other.capacity_;
        bufferId_ = other.bufferId_;
        label_ = std::move(other.label_) + "_MoveAssigned";

        // Disarm source object
        other.dataBuffer_ = nullptr;
        other.capacity_ = 0;
        other.bufferId_ = -1;

        cout << "    [MOVE ASSIGN]   Transferred resource ownership to ID " << bufferId_ 
             << " and disarmed source.\n";
        return *this;
    }

    // Helper Utility Methods
    void setElement(size_t index, int value) {
        if (dataBuffer_ != nullptr && index < capacity_) {
            dataBuffer_[index] = value;
        }
    }

    [[nodiscard]] int getElement(size_t index) const {
        if (dataBuffer_ != nullptr && index < capacity_) {
            return dataBuffer_[index];
        }
        return -1;
    }

    void displayState() const {
        if (dataBuffer_ != nullptr) {
            cout << "    [BUFFER STATE]  ID: " << setw(4) << bufferId_ 
                 << " | Label: " << setw(22) << left << label_ 
                 << " | Capacity: " << setw(4) << capacity_ 
                 << " | Address: " << static_cast<const void*>(dataBuffer_) 
                 << " | First Val: " << dataBuffer_[0] << "\n";
        } else {
            cout << "    [BUFFER STATE]  ID: " << setw(4) << bufferId_ 
                 << " | Label: " << setw(22) << left << label_ 
                 << " | DISARMED (Pointer: nullptr)\n";
        }
    }

    [[nodiscard]] int getBufferId() const noexcept { return bufferId_; }
    [[nodiscard]] size_t getCapacity() const noexcept { return capacity_; }
};

// Helper Factory Function returning temporary Rvalue object
DynamicBuffer createTemporaryBuffer(int id, size_t size) {
    cout << "  - Inside `createTemporaryBuffer()` factory function...\n";
    DynamicBuffer temp(id, size, "FactoryTemp");
    temp.setElement(0, 777);
    return temp; // Triggers Move Ctor or Named Return Value Optimization (NRVO)
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Copy/Move semantics analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. DEMONSTRATING COPY SEMANTICS (DEEP COPYING)
    // =====================================================================================
    cout << "\n================ 1. DEEP COPY SEMANTICS (COPY CTOR & COPY ASSIGN) ================\n";

    {
        cout << "  - Creating primary `DynamicBuffer` object...\n";
        DynamicBuffer buf1(userInputValue, 1000, "PrimaryBuffer");
        buf1.setElement(0, 999);
        buf1.displayState();

        cout << "\n  - Case A: Invoking Copy Constructor (`DynamicBuffer buf2 = buf1;`)...\n";
        DynamicBuffer buf2 = buf1; // Copy Ctor
        buf2.displayState();

        cout << "  - Modifying copy `buf2` to prove deep copy independence...\n";
        buf2.setElement(0, 555);
        cout << "    * Original buf1[0] = " << buf1.getElement(0) << " (Unchanged)\n";
        cout << "    * Copied   buf2[0] = " << buf2.getElement(0) << " (Mutated)\n";

        cout << "\n  - Case B: Invoking Copy Assignment (`buf3 = buf1;`)...\n";
        DynamicBuffer buf3(userInputValue + 1, 50, "PreExistingBuffer");
        buf3 = buf1; // Copy Assign
        buf3.displayState();

        cout << "\n  - Case C: Testing Self-Assignment Guard (`buf1 = buf1;`)...\n";
        buf1 = buf1;

        cout << "  - Exiting Copy Semantics block scope...\n";
    } // buf1, buf2, buf3 destructed HERE

    cout << "  - Active Heap Allocations Post-Copy Scope: " << DynamicBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 2. DEMONSTRATING MOVE SEMANTICS (ZERO-COST RESOURCE STEALING)
    // =====================================================================================
    cout << "\n================ 2. MOVE SEMANTICS (MOVE CTOR & MOVE ASSIGN) ================\n";

    {
        cout << "  - Creating source `DynamicBuffer` object...\n";
        DynamicBuffer bufA(userInputValue + 10, 5000, "MovableBufferA");
        bufA.setElement(0, 1234);
        bufA.displayState();

        cout << "\n  - Case A: Invoking Move Constructor using `std::move()`...\n";
        DynamicBuffer bufB = std::move(bufA); // Move Ctor
        
        cout << "  - Inspecting states post-move:\n";
        bufB.displayState(); // Holds stolen pointer
        bufA.displayState(); // Disarmed source

        cout << "\n  - Case B: Invoking Move Assignment with temporary Rvalue from factory function...\n";
        DynamicBuffer bufC(userInputValue + 20, 10, "PreExistingC");
        bufC = createTemporaryBuffer(userInputValue + 30, 2000); // Move Assign
        bufC.displayState();

        cout << "\n  - Case C: Testing Self-Move Guard via self-reference indirection...\n";
        DynamicBuffer& selfRef = bufB;
        bufB = std::move(selfRef); // Invokes operator=(DynamicBuffer&&) safely without -Wself-move warning

        cout << "  - Exiting Move Semantics block scope...\n";
    } // bufB, bufC, disarmed bufA destructed HERE

    cout << "  - Active Heap Allocations Post-Move Scope: " << DynamicBuffer::activeAllocations << "\n";

    // =====================================================================================
    // 3. STL VECTOR REALLOCATION & `noexcept` MOVE OPTIMIZATION
    // =====================================================================================
    cout << "\n================ 3. STL CONTAINER INTEGRATION & `noexcept` EFFICIENCY ================\n";

    {
        cout << "  - Inserting `DynamicBuffer` objects into `std::vector`...\n";
        std::vector<DynamicBuffer> container;
        container.reserve(1); // Capacity 1 to force reallocation on second insertion

        cout << "  - Pushing first element into vector...\n";
        container.push_back(DynamicBuffer(userInputValue + 40, 500, "VecNode1"));

        cout << "\n  - Triggering vector reallocation (Vector uses `noexcept` move operations safely):\n";
        container.push_back(DynamicBuffer(userInputValue + 41, 600, "VecNode2"));

        for (const auto& node : container) {
            node.displayState();
        }

        cout << "  - Exiting vector scope...\n";
    }

    cout << "\n  - Final Active Heap Allocations Count: " << DynamicBuffer::activeAllocations << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ COPY VS MOVE SEMANTICS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Metric      | Copy Semantics                    | Move Semantics                    |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Target Parameter      | `const Type&` (Lvalue Reference)  | `Type&&` (Rvalue Reference)       |\n"
         << "| Memory Strategy       | Allocates new heap memory block   | Steals existing memory pointer    |\n"
         << "| Execution Cost        | O(N) allocation & data copies     | O(1) constant-time pointer swap   |\n"
         << "| Source Object State   | Remains unmodified (Intact)       | Disarmed (Valid empty/null state) |\n"
         << "| STL Vector Efficiency | Requires deep element duplication | Uses fast move if marked `noexcept|\n"
         << "| Primary Use Case      | Persistent state replication      | Temporary transfers / factories   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}