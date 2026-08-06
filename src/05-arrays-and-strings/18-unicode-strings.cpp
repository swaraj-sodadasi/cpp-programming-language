/*
 * =====================================================================================
 * CONCEPT        : Unicode Strings in C++ (UTF-8, UTF-16, UTF-32 & Wide Strings)
 * DESCRIPTION    : Comprehensive guide and implementation of Unicode handling in C++:
 *                  1. Unicode Encoding Schemes   : UTF-8 (1-4 bytes), UTF-16 (2 or 4 bytes), 
 *                                                  UTF-32 (fixed 4 bytes per code point).
 *                  2. C++ Character Types        : `char` (UTF-8 in modern OS/C++11+),
 *                                                  `char16_t` (UTF-16), `char32_t` (UTF-32), `wchar_t` (Platform-dependent),
 *                                                  `char8_t` (C++20 explicit UTF-8 type).
 *                  3. String Containers          : `std::string` (UTF-8), `std::u16string`, 
 *                                                  `std::u32string`, `std::wstring`, and C++20 `std::u8string`.
 *                  4. Code Units vs Code Points  : `.size()` measures code units, NOT user-perceived 
 *                                                  grapheme clusters / emojis.
 *                  5. Code Point Iteration       : Iterating UTF-32 code points (U+XXXX).
 *                  6. Portable Fallbacks         : Feature-testing C++20 `std::u8string` to ensure 
 *                                                  compatibility across C++11, C++14, C++17, and C++20.
 *
 * TIME COMPLEXITY  : String Operations: O(N) over code units.
 * SPACE COMPLEXITY : UTF-8: 1-4 bytes/char | UTF-16: 2 or 4 bytes/char | UTF-32: 4 bytes/char.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <string_view>
#include <iomanip>
#include <cstddef>
#include <cstdint>

using namespace std;

// =====================================================================================
// PORTABLE C++20 FEATURE GUARDS & FALLBACK TYPEDEFS
// Resolves missing 'std::u8string' / 'std::u8string_view' on pre-C++20 compilers
// =====================================================================================
#if __cplusplus >= 202002L && defined(__cpp_lib_char8_t)
    using u8str_t = std::u8string;
    using u8view_t = std::u8string_view;
    #define HAS_CPP20_U8STRING 1
#else
    using u8str_t = std::string;
    using u8view_t = std::string_view;
    #define HAS_CPP20_U8STRING 0
#endif

// Helper to safely render UTF-8 view to std::cout across all C++ standards
std::string_view makeU8View(u8view_t u8sv) {
#if HAS_CPP20_U8STRING
    return std::string_view(reinterpret_cast<const char*>(u8sv.data()), u8sv.size());
#else
    return u8sv;
#endif
}

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userInput;

    // Dynamic input collection with stream flushing
    cout << "Enter a message (e.g., Hello 🌍 C++ Unicode): " << flush;
    getline(cin, userInput);

    if (userInput.empty()) {
        userInput = "Hello 🌍 C++ Unicode! ✨";
        cout << "Empty input detected. Defaulting to sample text: \"" << userInput << "\"\n";
    }

    // =====================================================================================
    // 1. UNICODE STRING TYPES & LITERAL SUFFIXES
    // =====================================================================================
    cout << "\n================ 1. UNICODE STRING TYPES & LITERALS ================\n";

    // Standard UTF-8 string (Universal multi-byte container in C++11/14/17/20)
    std::string utf8String = userInput;

    // UTF-8 representation (Portably guarded for C++20 char8_t)
#if HAS_CPP20_U8STRING
    u8str_t explicitU8String = u8"C++20 UTF-8 String: 🚀 Innovation";
#else
    u8str_t explicitU8String = "UTF-8 String: 🚀 Innovation";
#endif

    // UTF-16 string (`char16_t` - 16-bit code units)
    std::u16string utf16String = u"UTF-16 String: ⚡ Energy";

    // UTF-32 string (`char32_t` - 32-bit fixed code units / code points)
    std::u32string utf32String = U"UTF-32 String: 🌍 Global";

    // Platform-dependent Wide String (`wchar_t`: 16-bit Win UTF-16, 32-bit POSIX UTF-32)
    std::wstring wideString = L"Wide String Representation";

    cout << "  - std::string (UTF-8 Buffer)   : \"" << utf8String << "\"\n";
    cout << "  - Portable UTF-8 String        : \"" << makeU8View(explicitU8String) << "\"\n";
    cout << "  - std::u16string Code Units    : " << utf16String.size() << " units (16-bit each)\n";
    cout << "  - std::u32string Code Points   : " << utf32String.size() << " units (32-bit each)\n";
    wcout << L"  - std::wstring Output          : \"" << wideString << L"\"\n";

    // =====================================================================================
    // 2. CODE UNITS VS CODE POINTS DEMONSTRATION
    // Explaining why `.size()` does NOT equal character/emoji count in UTF-8 or UTF-16.
    // =====================================================================================
    cout << "\n================ 2. CODE UNITS VS CODE POINTS ================\n";

    std::string asciiSample = "Hello";                  // 5 ASCII chars = 5 bytes
    std::string emojiSample = "🌍";                     // 1 Emoji = 4 UTF-8 bytes (1 Code Point)
    std::u32string emojiU32 = U"🌍";                    // 1 Emoji = 1 UTF-32 Code Point

    cout << "  - ASCII String (\"Hello\"):\n";
    cout << "    std::string .size()       : " << asciiSample.size() << " bytes\n";

    cout << "  - Emoji String (\"🌍\"):\n";
    cout << "    std::string .size() (UTF-8): " << emojiSample.size() << " bytes (Code Units)\n";
    cout << "    std::u32string .size()    : " << emojiU32.size() << " code points (32-bit scalar values)\n";

    // Inspect byte layout of 4-byte UTF-8 Emoji '🌍' (U+1F30F)
    cout << "  - Byte-by-byte UTF-8 breakdown of '🌍':\n    ";
    for (size_t i = 0; i < emojiSample.size(); ++i) {
        auto byteVal = static_cast<unsigned int>(static_cast<unsigned char>(emojiSample[i]));
        cout << "[Byte " << i << ": 0x" << hex << uppercase << byteVal << dec << "] ";
    }
    cout << "\n";

    // =====================================================================================
    // 3. UTF-32 CODE POINT ITERATION
    // Iterating over standard unicode scalar values (Code Points) using UTF-32.
    // =====================================================================================
    cout << "\n================ 3. UTF-32 CODE POINT ITERATION ================\n";

    std::u32string sampleU32 = U"C++ 🚀 2026";
    cout << "  - Inspecting Unicode Code Points (Hex Value U+XXXX) for U\"C++ 🚀 2026\":\n";

    for (size_t i = 0; i < sampleU32.size(); ++i) {
        char32_t codePoint = sampleU32[i];
        cout << "    Index [" << i << "] -> Code Point: U+" 
             << hex << uppercase << setw(4) << setfill('0') << static_cast<uint32_t>(codePoint) 
             << dec << setfill(' ') << "\n";
    }

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ UNICODE TYPES & ENCODING SUMMARY ================\n";
    cout << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| Literal / Char Type   | Container Type    | Unit Size (Bits)  | Primary Unicode Encoding / Use    |\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n"
         << "| `\"...\"` / `char`      | `std::string`     | 8 bits            | Multi-byte UTF-8 (Standard default)|\n"
         << "| `u8\"...\"` / `char8_t` | `std::u8string`   | 8 bits            | Explicit UTF-8 (C++20 standard)   |\n"
         << "| `u\"...\"` / `char16_t` | `std::u16string`  | 16 bits           | UTF-16 (Windows API / Qt / ICU)   |\n"
         << "| `U\"...\"` / `char32_t` | `std::u32string`  | 32 bits           | UTF-32 (Fixed 1 Code Point / Unit)|\n"
         << "| `L\"...\"` / `wchar_t`  | `std::wstring`    | 16 or 32 bits     | OS Wide String (16 Win / 32 POSIX)|\n"
         << "+-----------------------+-------------------+-------------------+-----------------------------------+\n";

    return 0;
}