/*
 * =====================================================================================
 * CONCEPT        : Object Relationships in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the 4 fundamental object relationships in OOP:
 *
 *                  1. Composition ("Has-a" Strong Ownership) :
 *                     - Tight structural coupling where the parent owns the child lifetime.
 *                     - Child cannot exist without the parent.
 *                     - Implementation: Value objects or `std::unique_ptr`.
 *
 *                  2. Aggregation ("Has-a" Weak Ownership) :
 *                     - Loose structural coupling where the parent references the child.
 *                     - Child can exist independently of the parent container.
 *                     - Implementation: `std::shared_ptr`, `std::weak_ptr`, or non-owning pointers/references.
 *
 *                  3. Association ("Knows-a" Structural Relationship) :
 *                     - Structural connection between two independent classes (Unidirectional or Bidirectional).
 *                     - Both objects have independent lifetimes and maintain references to each other.
 *                     - Implementation: Pointers, handles, or IDs.
 *
 *                  4. Dependency ("Uses-a" Transient Relationship) :
 *                     - Weakest relationship where one object uses another temporarily during method execution.
 *                     - No persistent state/member reference stored.
 *                     - Implementation: Pass-by-value/reference in function parameters or local stack variables.
 *
 * TIME COMPLEXITY  : Relationship Creation / Lookup / Invocation : O(1) constant time.
 * SPACE COMPLEXITY : Memory Footprint : Dependent on structural ownership pointers/members.
 * =====================================================================================
 */

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <limits>

using namespace std;

// =====================================================================================
// 1. DEPENDENCY RELATIONSHIP ("USES-A") - TRANSIENT TOOL
// A DiagnosticScanner object used temporarily by a Car during servicing.
// =====================================================================================
class DiagnosticScanner {
private:
    int scannerId_;
    string firmwareVersion_;

public:
    DiagnosticScanner(int id, string version)
        : scannerId_(id), firmwareVersion_(std::move(version)) {
        cout << "    [DiagnosticScanner Ctor] Initialized Scanner ID: " << scannerId_ 
             << " (FW: " << firmwareVersion_ << ")\n";
    }

    ~DiagnosticScanner() noexcept {
        cout << "    [DiagnosticScanner Dtor] Scanner ID: " << scannerId_ << " powered down.\n";
    }

    void performSystemScan(int targetVehicleId) const {
        cout << "    [DEPENDENCY EXECUTION] DiagnosticScanner #" << scannerId_ 
             << " scanning ECU on Vehicle ID #" << targetVehicleId << "... All Systems OK!\n";
    }
};

// =====================================================================================
// 2. COMPOSITION RELATIONSHIP ("HAS-A" STRONG OWNERSHIP) - CHILD OBJECT
// Engine lifecycle is strictly tied to the Car lifecycle.
// =====================================================================================
class Engine {
private:
    int engineId_;
    int horsepower_;
    bool isRunning_{false};

public:
    Engine(int id, int hp) : engineId_(id), horsepower_(hp) {
        cout << "    [COMPOSITION Engine Ctor] Built Engine ID: " << engineId_ 
             << " (" << horsepower_ << " HP)\n";
    }

    ~Engine() noexcept {
        cout << "    [COMPOSITION Engine Dtor] Destroyed Engine ID: " << engineId_ << "\n";
    }

    void start() {
        isRunning_ = true;
        cout << "    [Engine State] Engine #" << engineId_ << " started. Vroom!\n";
    }

    void stop() {
        isRunning_ = false;
        cout << "    [Engine State] Engine #" << engineId_ << " stopped.\n";
    }

    [[nodiscard]] int getEngineId() const noexcept { return engineId_; }
    [[nodiscard]] int getHorsepower() const noexcept { return horsepower_; }
    [[nodiscard]] bool isRunning() const noexcept { return isRunning_; }
};

// =====================================================================================
// 3. AGGREGATION RELATIONSHIP ("HAS-A" WEAK OWNERSHIP) - INDEPENDENT CHILD
// Passenger exists independently of the Car.
// =====================================================================================
class Passenger {
private:
    int passengerId_;
    string name_;

public:
    Passenger(int id, string name)
        : passengerId_(id), name_(std::move(name)) {
        cout << "    [AGGREGATION Passenger Ctor] Passenger #" << passengerId_ 
             << " ('" << name_ << "') arrived at station.\n";
    }

    ~Passenger() noexcept {
        cout << "    [AGGREGATION Passenger Dtor] Passenger #" << passengerId_ 
             << " ('" << name_ << "') left the system.\n";
    }

