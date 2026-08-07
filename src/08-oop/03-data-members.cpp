/*
 * =====================================================================================
 * CONCEPT        : Data Members in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the taxonomy, memory layout, storage durations, and access rules
 *                  of Data Members in Modern C++:
 *
 *                  1. Instance Data Members       : Regular non-static members defining the state
 *                                                   of individual objects.
 *                  2. In-Class Member Initializers: Default initializers provided directly at
 *                                                   member declaration site.
 *                  3. Access Control Specifiers   : `private`, `protected`, and `public` data members.
 *                  4. Static Data Members         : Shared class-wide storage (`inline static` in C++17).
 *                  5. Const & Reference Members   : Immutable data members and non-reseatable references
 *                                                   (requiring constructor member initializer lists).
 *                  6. Mutable Data Members        : `mutable` fields inspectable/modifiable inside
 *                                                   `const` member functions (e.g., caches, locks).
 *                  7. Memory Layout & Bit-Fields  : Struct alignment, padding, and bit-level packing.
 *
 * TIME COMPLEXITY  : Member Access / Modifiers : O(1) constant time.
 * SPACE COMPLEXITY : Object Footprint         : Sum of non-static member sizes (+ alignment padding).
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <utility>
#include <iomanip>
#include <limits>
#include <cstdint>

using namespace std;

// =====================================================================================
// 1. ACCESS CONTROL & INHERITANCE OF DATA MEMBERS
// Demonstrates public, protected, and private data member visibility.
// =====================================================================================
class BaseEntity {
public:
    int publicId_; // Accessible anywhere

protected:
    string protectedTag_; // Accessible within BaseEntity and derived classes

private:
    double privateSecretKey_; // Accessible ONLY within BaseEntity

public:
    BaseEntity(int id, string tag, double secret)
        : publicId_(id), protectedTag_(std::move(tag)), privateSecretKey_(secret) {
        cout << "    [BaseEntity Ctor] Initialized publicId=" << publicId_ 
             << ", protectedTag='" << protectedTag_ << "'\n";
    }

    void displayBaseSecrets() const {
        cout << "    [BaseEntity Access] Private Secret Key: " << privateSecretKey_ << "\n";
    }
};

class DerivedSensor : public BaseEntity {
private:
    double sensorReading_;

public:
    DerivedSensor(int id, string tag, double secret, double reading)
        : BaseEntity(id, std::move(tag), secret), sensorReading_(reading) {}

    void inspectInheritedMembers() const {
        cout << "    [DerivedSensor Access] Public ID: " << publicId_ << "\n";
        cout << "    [DerivedSensor Access] Protected Tag: " << protectedTag_ << "\n";
        // cout << privateSecretKey_; // COMPILE ERROR: Cannot access private base member!
        cout << "    [DerivedSensor Access] Private Reading: " << sensorReading_ << "\n";
    }
};

// =====================================================================================
// 2. MODERN DATA MEMBER TYPES
// In-class Initializers, Const, Mutable, Reference, and Inline Static Data Members.
// =====================================================================================
class AdvancedDataMemberDemo {
private:
    // In-class Member Initializer (Default value if constructor doesn't provide one)
    int priorityLevel_{5};
    string componentName_{"UnassignedComponent"};

    // Const Member: Immutable after initialization (MUST be set in initializer list)
    const int creationTimestamp_;

    // Reference Member: Must be bound in constructor initializer list (Non-reseatable)
    const int& boundReferenceId_;

    // Mutable Member: Can be modified even inside `const` member functions
    mutable int accessCounter_{0};
    mutable double cachedResult_{0.0};
    mutable bool isCacheValid_{false};

    // Shared Static Member (C++17 inline static allows definition directly in header)
    static inline int totalActiveInstances = 0;

public:
    // Constructor utilizing Member Initializer List
    AdvancedDataMemberDemo(int timestamp, const int& refId, string name = "")
        : creationTimestamp_(timestamp), boundReferenceId_(refId) {
        if (!name.empty()) {
            componentName_ = std::move(name);
        }
        ++totalActiveInstances;
        cout << "    [Advanced Ctor] Component: '" << componentName_ 
             << "' | Timestamp: " << creationTimestamp_ 
             << " | Ref ID: " << boundReferenceId_ 
             << " | Active Instances: " << totalActiveInstances << "\n";
    }

    ~AdvancedDataMemberDemo() noexcept {
        --totalActiveInstances;
        cout << "    [Advanced Dtor] Component: '" << componentName_ 
             << "' destroyed | Remaining Active: " << totalActiveInstances << "\n";
    }

    // Const Member Function mutating `mutable` data members
    double getComputedData(double factor) const {
        ++accessCounter_; // Modifying mutable member in const function!
        
        if (!isCacheValid_) {
            cout << "    [CONST METHOD - CACHE MISS] Computing expensive result...\n";
            cachedResult_ = factor * 42.0;
            isCacheValid_ = true;
        } else {
            cout << "    [CONST METHOD - CACHE HIT] Returning cached result...\n";
        }

        cout << "    [MUTABLE METRICS] Total Const Accesses: " << accessCounter_ << "\n";
        return cachedResult_;
    }

    void invalidateCache() const {
        isCacheValid_ = false;
        cout << "    [MUTABLE STATE] Cache invalidated via const method.\n";
    }

    // Static Member Accessor
    static int getTotalActiveInstances() noexcept {
        return totalActiveInstances;
    }

    // Inspection Interface
    void displayMemberDetails() const {
        cout << "    [MEMBER STATE] Component: " << setw(20) << left << componentName_
             << " | Priority: " << priorityLevel_
             << " | Const Timestamp: " << creationTimestamp_
             << " | Bound Ref ID: " << boundReferenceId_ << "\n";
    }
};

// =====================================================================================
// 3. BIT-FIELDS & MEMORY LAYOUT/ALIGNMENT
// Demonstrates packing data members into bit-level structures to save memory.
// =====================================================================================
struct UnpackedHardwareFlags {
    bool isReady;       // 1 byte (+ padding)
    bool hasError;      // 1 byte (+ padding)
    uint32_t errorCode; // 4 bytes
    bool isActive;      // 1 byte (+ padding)
};

struct BitFieldHardwareFlags {
    // Packed into a single 32-bit integer block
    uint32_t isReady : 1;      // 1 bit
    uint32_t hasError : 1;     // 1 bit
    uint32_t isActive : 1;     // 1 bit
    uint32_t mode : 5;         // 5 bits (0-31)
    uint32_t errorCode : 24;   // 24 bits
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Data Member analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. ACCESS CONTROL & INHERITANCE DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. ACCESS CONTROL & INHERITANCE ================\n";

    {
        cout << "  - Instantiating DerivedSensor object...\n";
        DerivedSensor sensor(userInputValue, "TelemetrySensor_A", 98765.43, 23.5);

        sensor.displayBaseSecrets();
        sensor.inspectInheritedMembers();

        cout << "  - Direct Access to Public Member: sensor.publicId_ = " << sensor.publicId_ << "\n";
    }

    // =====================================================================================
    // 2. CONST, MUTABLE, REFERENCE, & INLINE STATIC DATA MEMBERS
    // =====================================================================================
    cout << "\n================ 2. ADVANCED DATA MEMBER TYPES ================\n";

    {
        int externalReferenceTarget = 9999;

        cout << "  - Instantiating AdvancedDataMemberDemo instances...\n";
        AdvancedDataMemberDemo compA(userInputValue + 10, externalReferenceTarget, "GraphicsEngine");
        AdvancedDataMemberDemo compB(userInputValue + 11, externalReferenceTarget, "AudioEngine");

        compA.displayMemberDetails();
        compB.displayMemberDetails();

        cout << "\n  - Demonstrating `mutable` data member mutation inside `const` method:\n";
        const AdvancedDataMemberDemo constComp(userInputValue + 20, externalReferenceTarget, "PhysicsEngine");
        
        // Invoking const method multiple times
        double r1 = constComp.getComputedData(2.0);
        double r2 = constComp.getComputedData(2.0); // Cache Hit
        cout << "    Computed Result: " << r1 << " (Cached: " << r2 << ")\n";

        constComp.invalidateCache();
        double r3 = constComp.getComputedData(2.0); // Recomputed
        cout << "    Recomputed Result: " << r3 << "\n";

        cout << "\n  - External reference target value mutation check:\n";
        cout << "    Original External Ref Target: " << externalReferenceTarget << "\n";
        externalReferenceTarget = 7777; // Modifies target bound by reference members!
        cout << "    Mutated External Ref Target : " << externalReferenceTarget << "\n";
        constComp.displayMemberDetails();

        cout << "\n  - Static Data Member Query: Total Active = " 
             << AdvancedDataMemberDemo::getTotalActiveInstances() << "\n";
        cout << "  - Leaving Advanced Scope...\n";
    }

    cout << "  - Active Instances Post-Scope: " << AdvancedDataMemberDemo::getTotalActiveInstances() << "\n";

    // =====================================================================================
    // 3. BIT-FIELDS & MEMORY LAYOUT / PADDING
    // =====================================================================================
    cout << "\n================ 3. MEMORY LAYOUT & BIT-FIELDS ================\n";

    cout << "  - Memory Footprint Comparison:\n";
    cout << "    * sizeof(UnpackedHardwareFlags)  : " << sizeof(UnpackedHardwareFlags) << " bytes\n";
    cout << "    * sizeof(BitFieldHardwareFlags)  : " << sizeof(BitFieldHardwareFlags) << " bytes\n";

    BitFieldHardwareFlags flags{};
    flags.isReady = 1;
    flags.hasError = 0;
    flags.isActive = 1;
    flags.mode = 15;        // Fits in 5 bits
    flags.errorCode = 1042; // Fits in 24 bits

    cout << "\n  - Bit-Field State Extraction:\n";
    cout << "    * isReady   : " << flags.isReady << "\n";
    cout << "    * hasError  : " << flags.hasError << "\n";
    cout << "    * isActive  : " << flags.isActive << "\n";
    cout << "    * Mode      : " << flags.mode << "\n";
    cout << "    * ErrorCode : " << flags.errorCode << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ DATA MEMBERS IN OOP SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Data Member Category  | Declaration Syntax / Keyword      | Primary Behavioral Characteristic |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Instance Member       | `int value_;`                     | Unique per-object state storage   |\n"
         << "| In-Class Initializer  | `int value_{10};`                 | Default fallback value if uninit  |\n"
         << "| Public Member         | `public: int id_;`                | Accessible everywhere             |\n"
         << "| Protected Member      | `protected: string tag_;`         | Accessible in Base & Derived      |\n"
         << "| Private Member        | `private: double secret_;`        | Enforces strict encapsulation     |\n"
         << "| Static Member         | `inline static int count;`        | Single shared state across all    |\n"
         << "| Const Member          | `const int timestamp_;`           | Immutable after object ctor       |\n"
         << "| Reference Member      | `const int& ref_;`                | Non-reseatable reference binding  |\n"
         << "| Mutable Member        | `mutable int cache_;`             | Modifiable inside const methods   |\n"
         << "| Bit-Field Member      | `uint32_t mode : 5;`              | Packs variables at bit-level width|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}