/*
 * =====================================================================================
 * CONCEPT        : Copying RAII Objects in Modern C++
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the paradigms, mechanics, and design choices when copying RAII objects:
 *
 *                  1. The Shallow Copy Hazard   : Default compiler-generated memberwise copies 
 *                                                 cause duplicate pointer addresses, leading to 
 *                                                 double-free crashes and Use-After-Free (UAF).
 *                  2. Non-Copyable RAII (Deleted): Explicitly suppressing copy operations 
 *                                                 (`= delete`) to enforce single/exclusive 
 *                                                 ownership semantics (e.g., `std::unique_ptr`).
 *                  3. Deep Copying (Value)       : Clones the underlying resource so each RAII 
 *                                                 object manages its own independent memory allocation.
 *                  4. Shared Ownership (Ref-Count): Shares the underlying resource across multiple 
 *                                                 RAII objects using a reference count (e.g., `std::shared_ptr`).
 *                  5. Copy-and-Swap Idiom        : The canonical C++ idiom for implementing strong exception-safe 
 *                                                 copy assignment operators.
 *
 * TIME COMPLEXITY  : Non-Copyable / Shared Copy: O(1) constant time.
 *                    Deep Copy                 : O(N) linear time based on resource size.
 * SPACE COMPLEXITY : Deep Copy                 : O(N) auxiliary payload allocation.
 *                    Shared Copy               : O(1) ref-count overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>
#include <algorithm>

using namespace std;

// =====================================================================================
// HELPER CLASS 1: MANAGED RESOURCE WITH ACTIVE INSTANCE TRACKING
// Logs resource lifetime events to trace cloning and deallocation sequences.
// =====================================================================================
class ResourcePayload {
private:
    int id_;
    string data_;

public:
    static inline int activeInstances = 0; // C++17 inline static variable

    ResourcePayload(int id, string data) : id_(id), data_(std::move(data)) {
        ++activeInstances;
        cout << "    [RESOURCE CREATED]   ID: " << setw(3) << id_ << " (" << setw(18) << left << data_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    ~ResourcePayload() noexcept {
        --activeInstances;
        cout << "    [RESOURCE DESTROYED] ID: " << setw(3) << id_ << " (" << setw(18) << left << data_ 
             << ") at " << static_cast<const void*>(this) 
             << " | Active: " << activeInstances << "\n";
    }

    void execute() const {
        cout << "    [PAYLOAD EXECUTED]   Resource ID: " << id_ << " (" << data_ << ") processing.\n";
    }

    [[nodiscard]] int getId() const { return id_; }
    [[nodiscard]] const string& getData() const { return data_; }
    void setData(string newData) { data_ = std::move(newData); }
};

// =====================================================================================
// STRATEGY 1: NON-COPYABLE RAII WRAPPER (EXCLUSIVE OWNERSHIP)
// Disables copy ctor and copy assignment to guarantee single owner invariants.
// =====================================================================================
class NonCopyableRaii {
private:
    ResourcePayload* payload_;

public:
    explicit NonCopyableRaii(ResourcePayload* res = nullptr) : payload_(res) {
        cout << "    [NonCopyable Ctor] Bound resource at: " << static_cast<const void*>(payload_) << "\n";
    }

    ~NonCopyableRaii() noexcept {
        cout << "    [NonCopyable Dtor] Deallocating resource at: " << static_cast<const void*>(payload_) << "\n";
        delete payload_;
    }

    // EXPLICITLY DELETE COPY OPERATIONS
    NonCopyableRaii(const NonCopyableRaii&) = delete;
    NonCopyableRaii& operator=(const NonCopyableRaii&) = delete;

    // ALLOW MOVE OPERATIONS
    NonCopyableRaii(NonCopyableRaii&& other) noexcept : payload_(other.payload_) {
        other.payload_ = nullptr;
        cout << "    [NonCopyable Move Ctor] Resource ownership transferred.\n";
    }

    NonCopyableRaii& operator=(NonCopyableRaii&& other) noexcept {
        if (this != &other) {
            delete payload_;
            payload_ = other.payload_;
            other.payload_ = nullptr;
            cout << "    [NonCopyable Move Assign] Previous freed, ownership transferred.\n";
        }
        return *this;
    }

    [[nodiscard]] ResourcePayload* get() const { return payload_; }
};

// =====================================================================================
// STRATEGY 2: DEEP-COPYING RAII WRAPPER (VALUE SEMANTICS)
// Allocates an independent resource clone upon copying using Copy-and-Swap idiom.
// =====================================================================================
class DeepCopyRaii {
private:
    ResourcePayload* payload_;

public:
    explicit DeepCopyRaii(int id, const string& data)
        : payload_(new ResourcePayload(id, data)) {
        cout << "    [DeepCopy Ctor] Primary resource created at: " << static_cast<const void*>(payload_) << "\n";
    }

    ~DeepCopyRaii() noexcept {
        cout << "    [DeepCopy Dtor] Freeing owned payload at: " << static_cast<const void*>(payload_) << "\n";
        delete payload_;
    }

    // DEEP COPY CONSTRUCTOR: Allocates a new independent copy of the resource
    DeepCopyRaii(const DeepCopyRaii& other)
        : payload_(other.payload_ ? new ResourcePayload(other.payload_->getId() + 1000, 
                                                        other.payload_->getData() + "_DeepClone") 
                                 : nullptr) {
        cout << "    [DeepCopy Copy Ctor] Cloned resource from " 
             << static_cast<const void*>(other.payload_) << " to new address: " 
             << static_cast<const void*>(payload_) << "\n";
    }

    // COPY-AND-SWAP IDIOM: Provides Strong Exception Safety
    friend void swap(DeepCopyRaii& first, DeepCopyRaii& second) noexcept {
        using std::swap;
        swap(first.payload_, second.payload_);
    }

    DeepCopyRaii& operator=(DeepCopyRaii other) noexcept { // Pass-by-value triggers Copy Ctor
        cout << "    [DeepCopy Copy Assign] Executing Copy-and-Swap assignment...\n";
        swap(*this, other);
        return *this;
    } // Temporary `other` destructs automatically HERE, freeing old resource

    // MOVE CONSTRUCTOR & ASSIGNMENT
    DeepCopyRaii(DeepCopyRaii&& other) noexcept : payload_(other.payload_) {
        other.payload_ = nullptr;
        cout << "    [DeepCopy Move Ctor] Dynamic resource moved.\n";
    }

    [[nodiscard]] ResourcePayload* get() const { return payload_; }
};

// =====================================================================================
// STRATEGY 3: SHARED REF-COUNTED RAII WRAPPER (SHARED OWNERSHIP)
// Shares the same underlying resource and increments a dynamic reference counter.
// =====================================================================================
class SharedRefCountedRaii {
private:
    ResourcePayload* payload_;
    int* refCount_;

    void release() noexcept {
        if (refCount_) {
            --(*refCount_);
            cout << "    [Shared Release] Decremented Ref Count to: " << *refCount_ 
                 << " for address " << static_cast<const void*>(payload_) << "\n";
            if (*refCount_ == 0) {
                cout << "    [Shared Release] Ref Count reached 0! Deallocating shared resource...\n";
                delete payload_;
                delete refCount_;
                payload_ = nullptr;
                refCount_ = nullptr;
            }
        }
    }

public:
    explicit SharedRefCountedRaii(int id, const string& data)
        : payload_(new ResourcePayload(id, data)), refCount_(new int(1)) {
        cout << "    [Shared Ctor] Primary resource created with initial Ref Count = 1\n";
    }

    ~SharedRefCountedRaii() noexcept {
        release();
    }

    // SHARED COPY CONSTRUCTOR: Shares ownership and increments reference count
    SharedRefCountedRaii(const SharedRefCountedRaii& other) noexcept
        : payload_(other.payload_), refCount_(other.refCount_) {
        if (refCount_) {
            ++(*refCount_);
            cout << "    [Shared Copy Ctor] Incremented Ref Count to: " << *refCount_ 
                 << " for shared address: " << static_cast<const void*>(payload_) << "\n";
        }
    }

    // SHARED COPY ASSIGNMENT
    SharedRefCountedRaii& operator=(const SharedRefCountedRaii& other) noexcept {
        if (this != &other) {
            release(); // Release current reference first
            payload_ = other.payload_;
            refCount_ = other.refCount_;
            if (refCount_) {
                ++(*refCount_);
                cout << "    [Shared Copy Assign] Incremented Ref Count to: " << *refCount_ << "\n";
            }
        }
        return *this;
    }

    [[nodiscard]] int getRefCount() const noexcept { return refCount_ ? *refCount_ : 0; }
    [[nodiscard]] ResourcePayload* get() const noexcept { return payload_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for copying RAII objects analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. THE SHALLOW COPY HAZARD (EXPLANATION)
    // =====================================================================================
    cout << "\n================ 1. THE SHALLOW COPY HAZARD (EXPLAINED) ================\n";
    cout << "  - Default C++ compiler-generated copy constructors perform memberwise copying.\n";
    cout << "  - For raw pointers (`T*`), this copies the ADDRESS, NOT the underlying resource.\n";
    cout << "  - Consequence: Two RAII objects hold the exact same raw pointer address.\n";
    cout << "  - When both objects exit scope, both call `delete ptr` -> DOUBLE-FREE CRASH!\n";

    // =====================================================================================
    // 2. STRATEGY 1: NON-COPYABLE RAII (`= delete`)
    // =====================================================================================
    cout << "\n================ 2. STRATEGY 1: NON-COPYABLE RAII (`= delete`) ================\n";

    {
        cout << "  - Instantiating non-copyable RAII handle...\n";
        NonCopyableRaii exclusiveObj(new ResourcePayload(userInputValue, "ExclusiveData"));

        // NonCopyableRaii copyObj = exclusiveObj; // COMPILE ERROR: Copy Ctor is deleted!
        cout << "  - [SAFETY GUARANTEE]: Copy attempts are blocked at compile-time by `= delete`.\n";

        cout << "  - Moving ownership via Move Ctor...\n";
        NonCopyableRaii movedObj = std::move(exclusiveObj);

        cout << "  - Exiting non-copyable scope...\n";
    }

    cout << "  - Active Instances Post Non-Copyable Scope: " << ResourcePayload::activeInstances << "\n";

    // =====================================================================================
    // 3. STRATEGY 2: DEEP COPYING (VALUE SEMANTICS)
    // =====================================================================================
    cout << "\n================ 3. STRATEGY 2: DEEP COPYING (VALUE SEMANTICS) ================\n";

    {
        cout << "  - Instantiating primary `DeepCopyRaii` object `original`...\n";
        DeepCopyRaii original(userInputValue + 10, "OriginalBuffer");

        cout << "\n  - Copy-constructing `cloned` object from `original` (Triggers Deep Copy)...\n";
        DeepCopyRaii cloned = original; // Deep copy constructor invoked

        cout << "  - Original Payload Addr: " << static_cast<const void*>(original.get()) 
             << " | Data: " << original.get()->getData() << "\n";
        cout << "  - Cloned Payload Addr  : " << static_cast<const void*>(cloned.get()) 
             << " | Data: " << cloned.get()->getData() << "\n";

        cout << "\n  - Modifying cloned object's payload data...\n";
        cloned.get()->setData("ClonedBuffer_Modified");

        cout << "  - Original Data after Clone Mutation: " << original.get()->getData() << "\n";
        cout << "  - Cloned Data after Mutation         : " << cloned.get()->getData() << "\n";
        cout << "  - [INDEPENDENCE GUARANTEE]: Deep copies are completely isolated in memory.\n";

        cout << "\n  - Exiting deep copy scope...\n";
    } // Both independent dynamic resources freed cleanly without double-free errors!

    cout << "  - Active Instances Post Deep Copy Scope: " << ResourcePayload::activeInstances << "\n";

    // =====================================================================================
    // 4. STRATEGY 3: SHARED REF-COUNTED COPYING (SHARED OWNERSHIP)
    // =====================================================================================
    cout << "\n================ 4. STRATEGY 3: SHARED REF-COUNTED COPYING ================\n";

    {
        cout << "  - Instantiating primary `SharedRefCountedRaii` object `shared1`...\n";
        SharedRefCountedRaii shared1(userInputValue + 20, "SharedData");
        cout << "    * Current Ref Count: " << shared1.getRefCount() << "\n";

        {
            cout << "\n  - Entering inner scope... Copy-constructing `shared2` from `shared1`...\n";
            SharedRefCountedRaii shared2 = shared1;
            cout << "    * Current Ref Count: " << shared1.getRefCount() << "\n";

            {
                cout << "\n  - Entering deeper scope... Copy-assigning `shared3 = shared2`...\n";
                SharedRefCountedRaii shared3(999, "TempDummy");
                shared3 = shared2;
                cout << "    * Current Ref Count: " << shared1.getRefCount() << "\n";
                cout << "  - Exiting deeper scope...\n";
            }

            cout << "  - Exited deeper scope. Ref Count decremented to: " << shared1.getRefCount() << "\n";
            cout << "  - Exiting inner scope...\n";
        }

        cout << "  - Exited inner scope. Ref Count decremented to: " << shared1.getRefCount() << "\n";
        cout << "  - [SHARED GUARANTEE]: Resource remains alive as long as Ref Count > 0.\n";

        cout << "  - Exiting outer shared scope...\n";
    } // Ref count reaches 0; underlying resource deleted cleanly!

    cout << "  - Final Active Resource Instances: " << ResourcePayload::activeInstances << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ COPYING RAII OBJECTS SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Copying Paradigm      | Copy Syntax / Rule| Memory Behavior   | Primary Operational Characteristics|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Shallow Copy (Hazard) | Default Ctor/Assign| Duplicate Pointer | FATAL: Double-free crash on exit  |\n"
         << "| Non-Copyable RAII     | `= delete`        | Single Owner      | Compile-time error on copy attempt|\n"
         << "| Deep Copying (Value)  | Custom `new Alloc`| Isolated Cloning  | Each object owns separate memory  |\n"
         << "| Shared Ref-Counted    | `refCount++`      | Shared Pointer    | Ref-counted; freed when count == 0|\n"
         << "| Copy-and-Swap Idiom   | `operator=(T val)`| Atomic State Swap | Provides Strong Exception Safety  |\n"
         << "| Standard Library Types| `unique_ptr`      | Non-Copyable      | Move-only exclusive ownership     |\n"
         << "| Standard Library Types| `shared_ptr`      | Shared Ownership  | Thread-safe reference counting    |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}