    [[nodiscard]] int getPassengerId() const noexcept { return passengerId_; }
    [[nodiscard]] const string& getName() const noexcept { return name_; }
};

// Forward declaration for Association
class Driver;

// =====================================================================================
// 4. MAIN CONTAINER CLASS: CAR
// Integrates Composition, Aggregation, Association, and Dependency.
// =====================================================================================
class Car {
private:
    int carId_;
    string modelName_;

    // A. COMPOSITION (Strong Ownership): Engine is created & destroyed with Car
    Engine engine_;

    // B. AGGREGATION (Weak Ownership): Passengers exist independently outside Car
    vector<shared_ptr<Passenger>> passengers_;

    // C. ASSOCIATION (Structural Connection): Pointer to independent Driver object
    Driver* assignedDriver_{nullptr};

public:
    Car(int id, string model, int engineHp)
        : carId_(id), modelName_(std::move(model)), engine_(id + 1000, engineHp) {
        cout << "    [Car Ctor] Constructed Car ID: " << carId_ << " ('" << modelName_ << "')\n";
    }

    ~Car() noexcept {
        cout << "    [Car Dtor] Destroying Car ID: " << carId_ << " ('" << modelName_ << "')\n";
        // Engine's destructor will be called automatically right after Car Dtor (Composition)
    }

    // --- COMPOSITION METHODS ---
    void startCar() { engine_.start(); }
    void stopCar() { engine_.stop(); }

    // --- AGGREGATION METHODS ---
    void boardPassenger(shared_ptr<Passenger> passenger) {
        if (passenger) {
            passengers_.push_back(passenger);
            cout << "    [AGGREGATION] Passenger '" << passenger->getName() 
                 << "' boarded Car #" << carId_ << "\n";
        }
    }

    void listPassengers() const {
        cout << "    [AGGREGATION LIST] Passengers inside Car #" << carId_ << ":\n";
        for (const auto& p : passengers_) {
            cout << "      * ID #" << p->getPassengerId() << ": " << p->getName() << "\n";
        }
    }

    // --- ASSOCIATION METHODS ---
    void setDriver(Driver* driver) noexcept {
        assignedDriver_ = driver;
    }

    [[nodiscard]] Driver* getDriver() const noexcept { return assignedDriver_; }

    // --- DEPENDENCY METHOD ("USES-A") ---
    // Takes DiagnosticScanner temporarily as a parameter. Stored nowhere in member variables!
    void performMaintenanceScan(const DiagnosticScanner& scanner) const {
        cout << "    [DEPENDENCY] Car #" << carId_ << " invoking temporary DiagnosticScanner tool...\n";
        scanner.performSystemScan(carId_);
    }

    [[nodiscard]] int getCarId() const noexcept { return carId_; }
    [[nodiscard]] const string& getModelName() const noexcept { return modelName_; }
};

// =====================================================================================
// 5. ASSOCIATION RELATIONSHIP ("KNOWS-A") - DRIVER CLASS
// Driver and Car know about each other, but both have completely independent lifecycles.
// =====================================================================================
class Driver {
private:
    int driverId_;
    string name_;
    Car* drivingCar_{nullptr}; // Association pointer

public:
    Driver(int id, string name)
        : driverId_(id), name_(std::move(name)) {
        cout << "    [ASSOCIATION Driver Ctor] Driver #" << driverId_ 
             << " ('" << name_ << "') hired.\n";
    }

    ~Driver() noexcept {
        cout << "    [ASSOCIATION Driver Dtor] Driver #" << driverId_ 
             << " ('" << name_ << "') retired.\n";
    }

    // Establish Bidirectional Association
    void assignCar(Car* car) {
        drivingCar_ = car;
        if (car) {
            car->setDriver(this); // Complete mutual association
            cout << "    [ASSOCIATION] Driver '" << name_ << "' assigned to drive Car '" 
                 << car->getModelName() << "'\n";
        }
    }

    void drive() const {
        if (drivingCar_) {
            cout << "    [ASSOCIATION EXECUTION] Driver '" << name_ << "' is driving Car '" 
                 << drivingCar_->getModelName() << "' (ID #" << drivingCar_->getCarId() << ")\n";
        } else {
            cout << "    [ASSOCIATION EXECUTION] Driver '" << name_ << "' has no car assigned!\n";
        }
    }

