/*
 * =====================================================================================
 * CONCEPT        : Organising Large C++ Projects (Architecture, Modules & Clean Code)
 * DESCRIPTION    : Comprehensive program illustrating architectural patterns for large C++ codebases:
 *                  1. Directory & File Layout  : Separation into `/include`, `/src`, `/tests`, `/cmake`.
 *                  2. Layered Namespaces       : Modular hierarchy (`Core`, `Domain`, `Infrastructure`, `Services`).
 *                  3. Dependency Injection     : Passing interfaces (`IRepository`) instead of concrete instances.
 *                  4. Compilation Insulation   : Pimpl (Pointer to Implementation) idiom to eliminate header dependency cascades.
 *                  5. Service Facade & Factory : High-level API insulating complex subsystem orchestration.
 *                  6. Const-Correctness        : Resolving map lookup errors in const member functions via `.at()` / `.find()`.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution.
 * SPACE COMPLEXITY : Best Case: O(1) stack allocation with RAII smart pointer ownership.
 * =====================================================================================
 *
 * TYPICAL ENTERPRISE DIRECTORY STRUCTURE:
 * -------------------------------------------------------------------------------------
 * my_large_project/
 * ├── CMakeLists.txt                 # Root build system
 * ├── cmake/                         # CMake modules, toolchains, & dependencies
 * ├── include/                       # Public API header files
 * │   └── MyProject/
 * │       ├── Core/                  # Base utilities, logging, platform abstractions
 * │       ├── Domain/                # Domain models & entities
 * │       ├── Infrastructure/        # DB, network, external API interfaces
 * │       └── Services/              # High-level business logic facades
 * ├── src/                           # Private implementation source files (.cpp)
 * │   ├── Core/
 * │   ├── Domain/
 * │   ├── Infrastructure/
 * │   └── Services/
 * ├── tests/                         # Unit & Integration tests (GTest / Catch2)
 * └── third_party/                   # Vendor libraries / submodules
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <optional>
#include <utility>

using namespace std;

// =====================================================================================
// LAYER 1: CORE SUBSYSTEM (LOGGING & INFRASTRUCTURE UTILITIES)
// =====================================================================================
namespace EnterpriseApp::Core {

enum class LogLevel { INFO, WARNING, ERROR_LOG };

class Logger {
public:
    static void log(LogLevel level, const string& module, const string& message) {
        string prefix;
        switch (level) {
            case LogLevel::INFO:      prefix = "[INFO] "; break;
            case LogLevel::WARNING:   prefix = "[WARN] "; break;
            case LogLevel::ERROR_LOG: prefix = "[ERR ] "; break;
        }
        cout << "  - " << prefix << "[" << module << "] " << message << "\n";
    }
};

} // namespace EnterpriseApp::Core


// =====================================================================================
// LAYER 2: DOMAIN MODELS
// Pure data structures representing enterprise domain entities.
// =====================================================================================
namespace EnterpriseApp::Domain {

struct OrderItem {
    string productId;
    int quantity;
    double unitPrice;
};

class Order {
private:
    string orderId_;
    string customerId_;
    vector<OrderItem> items_;

public:
    Order(string orderId, string customerId)
        : orderId_(std::move(orderId)), customerId_(std::move(customerId)) {}

    void addItem(const string& prodId, int qty, double price) {
        items_.push_back({prodId, qty, price});
    }

    [[nodiscard]] const string& getOrderId() const { return orderId_; }
    [[nodiscard]] const string& getCustomerId() const { return customerId_; }
    [[nodiscard]] const vector<OrderItem>& getItems() const { return items_; }

    [[nodiscard]] double calculateTotal() const {
        double total = 0.0;
        for (const auto& item : items_) {
            total += item.quantity * item.unitPrice;
        }
        return total;
    }
};

} // namespace EnterpriseApp::Domain


// =====================================================================================
// LAYER 3: INFRASTRUCTURE LAYER (INTERFACES & REPOSITORIES)
// Abstract contracts decoupling business logic from databases/external APIs.
// =====================================================================================
namespace EnterpriseApp::Infrastructure {

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;
    virtual bool saveOrder(const Domain::Order& order) = 0;
    virtual std::optional<Domain::Order> findOrderById(const string& orderId) = 0;
};

// Concrete Repository Implementation (In-Memory Database for demonstration)
class InMemoryOrderRepository final : public IOrderRepository {
private:
    unordered_map<string, Domain::Order> storage_;

public:
    bool saveOrder(const Domain::Order& order) override {
        storage_.insert_or_assign(order.getOrderId(), order);
        Core::Logger::log(Core::LogLevel::INFO, "InMemoryDB", 
            "Persisted Order #" + order.getOrderId() + " to storage table.");
        return true;
    }

    std::optional<Domain::Order> findOrderById(const string& orderId) override {
        auto it = storage_.find(orderId);
        if (it != storage_.end()) {
            return it->second;
        }
        return std::nullopt;
    }
};

} // namespace EnterpriseApp::Infrastructure


// =====================================================================================
// LAYER 4: HEAVY SUBSYSTEM WITH PIMPL IDIOM (COMPILATION INSULATION)
// Prevents heavy dependencies (e.g., complex calculation engines) from forcing rebuilds.
// =====================================================================================
namespace EnterpriseApp::Services {

class TaxCalculationEngine {
private:
    // Forward declaration of private implementation structure
    struct Impl;
    unique_ptr<Impl> pimpl_; // Pointer to Implementation (Pimpl)

public:
    TaxCalculationEngine();
    ~TaxCalculationEngine(); // Must be explicitly declared in header, defined in .cpp where Impl is complete

    [[nodiscard]] double computeTax(double amount, const string& regionCode) const;
};

// Implementation definition (In a real project, this resides in TaxCalculationEngine.cpp)
struct TaxCalculationEngine::Impl {
    unordered_map<string, double> regionalTaxRates;

    Impl() {
        regionalTaxRates["US_CA"] = 0.0925;
        regionalTaxRates["US_NY"] = 0.08875;
        regionalTaxRates["EU_DE"] = 0.1900;
        regionalTaxRates["DEFAULT"] = 0.0500;
    }

    double getRate(const string& region) const {
        auto it = regionalTaxRates.find(region);
        if (it != regionalTaxRates.end()) {
            return it->second;
        }
        // RESOLVED: Replaced operator[] with .at() to support const member function access
        return regionalTaxRates.at("DEFAULT");
    }
};

TaxCalculationEngine::TaxCalculationEngine() : pimpl_(make_unique<Impl>()) {}
TaxCalculationEngine::~TaxCalculationEngine() = default;

double TaxCalculationEngine::computeTax(double amount, const string& regionCode) const {
    double rate = pimpl_->getRate(regionCode);
    return amount * rate;
}

} // namespace EnterpriseApp::Services


// =====================================================================================
// LAYER 5: SERVICE FACADE (APPLICATION ORCHESTRATION WITH DEPENDENCY INJECTION)
// Orchestrates Domain, Infrastructure, and Core Subsystems cleanly.
// =====================================================================================
namespace EnterpriseApp::Services {

class OrderFulfillmentFacade {
private:
    // Dependencies injected via interfaces (Inversion of Control)
    shared_ptr<Infrastructure::IOrderRepository> repository_;
    TaxCalculationEngine taxEngine_;

public:
    explicit OrderFulfillmentFacade(shared_ptr<Infrastructure::IOrderRepository> repo)
        : repository_(std::move(repo)) {}

    bool processOrderFulfillment(const string& orderId, const string& customerId, 
                                 const string& regionCode, double amount) {
        Core::Logger::log(Core::LogLevel::INFO, "OrderFacade", 
            "Starting fulfillment pipeline for Order #" + orderId);

        Domain::Order order(orderId, customerId);
        order.addItem("SKU-9001", 1, amount);

        double baseTotal = order.calculateTotal();
        double taxAmount = taxEngine_.computeTax(baseTotal, regionCode);
        double finalPrice = baseTotal + taxAmount;

        Core::Logger::log(Core::LogLevel::INFO, "OrderFacade", 
            "Calculated Base: $" + to_string(baseTotal) + " | Tax (" + regionCode + "): $" + to_string(taxAmount));

        // Persist order via repository interface
        if (repository_ && repository_->saveOrder(order)) {
            Core::Logger::log(Core::LogLevel::INFO, "OrderFacade", 
                "Order #" + orderId + " fulfilled successfully! Final Charged Amount: $" + to_string(finalPrice));
            return true;
        }

        Core::Logger::log(Core::LogLevel::ERROR_LOG, "OrderFacade", "Failed to persist order!");
        return false;
    }
};

} // namespace EnterpriseApp::Services


// =====================================================================================
// MAIN ENTRY POINT (SIMULATING PRODUCTION APPLICATION ENTRY)
// =====================================================================================
int main() {
    using namespace EnterpriseApp;

    double itemPriceInput = 0.0;
    string regionInput;

    // Dynamic input collection with stream flushing
    cout << "Enter Item Price USD (e.g., 199.99): " << flush;
    if (!(cin >> itemPriceInput) || itemPriceInput <= 0.0) {
        cout << "Invalid input. Defaulting item price to $199.99." << endl;
        itemPriceInput = 199.99;
    }

    cout << "Enter Region Code (e.g., US_CA, US_NY, EU_DE): " << flush;
    cin >> regionInput;
    if (regionInput.empty()) {
        regionInput = "US_CA";
    }

    // 1. APPLICATION INITIALIZATION & DEPENDENCY INJECTION
    cout << "\n================ 1. INITIALIZING APPLICATION SUBSYSTEMS ================\n";
    
    // Instantiate concrete infrastructure dependency
    auto orderRepo = make_shared<Infrastructure::InMemoryOrderRepository>();

    // Inject repository dependency into high-level Application Facade
    Services::OrderFulfillmentFacade fulfillmentService(orderRepo);

    // 2. EXECUTING APPLICATION PIPELINE
    cout << "\n================ 2. EXECUTING ORDER FULFILLMENT FACADE ================\n";
    fulfillmentService.processOrderFulfillment("ORD-2026-8801", "CUST-402", regionInput, itemPriceInput);

    // 3. ARCHITECTURAL BEST PRACTICES SUMMARY
    cout << "\n================ LARGE PROJECT ORGANISATION SUMMARY ================\n";
    cout << "1. Layered Namespaces   : Separate code by architectural responsibility (Core, Domain, Infra, Services).\n";
    cout << "2. Dependency Injection : High-level services depend on abstract interfaces (IOrderRepository), not concrete DBs.\n";
    cout << "3. Pimpl Idiom Insulation: Heavy engines use Pimpl to hide private implementation headers and speed up build times.\n";
    cout << "4. Service Facades      : Hide complex orchestration logic behind clean, high-level API methods.\n";
    cout << "5. Const-Correct Maps   : Use `.at()` or `.find()` instead of `operator[]` inside `const` member functions.\n";

    return 0;
}