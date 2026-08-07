/*
 * =====================================================================================
 * CONCEPT        : Classes and Objects in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the core fundamentals of Classes and Objects in C++:
 *
 *                  1. Class vs. Object Blueprint : Defining user-defined types (Classes) and 
 *                                                  instantiating concrete instances (Objects).
 *                  2. Encapsulation & Access     : Restricting direct attribute access via `private`
 *                                                  access specifiers and exposing controlled `public`
 *                                                  getter/setter interfaces with invariant guards.
 *                  3. Object Lifecycle & Ctors   : Default, Parameterized, Delegating, Copy, and Move
 *                                                  Constructors, alongside deterministic Destructors.
 *                  4. The `this` Pointer         : Explicit self-referencing for disambiguation and
 *                                                  method chaining.
 *                  5. Const-Correctness          : Const objects and const member functions preventing
 *                                                  unintended object state mutations.
 *                  6. Static Class Members       : Class-wide attributes and functions shared across
 *                                                  all instances (`inline static`).
 *                  7. Allocation Lifetimes       : Stack-bound objects vs. Dynamic Heap objects managed
 *                                                  via Modern C++ Smart Pointers (`std::unique_ptr`).
 *
 * TIME COMPLEXITY  : Member Access / Method Invocation : O(1) constant time.
 * SPACE COMPLEXITY : Stack Object Footprint            : Sum of non-static data member sizes (+ alignment padding).
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <stdexcept>
#include <iomanip>
#include <limits>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: SmartDevice
// Serves as the blueprint defining attributes (state) and methods (behavior).
// =====================================================================================
class SmartDevice {
private:
    // Encapsulated Member Variables (State)
    int deviceId_;
    string deviceName_;
    double powerRatingWatts_;
    bool isPoweredOn_;

    // Static Data Member: Shared across ALL instances of SmartDevice
    static inline int totalDeviceCount = 0; // C++17 inline static

public:
    // =================================================================================
    // 1. CONSTRUCTORS & DESTRUCTOR (LIFECYCLE MANAGEMENT)
    // =================================================================================

    // A. Default Constructor (Delegates to Parameterized Constructor)
    SmartDevice() : SmartDevice(0, "Unnamed_Device", 10.0) {
        cout << "    [DEFAULT CTOR] Delegated initialization for default SmartDevice.\n";
    }

    // B. Parameterized Constructor with Invariant Enforcement
    SmartDevice(int id, string name, double powerRating)
        : deviceId_(id), deviceName_(std::move(name)), powerRatingWatts_(powerRating), isPoweredOn_(false) {
        if (id < 0) {
            throw invalid_argument("Device ID cannot be negative!");
        }
        if (powerRating <= 0.0) {
            throw invalid_argument("Power rating must be strictly positive!");
        }
        ++totalDeviceCount;
        cout << "    [PARAM CTOR]   Created SmartDevice ID: " << deviceId_ 
             << " ('" << deviceName_ << "') | Total Active Devices: " << totalDeviceCount << "\n";
    }

    // C. Destructor
    ~SmartDevice() noexcept {
        --totalDeviceCount;
        cout << "    [DESTRUCTOR]   Destroyed SmartDevice ID: " << deviceId_ 
             << " ('" << deviceName_ << "') | Total Active Devices: " << totalDeviceCount << "\n";
    }

    // D. Copy Constructor (Deep Copy)
    SmartDevice(const SmartDevice& other)
        : deviceId_(other.deviceId_ + 1000), 
          deviceName_(other.deviceName_ + "_Copy"), 
          powerRatingWatts_(other.powerRatingWatts_), 
          isPoweredOn_(other.isPoweredOn_) {
        ++totalDeviceCount;
        cout << "    [COPY CTOR]    Cloned device ID " << other.deviceId_ 
             << " into new device ID " << deviceId_ << "\n";
    }

    // E. Copy Assignment Operator
    SmartDevice& operator=(const SmartDevice& other) {
        if (this != &other) { // Self-assignment check
            deviceId_ = other.deviceId_ + 2000;
            deviceName_ = other.deviceName_ + "_Assigned";
            powerRatingWatts_ = other.powerRatingWatts_;
            isPoweredOn_ = other.isPoweredOn_;
            cout << "    [COPY ASSIGN]  Assigned state from device ID " << other.deviceId_ 
                 << " to device ID " << deviceId_ << "\n";
        }
        return *this; // Return reference for assignment chaining
    }

    // F. Move Constructor (Resource Transfer)
    SmartDevice(SmartDevice&& other) noexcept
        : deviceId_(other.deviceId_), 
          deviceName_(std::move(other.deviceName_) + "_Moved"), 
          powerRatingWatts_(other.powerRatingWatts_), 
          isPoweredOn_(other.isPoweredOn_) {
        other.deviceId_ = -1;
        other.isPoweredOn_ = false;
        cout << "    [MOVE CTOR]    Transferred ownership from source device to ID " << deviceId_ << "\n";
    }

    // G. Move Assignment Operator
    SmartDevice& operator=(SmartDevice&& other) noexcept {
        if (this != &other) {
            deviceId_ = other.deviceId_;
            deviceName_ = std::move(other.deviceName_) + "_MoveAssigned";
            powerRatingWatts_ = other.powerRatingWatts_;
            isPoweredOn_ = other.isPoweredOn_;

            other.deviceId_ = -1;
            other.isPoweredOn_ = false;
            cout << "    [MOVE ASSIGN]  Move-assigned device state into ID " << deviceId_ << "\n";
        }
        return *this;
    }

    // =================================================================================
    // 2. ENCAPSULATION & METHOD INTERFACE (BEHAVIOR)
    // =================================================================================

    // Method Chaining via `this` Pointer
    SmartDevice& setPowerOn(bool state) {
        this->isPoweredOn_ = state;
        cout << "    [POWER STATE]  Device ID " << this->deviceId_ 
             << " is now " << (this->isPoweredOn_ ? "ON" : "OFF") << "\n";
        return *this; // Return *this enables method chaining: dev.setPowerOn(true).updateName(...)
    }

    SmartDevice& setPowerRating(double watts) {
        if (watts <= 0.0) {
            throw invalid_argument("Power rating must be positive!");
        }
        this->powerRatingWatts_ = watts;
        return *this;
    }

    // Const Member Functions (Guaranteed NOT to modify object state)
    [[nodiscard]] int getDeviceId() const noexcept { return deviceId_; }
    [[nodiscard]] const string& getDeviceName() const noexcept { return deviceName_; }
    [[nodiscard]] double getPowerRatingWatts() const noexcept { return powerRatingWatts_; }
    [[nodiscard]] bool isPoweredOn() const noexcept { return isPoweredOn_; }

    void displayStatus() const {
        cout << "    [DEVICE STATUS] ID: " << setw(4) << deviceId_ 
             << " | Name: " << setw(20) << left << deviceName_ 
             << " | Power Rating: " << setw(6) << fixed << setprecision(1) << powerRatingWatts_ << " W"
             << " | State: " << (isPoweredOn_ ? "ONLINE " : "OFFLINE") << "\n";
    }

    // Static Member Function: Operates on class-level data without needing an instance
    static int getTotalDeviceCount() noexcept {
        return totalDeviceCount;
    }

    // Friend Function Operator Overload for Output Stream Operations
    friend ostream& operator<<(ostream& os, const SmartDevice& device) {
        os << "SmartDevice[ID=" << device.deviceId_ << ", Name='" << device.deviceName_ 
           << "', Power=" << device.powerRatingWatts_ << "W, On=" << (device.isPoweredOn_ ? "true" : "false") << "]";
        return os;
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Class/Object analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. STACK INSTANTIATION & ENCAPSULATION DEMONSTRATION
    // =====================================================================================
    cout << "\n================ 1. OBJECT INSTANTIATION & ENCAPSULATION ================\n";

    {
        cout << "  - Creating stack-allocated objects...\n";
        SmartDevice devA(userInputValue, "LivingRoom_Light", 15.5);
        SmartDevice devB(userInputValue + 1, "Kitchen_Thermostat", 45.0);

        cout << "\n  - Demonstrating Method Chaining using `this` pointer...\n";
        devA.setPowerOn(true).setPowerRating(18.0);

        cout << "\n  - Reading encapsulated state via const getters...\n";
        devA.displayStatus();
        devB.displayStatus();

        cout << "  - Exiting local scope for stack objects...\n";
    } // devB and devA destructed automatically HERE in reverse LIFO order

    cout << "  - Active Instances Post-Scope: " << SmartDevice::getTotalDeviceCount() << "\n";

    // =====================================================================================
    // 2. COPY & MOVE LIFECYCLE SEMANTICS
    // =====================================================================================
    cout << "\n================ 2. COPY & MOVE SEMANTICS ================\n";

    {
        cout << "  - Instantiating primary object...\n";
        SmartDevice primary(userInputValue + 10, "Master_Router", 25.0);

        cout << "\n  - Invoking Copy Constructor...\n";
        SmartDevice copyObj = primary; // Copy Ctor
        copyObj.displayStatus();

        cout << "\n  - Invoking Move Constructor...\n";
        SmartDevice movedObj = std::move(primary); // Move Ctor
        movedObj.displayStatus();

        cout << "  - Inspecting moved-from source object state:\n";
        primary.displayStatus(); // Left in valid disarmed state

        cout << "  - Exiting copy/move scope...\n";
    }

    cout << "  - Active Instances Post Copy/Move Scope: " << SmartDevice::getTotalDeviceCount() << "\n";

    // =====================================================================================
    // 3. CONST OBJECTS & CONST MEMBER FUNCTIONS
    // =====================================================================================
    cout << "\n================ 3. CONST OBJECTS & CONST-CORRECTNESS ================\n";

    {
        cout << "  - Creating a `const` SmartDevice object...\n";
        const SmartDevice constDev(userInputValue + 20, "Security_Camera", 12.0);

        // constDev.setPowerOn(true); // COMPILE ERROR: Cannot invoke non-const method on const object!
        cout << "  - Calling const member functions on const object:\n";
        cout << "    Device Name   : " << constDev.getDeviceName() << "\n";
        cout << "    Power Rating  : " << constDev.getPowerRatingWatts() << " W\n";
        constDev.displayStatus();
    }

    // =====================================================================================
    // 4. DYNAMIC HEAP INSTANTIATION WITH SMART POINTERS
    // =====================================================================================
    cout << "\n================ 4. DYNAMIC HEAP ALLOCATION (`std::unique_ptr`) ================\n";

    {
        cout << "  - Instantiating heap object managed by `std::unique_ptr`...\n";
        auto heapDev = std::make_unique<SmartDevice>(userInputValue + 30, "Smart_Doorlock", 8.5);

        heapDev->setPowerOn(true);
        cout << "  - Stream Overload Output: " << *heapDev << "\n";

        cout << "  - Leaving smart pointer scope...\n";
    } // Heap object automatically freed HERE via RAII!

    // =====================================================================================
    // 5. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 5. INVARIANT ENFORCEMENT ================\n";

    try {
        cout << "  - Attempting to instantiate device with invalid power rating (-50.0 W)...\n";
        SmartDevice invalidDev(userInputValue + 40, "Faulty_Device", -50.0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    cout << "\n  - Final Active Device Count: " << SmartDevice::getTotalDeviceCount() << "\n";

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ CLASSES AND OBJECTS SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Concept / Feature     | Technical Implementation          | Primary Architectural Role        |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Class Blueprint       | `class SmartDevice { ... };`       | Defines custom user-defined type  |\n"
         << "| Object Instance       | `SmartDevice dev(1, \"Light\", 10);`| Memory allocation of class state  |\n"
         << "| Data Encapsulation    | `private` fields + Public getters | Protects internal state invariants|\n"
         << "| Lifecycle Ctors/Dtor  | Ctors, `~SmartDevice()`, Rule of 5| Deterministic creation/teardown   |\n"
         << "| Method Chaining       | `return *this;` via `this` ptr   | Allows fluid caller interface syntax|\n"
         << "| Const-Correctness     | `void display() const;`           | Prevents accidental state mutation|\n"
         << "| Shared Static State   | `inline static int count;`        | Tracks class-wide data across objs|\n"
         << "| Dynamic Heap Object   | `std::make_unique<SmartDevice>()` | Managed lifecycle dynamic allocation|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}