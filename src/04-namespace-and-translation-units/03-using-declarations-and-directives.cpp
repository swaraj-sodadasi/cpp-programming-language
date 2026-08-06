/*
 * =====================================================================================
 * CONCEPT        : Using Declarations vs Using Directives in C++
 * DESCRIPTION    : Comprehensive implementation detailing scope importing mechanics:
 *                  1. Using Declaration (`using Namespace::Symbol`) :
 *                     - Selectively introduces a SINGLE symbol into the current declarative scope.
 *                     - Safe, predictable, and prevents unexpected name collisions.
 *                  2. Using Directive (`using namespace Namespace`) :
 *                     - Nominally introduces ALL symbols from a namespace into the enclosing scope.
 *                     - Convenient for small local scopes, but risks global scope pollution and ambiguity errors.
 *                  3. Namespace Aliasing (`namespace Alias = Deep::Nested`) :
 *                     - Cleanly shortens deep hierarchical namespace paths without dumping symbols into scope.
 *                  4. Scope Pollution & Shadowing :
 *                     - How local variables, using declarations, and using directives interact with shadowing.
 *                  5. Header File Safety Rules :
 *                     - Why `using` directives/declarations must NEVER be placed in global scope of header files.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Resolved entirely at compile time during symbol lookup.
 * SPACE COMPLEXITY : Best Case: O(1) — Zero runtime memory or stack overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>

using namespace std;

// =====================================================================================
// NAMESPACE DEFINITIONS FOR DEMONSTRATION
// =====================================================================================

namespace AudioEngine {
    int g_systemVolume = 80;

    void processSignal(double frequency) {
        cout << "  - [AudioEngine] Processing audio frequency: " << frequency << " Hz\n";
    }

    void calibrate() {
        cout << "  - [AudioEngine] Calibrating speakers...\n";
    }
}

namespace VideoEngine {
    int g_systemVolume = 100; // Identical symbol name

    void processSignal(double frameRate) {
        cout << "  - [VideoEngine] Processing video frame rate: " << frameRate << " FPS\n";
    }

    void renderFrame() {
        cout << "  - [VideoEngine] Rendering video frame...\n";
    }
}

namespace Application::Core::Subsystems::Networking {
    void establishConnection(const string& endpoint) {
        cout << "  - [Networking] Connected to endpoint: " << endpoint << "\n";
    }
}

// =====================================================================================
// 1. USING DECLARATION DEMONSTRATION
// `using Namespace::Symbol;`
// =====================================================================================
void demonstrateUsingDeclaration(double freqVal) {
    cout << "\n================ 1. USING DECLARATION (Selective Import) ================" << endl;
    
    // Selective import: ONLY 'processSignal' from AudioEngine is introduced into this function scope
    using AudioEngine::processSignal;

    // Call processSignal directly without 'AudioEngine::' prefix
    processSignal(freqVal);

    // Other symbols from AudioEngine still require full qualification:
    AudioEngine::calibrate();

    // Trying to import VideoEngine::processSignal in the same scope causes a compile error due to name collision:
    // using VideoEngine::processSignal; // COMPILER ERROR: conflict with 'using AudioEngine::processSignal'
}

// =====================================================================================
// 2. USING DIRECTIVE DEMONSTRATION
// `using namespace Namespace;`
// =====================================================================================
void demonstrateUsingDirective() {
    cout << "\n================ 2. USING DIRECTIVE (Bulk Import) ================" << endl;

    {
        // Bulk import: Brings ALL symbols from VideoEngine into this local scope block
        using namespace VideoEngine;

        cout << "  - Accessing 'g_systemVolume' via VideoEngine directive: " << g_systemVolume << endl;
        processSignal(60.0);
        renderFrame();
    } // VideoEngine symbols leave scope here

    // DEMONSTRATING AMBIGUITY HAZARD OF USING DIRECTIVES:
    {
        using namespace AudioEngine;
        using namespace VideoEngine;

        // Calling 'calibrate()' or 'renderFrame()' works fine because they are unique:
        calibrate();
        renderFrame();

        // BUT calling 'processSignal(50.0)' or accessing 'g_systemVolume' causes COMPILATION ERROR!
        // processSignal(440.0); // COMPILER ERROR: Call to 'processSignal' is ambiguous!
        // cout << g_systemVolume; // COMPILER ERROR: Reference to 'g_systemVolume' is ambiguous!

        cout << "  - [Hazard Note] Unqualified call to 'processSignal()' here is AMBIGUOUS because both\n"
             << "                  AudioEngine and VideoEngine directives are active in the same scope.\n";
        
        // Resolution requires explicit scope qualification:
        AudioEngine::processSignal(440.0);
        VideoEngine::processSignal(120.0);
    }
}

// =====================================================================================
// 3. NAMESPACE ALIASING
// =====================================================================================
void demonstrateNamespaceAlias(const string& endpoint) {
    cout << "\n================ 3. NAMESPACE ALIASING ================" << endl;

    // Creating a short alias for a deeply nested namespace path
    namespace Net = Application::Core::Subsystems::Networking;

    // Clean, readable invocation without scope pollution
    Net::establishConnection(endpoint);
}

// =====================================================================================
// 4. SHADOWING & SCOPE OVERRIDING
// =====================================================================================
void demonstrateScopeShadowing() {
    cout << "\n================ 4. SCOPE SHADOWING & OVERRIDING ================" << endl;

    int g_systemVolume = 10; // Local variable

    using namespace AudioEngine; // Introduces AudioEngine::g_systemVolume nominally

    // Local variable 'g_systemVolume' SHADOWS the namespace symbol brought by directive!
    cout << "  - Local 'g_systemVolume' shadows directive volume: " << g_systemVolume << endl;

    // To access the namespace volume explicitly:
    cout << "  - AudioEngine::g_systemVolume explicitly accessed: " << AudioEngine::g_systemVolume << endl;

    // A Using Declaration for an identical name in the same scope as a local variable is a COMPILER ERROR:
    // using AudioEngine::g_systemVolume; // COMPILER ERROR: redeclaration of 'int g_systemVolume'
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    double userFrequency = 0.0;

    // Dynamic input collection with stream flushing
    cout << "Enter audio signal frequency in Hz (e.g., 440.0): " << flush;
    if (!(cin >> userFrequency) || userFrequency <= 0.0) {
        cout << "Invalid frequency input. Defaulting to 440.0 Hz." << endl;
        userFrequency = 440.0;
    }

    // 1. USING DECLARATION
    demonstrateUsingDeclaration(userFrequency);

    // 2. USING DIRECTIVE & AMBIGUITY HAZARD
    demonstrateUsingDirective();

    // 3. NAMESPACE ALIASING
    demonstrateNamespaceAlias("https://api.service.internal:8443");

    // 4. SHADOWING RULES
    demonstrateScopeShadowing();

    cout << "\n================ BEST PRACTICES & SUMMARY ================" << endl;
    cout << "1. Using Declaration (`using N::symbol`) : SAFE. Prefer this inside function scopes for specific symbols." << endl;
    cout << "2. Using Directive   (`using namespace N`) : USE WITH CAUTION. Acceptable inside tight function scopes;" << endl;
    cout << "                                            AVOID in global scope." << endl;
    cout << "3. Header File Rule                      : NEVER place `using` directives/declarations in header files (.h/.hpp)" << endl;
    cout << "                                            at global scope, as it forces scope pollution on all inclusion sites." << endl;
    cout << "4. Namespace Aliases (`namespace A = N`)  : Excellent alternative for deep paths without polluting scope." << endl;

    return 0;
}