/*
 * =====================================================================================
 * CONCEPT        : Best Practices in Namespaces and Translation Units (TUs) in C++
 * DESCRIPTION    : Comprehensive program illustrating industry-standard best practices:
 *                  1. No Global `using namespace std;`: Avoid header & global pollution; 
 *                     use explicit qualification (`std::`) or function-scoped `using`.
 *                  2. Anonymous Namespaces         : Replaces file-scope `static` for internal
 *                                                    linkage (TU privacy).
 *                  3. Inline Namespaces            : ABI versioning (`namespace v1`, `inline namespace v2`).
 *                  4. C++17 Nested Syntax & Aliases: `namespace Outer::Inner` and `namespace Alias = ...`.
 *                  5. ADL (Koenig Lookup) Hygiene  : Keeping helper operators/functions in the same 
 *                                                    namespace as their target parameter types.
 *                  6. Header vs TU Insulation      : Preventing ODR violations and symbol bloat.
 *
 * TIME COMPLEXITY  : Best Case: O(1) runtime execution.
 * SPACE COMPLEXITY : Best Case: O(1) stack frame footprint.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

// BEST PRACTICE 1: Do NOT put `using namespace std;` at global scope or in headers!
// Always use explicit qualification or local scope declarations to prevent namespace pollution.

// =====================================================================================
// SECTION 1: ANONYMOUS NAMESPACES FOR TRANSLATION UNIT (TU) PRIVACY
// Anonymous namespaces grant Internal Linkage to all contained entities.
// Preferred over C-style global `static` variables/functions in modern C++.
// =====================================================================================
namespace {
    // This variable is visible ONLY within this translation unit (.cpp file).
    const std::string g_tuPrivateKey = "TU_LOCAL_ENCRYPTION_KEY_2026";

    void executeTuPrivateTask() {
        std::cout << "  - [Anonymous Namespace] Executed private TU routine. Secret Key: " 
                  << g_tuPrivateKey << "\n";
    }
} // Unnamed / Anonymous namespace

// =====================================================================================
// SECTION 2: ABI VERSIONING WITH INLINE NAMESPACES
// Allows updating default library implementation while preserving backward compatibility.
// =====================================================================================
namespace PlatformSDK {

    namespace v1 {
        class DataFormatter {
        public:
            static std::string format(const std::string& input) {
                return "[V1 Format] " + input;
            }
        };
    } // namespace v1

    // `inline namespace` makes members accessible directly in `PlatformSDK::`
    inline namespace v2 {
        class DataFormatter {
        public:
            static std::string format(const std::string& input) {
                return "== [V2 Enhanced Format] " + input + " ==";
            }
        };
    } // namespace v2
} // namespace PlatformSDK

// =====================================================================================
// SECTION 3: C++17 NESTED NAMESPACES & NAMESPACE ALIASES
// Clean directory-like namespace hierarchy without nested boilerplate.
// =====================================================================================
namespace FinancialEngine::Analytics::Risk {

    struct RiskProfile {
        std::string assetClass;
        double volatilityScore;
    };

    class RiskEvaluator {
    public:
        static void evaluate(const RiskProfile& profile) {
            std::cout << "  - [RiskEvaluator] Asset: " << profile.assetClass 
                      << " | Volatility Score: " << std::fixed << std::setprecision(2) 
                      << profile.volatilityScore << "\n";
        }
    };

} // namespace FinancialEngine::Analytics::Risk

// BEST PRACTICE 3: Use Namespace Aliases to shorten deeply nested scopes safely.
namespace RiskAnalytics = FinancialEngine::Analytics::Risk;

// =====================================================================================
// SECTION 4: ARGUMENT-DEPENDENT LOOKUP (ADL) HYGIENE
// Functions tied to custom types should reside in the same namespace to enable ADL
// without polluting global or parent scopes.
// =====================================================================================
namespace HighFrequencyTrading {

    struct OrderPacket {
        int orderId;
        double price;
    };

    // Found automatically via ADL when calling printOrderPacket(pkt)
    void printOrderPacket(const OrderPacket& pkt) {
        std::cout << "  - [ADL Match] HighFrequencyTrading::OrderPacket #" 
                  << pkt.orderId << " @ $" << std::fixed << std::setprecision(2) << pkt.price << "\n";
    }

} // namespace HighFrequencyTrading

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    // BEST PRACTICE 4: Scoped `using` declarations inside functions are safe!
    using std::cout;
    using std::cin;
    using std::endl;
    using std::flush;

    std::string userAssetInput;
    double volatilityInput = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter Asset Class Name (e.g., Equity_Derivatives): " << flush;
    cin >> userAssetInput;
    if (userAssetInput.empty()) {
        userAssetInput = "Equity_Derivatives";
    }

    cout << "Enter Asset Volatility Score (e.g., 14.75): " << flush;
    if (!(cin >> volatilityInput) || volatilityInput <= 0.0) {
        cout << "Invalid volatility score. Defaulting to 14.75." << endl;
        volatilityInput = 14.75;
    }

    // 1. ANONYMOUS NAMESPACE DEMO
    cout << "\n================ 1. TRANSLATION UNIT PRIVACY (ANONYMOUS NAMESPACE) ================\n";
    executeTuPrivateTask();

    // 2. INLINE NAMESPACE VERSIONING DEMO
    cout << "\n================ 2. API VERSIONING (INLINE NAMESPACES) ================\n";
    // Unqualified call uses v2 (inline default)
    cout << "  - Default (v2)  : " << PlatformSDK::DataFormatter::format("PayloadData") << "\n";
    // Explicitly calling legacy v1
    cout << "  - Explicit (v1) : " << PlatformSDK::v1::DataFormatter::format("PayloadData") << "\n";

    // 3. C++17 NESTED SYNTAX & ALIASES DEMO
    cout << "\n================ 3. C++17 NESTED NAMESPACES & ALIASES ================\n";
    RiskAnalytics::RiskProfile profile{userAssetInput, volatilityInput};
    RiskAnalytics::RiskEvaluator::evaluate(profile);

    // 4. ADL HYGIENE DEMO
    cout << "\n================ 4. ARGUMENT-DEPENDENT LOOKUP (ADL) ================\n";
    HighFrequencyTrading::OrderPacket packet{10088, 450.25};
    // Unqualified function call works thanks to ADL inspecting parameter's namespace
    printOrderPacket(packet);

    // 5. SUMMARY OF BEST PRACTICES
    cout << "\n================ NAMESPACE & TU BEST PRACTICES SUMMARY ================\n";
    cout << "+---------------------------------+---------------------------------------------------+\n"
         << "| Practice Rule                   | Justification / Industry Benefit                  |\n"
         << "+---------------------------------+---------------------------------------------------+\n"
         << "| Avoid Global `using namespace`  | Prevents standard symbol collision & name hijacking|\n"
         << "| Use Anonymous Namespaces in .cpp| Grants internal linkage; isolates TU private state|\n"
         << "| Use Inline Namespaces for API   | Enables non-breaking ABI & API library updates    |\n"
         << "| Use Namespace Aliases           | Simplifies deep path references without pollution |\n"
         << "| Keep Overloads with Type        | Ensures ADL functions without global lookup clutter|\n"
         << "| Prefer C++17 `namespace A::B`   | Reduces boilerplate header indentation levels     |\n"
         << "+---------------------------------+---------------------------------------------------+\n";

    return 0;
}