    [[nodiscard]] int getDriverId() const noexcept { return driverId_; }
    [[nodiscard]] const string& getName() const noexcept { return name_; }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer ID for Object Relationships analysis (e.g., 100): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base ID to 100." << endl;
        userInputValue = 100;
    }

    // =====================================================================================
    // 1. COMPOSITION DEMONSTRATION ("HAS-A" STRONG OWNERSHIP)
    // =====================================================================================
    cout << "\n================ 1. COMPOSITION (\"HAS-A\" STRONG OWNERSHIP) ================\n";
    cout << "  - Concept: Car owns Engine directly. Engine is created WITH Car and destroyed WITH Car.\n\n";

    {
        cout << "  - Creating Car object (Watch Engine Ctor fire as part of Car):\n";
        Car myCar(userInputValue, "Tesla Model S", 670);

        myCar.startCar();
        myCar.stopCar();

        cout << "\n  - Exiting scope (Watch Engine Dtor fire immediately as Car is destroyed):\n";
    }

    // =====================================================================================
    // 2. AGGREGATION DEMONSTRATION ("HAS-A" WEAK OWNERSHIP)
    // =====================================================================================
    cout << "\n================ 2. AGGREGATION (\"HAS-A\" WEAK OWNERSHIP) ================\n";
    cout << "  - Concept: Passengers exist independently. They board Car, but survive Car destruction.\n\n";

    // Passengers instantiated OUTSIDE Car scope
    auto passenger1 = make_shared<Passenger>(userInputValue + 10, "Alice Smith");
    auto passenger2 = make_shared<Passenger>(userInputValue + 11, "Bob Jones");

    {
        cout << "\n  - Creating Car and boarding Passengers...\n";
        Car rideShareCar(userInputValue + 1, "Uber XL", 300);

        rideShareCar.boardPassenger(passenger1);
        rideShareCar.boardPassenger(passenger2);
        rideShareCar.listPassengers();

        cout << "\n  - Destroying Car object scope...\n";
    } // Car destroyed HERE

    cout << "\n  - Verifying Passengers still exist AFTER Car destruction:\n";
    cout << "    * Passenger 1: " << passenger1->getName() << " (Ref Count: " << passenger1.use_count() << ")\n";
    cout << "    * Passenger 2: " << passenger2->getName() << " (Ref Count: " << passenger2.use_count() << ")\n";

    // =====================================================================================
    // 3. ASSOCIATION DEMONSTRATION ("KNOWS-A" STRUCTURAL RELATIONSHIP)
    // =====================================================================================
    cout << "\n================ 3. ASSOCIATION (\"KNOWS-A\" RELATIONSHIP) ================\n";
    cout << "  - Concept: Driver and Car know each other, but have separate independent lifecycles.\n\n";

    {
        Car sportCar(userInputValue + 2, "Porsche 911", 473);
        Driver professionalDriver(userInputValue + 20, "Michael Schumacher");

        // Establish association
        professionalDriver.assignCar(&sportCar);

        // Execute association behavior
        professionalDriver.drive();

        cout << "\n  - Scope ending (Both Driver and Car destroyed independently):\n";
    }

    // =====================================================================================
    // 4. DEPENDENCY DEMONSTRATION ("USES-A" TRANSIENT RELATIONSHIP)
    // =====================================================================================
    cout << "\n================ 4. DEPENDENCY (\"USES-A\" TRANSIENT RELATIONSHIP) ================\n";
    cout << "  - Concept: Car uses DiagnosticScanner temporarily in a method call. No pointer retained.\n\n";

    {
        Car fleetCar(userInputValue + 3, "Ford Transit", 275);
        
        // Diagnostic tool created on stack temporarily
        cout << "\n  - Creating temporary DiagnosticScanner tool on stack...\n";
        DiagnosticScanner scannerTool(userInputValue + 30, "v2.4.1-Pro");

        // Pass by reference (Uses-a transient dependency)
        fleetCar.performMaintenanceScan(scannerTool);

        cout << "\n  - Method completed. FleetCar retains NO persistent reference to scannerTool.\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ OBJECT RELATIONSHIPS SUMMARY =================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Relationship Type     | Relationship Term | Lifetime Coupling | Implementation Mechanism          |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Composition           | \"Has-a\" (Strong)  | Tightly Bound     | Value member / `std::unique_ptr`  |\n"
         << "| Aggregation           | \"Has-a\" (Weak)    | Independent       | `std::shared_ptr` / Raw Pointer   |\n"
         << "| Association           | \"Knows-a\"         | Independent       | Pointer / Reference / ID Handle   |\n"
         << "| Dependency            | \"Uses-a\"          | Transient         | Pass-by-ref / Value parameter     |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}