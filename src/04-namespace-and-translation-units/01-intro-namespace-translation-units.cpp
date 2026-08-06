/*
 * =====================================================================================
 * CONCEPT        : Introduction to Namespaces and Translation Units in C++
 * DESCRIPTION    : Comprehensive implementation explaining namespaces & translation units:
 *                  1. Translation Unit (TU) : A single .cpp file + all included headers 
 *                                             after preprocessing.
 *                  2. Global Name Pollution : How global identifiers collide across TUs.
 *                  3. Named Namespaces      : Structuring code into logical scope blocks 
 *                                             (e.g., `namespace Engine`).
 *                  4. Unnamed/Anonymous     : `namespace { ... }` giving internal linkage 
 *                     Namespaces              (visible ONLY within the current TU).
 *                  5. Nested Namespaces     : `namespace A::B::C` (C++17) for clean hierarchies.
 *                  6. Inline Namespaces     : `inline namespace v2` for seamless API versioning.
 *                  7. Scope Resolution `::` : Disambiguating colliding symbol names.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Scope resolution occurs entirely at compile time.
 * SPACE COMPLEXITY : Best Case: O(1) — Zero runtime memory overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// 1. UNNAMED / ANONYMOUS NAMESPACE (INTERNAL LINKAGE FOR THIS TRANSLATION UNIT)
// Symbols here are visible ONLY inside this single translation unit (.cpp file).
// Replaces legacy 'static' functions/variables for translation-unit local scope.
// =====================================================================================
namespace {
    const string g_translationUnitID = "TU_MAIN_001";

    void logTranslationUnitLocalMessage(const string& msg) {
        cout << "  - [Internal Linkage | " << g_translationUnitID << "] " << msg << endl;
    }
}

// =====================================================================================
// 2. DISAMBIGUATING COLLIDING NAMESPACES
// Multiple namespaces can define symbols with the exact same identifier safely.
// =====================================================================================
namespace PhysicsEngine {
    struct Vector {
        double x;
        double y;
    };

    void computeForce(double mass, double acceleration) {
        cout << "  - [PhysicsEngine::computeForce] Force F = m*a = " << (mass * acceleration) << " N\n";
    }
}

namespace GraphicsEngine {
    struct Vector {
        float x;
        float y;
        float z;
    };

    void computeForce(double loadFactor) {
        cout << "  - [GraphicsEngine::computeForce] Render load = " << (loadFactor * 100.0) << "%\n";
    }
}

// =====================================================================================
// 3. C++17 NESTED NAMESPACE SYNTAX
// Syntax: `namespace Outer::Inner` instead of nested braces.
// =====================================================================================
namespace Framework::Utilities::Logging {
    void logEvent(const string& event) {
        cout << "  - [Framework::Utilities::Logging] Event: \"" << event << "\"\n";
    }
}

// =====================================================================================
// 4. INLINE NAMESPACES FOR API VERSIONING
// Members of an 'inline' namespace are treated as if they were directly in the parent namespace.
// Enables transparent default versioning while keeping older API versions available.
// =====================================================================================
namespace ApplicationAPI {
    // Legacy Version 1 API
    namespace v1 {
        void renderUI() {
            cout << "  - [ApplicationAPI::v1] Rendering legacy 2D UI layout...\n";
        }
    }

    // Modern Version 2 API (Set as default via 'inline')
    inline namespace v2 {
        void renderUI() {
            cout << "  - [ApplicationAPI::v2 (Default)] Rendering modern GPU-accelerated UI layout...\n";
        }
    }
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double massVal = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter mass in kg for physics computation (e.g., 10.0): " << flush;
    if (!(cin >> massVal) || massVal <= 0.0) {
        cout << "Invalid mass input. Defaulting to 10.0." << endl;
        massVal = 10.0;
    }

    // 1. ANONYMOUS NAMESPACE (TRANSLATION UNIT LOCAL LINKAGE)
    cout << "\n================ 1. ANONYMOUS NAMESPACE (INTERNAL LINKAGE) ================" << endl;
    logTranslationUnitLocalMessage("Initializing main translation unit routines...");

    // 2. DISAMBIGUATING IDENTICAL NAMES VIA SCOPE RESOLUTION (`::`)
    cout << "\n================ 2. NAMED NAMESPACES & SCOPE RESOLUTION ================" << endl;
    PhysicsEngine::computeForce(massVal, 9.8);
    GraphicsEngine::computeForce(0.75);

    PhysicsEngine::Vector physVec{1.0, 2.0};
    GraphicsEngine::Vector gfxVec{1.0f, 2.0f, 3.0f};

    cout << "  - PhysicsEngine::Vector2D : (" << physVec.x << ", " << physVec.y << ")\n";
    cout << "  - GraphicsEngine::Vector3D: (" << gfxVec.x << ", " << gfxVec.y << ", " << gfxVec.z << ")\n";

    // 3. C++17 NESTED NAMESPACES & ALIASING
    cout << "\n================ 3. NESTED NAMESPACES & ALIASES ================" << endl;
    Framework::Utilities::Logging::logEvent("System boot completed");

    // Namespace Alias: Creating a concise shorthand for deep nested namespaces
    namespace Log = Framework::Utilities::Logging;
    Log::logEvent("Alias 'Log::logEvent' executed successfully");

    // 4. INLINE NAMESPACES & VERSIONING
    cout << "\n================ 4. INLINE NAMESPACES & API VERSIONING ================" << endl;
    // Calls ApplicationAPI::v2::renderUI() automatically because v2 is 'inline'
    ApplicationAPI::renderUI();

    // Explicitly calling legacy version when needed:
    ApplicationAPI::v1::renderUI();

    // 5. USING DECLARATIONS VS USING DIRECTIVES
    cout << "\n================ 5. USING DECLARATIONS & BEST PRACTICES ================" << endl;
    
    // Recommended: 'using declaration' (Imports specific symbol into scope)
    using PhysicsEngine::computeForce;
    cout << "Calling 'computeForce' directly via scope import:\n";
    computeForce(massVal, 2.5);

    cout << "\n================ NAMESPACES & TRANSLATION UNITS SUMMARY ================" << endl;
    cout << "1. Translation Unit (TU) : Result of single .cpp file after preprocessor includes." << endl;
    cout << "2. Named Namespaces      : Prevents global symbol collisions across libraries & TUs." << endl;
    cout << "3. Anonymous Namespace   : Gives variables/functions internal linkage (TU-private)." << endl;
    cout << "4. Inline Namespaces    : Enables API version updates without breaking legacy callers." << endl;
    cout << "5. Avoid Global `using`  : Never put `using namespace std;` in header files (.h/.hpp)." << endl;

    return 0;
}