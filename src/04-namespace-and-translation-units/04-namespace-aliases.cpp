/*
 * =====================================================================================
 * CONCEPT        : Namespace Aliases in C++
 * DESCRIPTION    : Comprehensive implementation detailing namespace aliasing mechanics:
 *                  1. Syntax & Basic Declaration: `namespace Alias = Long::Nested::Namespace;`
 *                  2. Deep Hierarchy Simplification: Eliminating verbose scope qualifiers in call sites.
 *                  3. Scoped Namespace Aliasing : Defining aliases locally inside functions or blocks.
 *                  4. Backend / Implementation Swapping: Using aliases to switch underlying modules
 *                                                       (e.g., Vulkan vs DirectX, Mock vs Live API).
 *                  5. Chained Aliasing          : Aliasing an existing namespace alias.
 *
 * TIME COMPLEXITY  : Best Case: O(1) — Resolved entirely at compile time during symbol resolution.
 * SPACE COMPLEXITY : Best Case: O(1) — Zero runtime memory, stack, or binary performance overhead.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// =====================================================================================
// DEEPLY NESTED NAMESPACE HIERARCHIES
// =====================================================================================

namespace Enterprise::Framework::Subsystems::Graphics::RenderEngine {
    struct RenderMetrics {
        int drawCalls;
        double frameTimeMs;
    };

    void drawFrame(const string& sceneName, int objectCount) {
        cout << "  - [RenderEngine] Drawing scene '" << sceneName 
             << "' with " << objectCount << " objects.\n";
    }
}

// Backend 1: Vulkan Driver Implementation
namespace Application::Drivers::Vulkan {
    void initializePipeline() {
        cout << "  - [Vulkan Driver] Vulkan 1.3 pipeline initialized successfully.\n";
    }

    void submitCommandBuffer(int bufferId) {
        cout << "  - [Vulkan Driver] Command buffer #" << bufferId << " submitted to GPU queue.\n";
    }
}

// Backend 2: DirectX 12 Driver Implementation
namespace Application::Drivers::DirectX12 {
    void initializePipeline() {
        cout << "  - [DirectX12 Driver] DirectX 12 command queue and pipeline state created.\n";
    }

    void submitCommandBuffer(int bufferId) {
        cout << "  - [DirectX12 Driver] Command list #" << bufferId << " executed on Direct queue.\n";
    }
}

// =====================================================================================
// 1. GLOBAL NAMESPACE ALIAS DECLARATION
// Syntax: `namespace ShortName = Long::Nested::Namespace::Path;`
// =====================================================================================

// Global alias for the long graphics subsystem path
namespace GfxEngine = Enterprise::Framework::Subsystems::Graphics::RenderEngine;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userObjectCount = 0;
    int backendSelection = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter object count for rendering scene (e.g., 250): " << flush;
    if (!(cin >> userObjectCount) || userObjectCount <= 0) {
        cout << "Invalid count input. Defaulting object count to 250." << endl;
        userObjectCount = 250;
    }

    cout << "Select Graphics Backend API (1 = Vulkan, 2 = DirectX 12): " << flush;
    if (!(cin >> backendSelection) || (backendSelection != 1 && backendSelection != 2)) {
        cout << "Invalid choice. Defaulting to Vulkan (1)." << endl;
        backendSelection = 1;
    }

    // 1. USING GLOBAL NAMESPACE ALIASES
    cout << "\n================ 1. BASIC NAMESPACE ALIASING ================" << endl;
    cout << "Without Alias : Enterprise::Framework::Subsystems::Graphics::RenderEngine::drawFrame(...)\n";
    cout << "With Alias    : GfxEngine::drawFrame(...)\n\n";

    // Calling function using global alias 'GfxEngine'
    GfxEngine::drawFrame("MainLevel_01", userObjectCount);

    GfxEngine::RenderMetrics metrics{42, 16.67};
    cout << "  - Metrics via Alias -> Draw Calls: " << metrics.drawCalls 
         << " | Frame Time: " << fixed << setprecision(2) << metrics.frameTimeMs << " ms\n";

    // 2. SCOPED / LOCAL NAMESPACE ALIASES
    cout << "\n================ 2. SCOPED (FUNCTION-LOCAL) ALIASES ================" << endl;
    {
        // Local namespace alias active ONLY inside this scope block {}
        namespace LocalGfx = Enterprise::Framework::Subsystems::Graphics::RenderEngine;
        LocalGfx::drawFrame("LocalScope_Shadow", 10);
    } // LocalGfx scope ends here

    // 3. BACKEND SWAPPING VIA NAMESPACE ALIASING
    cout << "\n================ 3. BACKEND SWAPPING VIA ALIASING ================" << endl;
    
    // Switch the underlying driver backend using a local namespace alias based on user input
    if (backendSelection == 1) {
        namespace ActiveDriver = Application::Drivers::Vulkan;
        cout << "  [Config] Active Driver selected: Vulkan\n";
        ActiveDriver::initializePipeline();
        ActiveDriver::submitCommandBuffer(101);
    } else {
        namespace ActiveDriver = Application::Drivers::DirectX12;
        cout << "  [Config] Active Driver selected: DirectX 12\n";
        ActiveDriver::initializePipeline();
        ActiveDriver::submitCommandBuffer(202);
    }

    // 4. CHAINED NAMESPACE ALIASING
    cout << "\n================ 4. CHAINED NAMESPACE ALIASES ================" << endl;
    
    // Creating an alias from an existing alias
    namespace FastGfx = GfxEngine; // GfxEngine is already an alias for Enterprise::Framework::...
    
    FastGfx::drawFrame("ChainedAlias_Scene", userObjectCount * 2);

    cout << "\n================ NAMESPACE ALIASES SUMMARY ================" << endl;
    cout << "1. Syntax Structure : `namespace Alias = Original::Nested::Namespace;`" << endl;
    cout << "2. Primary Benefit  : Replaces verbose scope qualifiers with short, intuitive aliases." << endl;
    cout << "3. Safety Guarantee : Unlike `using namespace`, aliases DO NOT dump symbols into the scope;" << endl;
    cout << "                      they require explicit qualification (`Alias::Symbol`)." << endl;
    cout << "4. Abstraction Tool : Allows clean swapping of libraries/backends without touching business logic." << endl;
    cout << "5. Compile-Time     : Pure compile-time alias mapping with ZERO runtime performance cost." << endl;

    return 0;
}