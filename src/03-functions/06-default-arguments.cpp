/*
 * =====================================================================================
 * CONCEPT        : Default Arguments in C++ Functions
 * DESCRIPTION    : Comprehensive implementation detailing default argument mechanics:
 *                  1. Declaration Placement   : Default values MUST be specified in the function 
 *                                               declaration (prototype), NOT repeated in the definition.
 *                  2. Trailing Parameter Rule : Default parameters must appear at the end of the 
 *                                               parameter list (right-to-left order).
 *                  3. Call-Site Resolution    : Arguments are matched left-to-right; omitted trailing 
 *                                               arguments automatically take their default values.
 *                  4. Overloading Interaction : How default parameters provide an alternative to 
 *                                               writing multiple overloaded functions.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Direct scalar evaluation and function execution.
 * SPACE COMPLEXITY : Best Case: O(1) — Minimal stack frame allocation.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// =====================================================================================
// FUNCTION DECLARATIONS (PROTOTYPES WITH DEFAULT ARGUMENTS)
// Rule 1: Default argument values belong in the declaration/prototype.
// Rule 2: Default parameters MUST be trailing (from right to left).
// =====================================================================================

/**
 * @brief Configures a window display layout.
 * @param title Title text (mandatory parameter, no default).
 * @param width Display width in pixels (default: 800).
 * @param height Display height in pixels (default: 600).
 * @param fullscreen Fullscreen toggle (default: false).
 */
void configureWindow(const string& title, int width = 800, int height = 600, bool fullscreen = false);

/**
 * @brief Calculates total price with optional tax rate and shipping fee.
 * @param basePrice Item base cost (mandatory parameter).
 * @param taxRate Sales tax multiplier (default: 0.05 / 5%).
 * @param shippingFee Shipping charge (default: 0.0 / free shipping).
 */
double computeTotalCost(double basePrice, double taxRate = 0.05, double shippingFee = 0.0);

/**
 * @brief Formats and logs a system event message.
 * @param message Text payload.
 * @param logLevel Severity label (default: "INFO").
 */
void logSystemEvent(const string& message, const string& logLevel = "INFO");

// =====================================================================================
// MAIN FUNCTION (Entry point demonstrating call-site default argument behavior)
// =====================================================================================
int main() {
    double userPrice = 0.0;
    string userAppTitle;

    // Dynamic input collection with stream flushing
    cout << "Enter application title string (e.g., TradingTerminal): " << flush;
    if (!(cin >> userAppTitle) || userAppTitle.empty()) {
        userAppTitle = "TradingTerminal";
    }

    cout << "Enter base item price (e.g., 100.00): " << flush;
    if (!(cin >> userPrice) || userPrice <= 0.0) {
        cout << "Invalid price input. Defaulting to 100.00." << endl;
        userPrice = 100.00;
    }

    // 1. DEMONSTRATING BASIC DEFAULT ARGUMENT RESOLUTION
    cout << "\n================ 1. CALL-SITE RESOLUTION (LEFT-TO-RIGHT) ================" << endl;
    
    cout << "A. Providing ALL arguments explicitly:" << endl;
    configureWindow(userAppTitle, 1920, 1080, true);

    cout << "\nB. Omitting the last argument ('fullscreen' defaults to false):" << endl;
    configureWindow(userAppTitle, 1280, 720);

    cout << "\nC. Omitting 'height' and 'fullscreen' (defaults to 600 and false):" << endl;
    configureWindow(userAppTitle, 1024);

    cout << "\nD. Omitting ALL optional trailing arguments (defaults to 800x600, false):" << endl;
    configureWindow(userAppTitle);

    // 2. DEMONSTRATING COST CALCULATION WITH MULTIPLE DEFAULTS
    cout << "\n================ 2. MULTIPLE DEFAULTS IN CALCULATIONS ================" << endl;
    
    double costDefault = computeTotalCost(userPrice);
    cout << "  - Default tax (5%) & Free shipping : $" << fixed << setprecision(2) << costDefault << endl;

    double costCustomTax = computeTotalCost(userPrice, 0.10);
    cout << "  - Custom tax (10%) & Free shipping  : $" << fixed << setprecision(2) << costCustomTax << endl;

    double costAllCustom = computeTotalCost(userPrice, 0.10, 15.50);
    cout << "  - Custom tax (10%) & $15.50 Shipping: $" << fixed << setprecision(2) << costAllCustom << endl;

    // 3. LOGGING SYSTEM EVENT DEMONSTRATION
    cout << "\n================ 3. PRACTICAL DEFAULT PARAMETER LOGGING ================" << endl;
    logSystemEvent("System initialized successfully"); // Uses default "INFO"
    logSystemEvent("High CPU usage detected", "WARNING");
    logSystemEvent("Memory allocation failure", "CRITICAL");

    cout << "\n================ DEFAULT ARGUMENTS SUMMARY & RULES ================" << endl;
    cout << "1. Location Rule : Place default argument values in DECLARATIONS (.h), not in definitions (.cpp)." << endl;
    cout << "2. Trailing Rule : Default parameters MUST be placed at the END of the parameter list." << endl;
    cout << "3. Binding Order : Arguments are assigned left-to-right; omitted trailing values use defaults." << endl;
    cout << "4. No Skipping   : You CANNOT supply the 3rd argument while omitting the 2nd argument." << endl;
    cout << "5. Overloading   : Default parameters often reduce boilerplate by replacing multiple function overloads." << endl;

    return 0;
}

// =====================================================================================
// FUNCTION DEFINITIONS
// Note: Default argument values MUST NOT be repeated here!
// Re-specifying defaults in the definition causes a compiler error.
// =====================================================================================

void configureWindow(const string& title, int width, int height, bool fullscreen) {
    cout << "  [Window Config] Title: " << title 
         << " | Resolution: " << width << "x" << height 
         << " | Fullscreen: " << (fullscreen ? "YES" : "NO") << endl;
}

double computeTotalCost(double basePrice, double taxRate, double shippingFee) {
    double taxAmount = basePrice * taxRate;
    return basePrice + taxAmount + shippingFee;
}

void logSystemEvent(const string& message, const string& logLevel) {
    cout << "  [" << logLevel << "] " << message << endl;
}