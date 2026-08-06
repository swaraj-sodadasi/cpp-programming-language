/*
 * =====================================================================================
 * CONCEPT        : Raw String Literals in C++ (C++11)
 * DESCRIPTION    : Comprehensive guide and practical implementation of Raw String Literals:
 *                  1. What is a Raw String Literal? : A string syntax `R"(...)"` that ignores 
 *                                                    escape sequences (`\n`, `\t`, `\\`, `"`, etc.),
 *                                                    treating backslashes and quotes literally.
 *                  2. Escaped vs Raw Comparison    : Windows file paths and Regular Expressions 
 *                                                    without double-escaping ("backslash hell").
 *                  3. Embedded Code & Multilines   : Multi-line string blocks (JSON, HTML, SQL)
 *                                                    preserving exact formatting and newlines.
 *                  4. Custom Delimiters            : Handling strings containing `)"` using custom 
 *                                                    delimiters `R"delimiter(...)delimiter"`.
 *                  5. Unicode & Encoding Prefixes  : Combining raw syntax with encoding prefixes 
 *                                                    (`u8R"(...)"`, `LR"(...)"`, `uR"(...)"`, `UR"(...)"`).
 *
 * TIME COMPLEXITY  : Construction & Access: O(1) at runtime (Evaluated at compile-time).
 * SPACE COMPLEXITY : String storage footprint in `.rodata` segment: O(N) bytes.
 * =====================================================================================
 */

#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    string userAppName;

    // Dynamic input collection with stream flushing
    cout << "Enter an application name for raw path formatting (e.g., EngineCore): " << flush;
    getline(cin, userAppName);

    if (userAppName.empty()) {
        userAppName = "EngineCore";
        cout << "Empty input detected. Defaulting to: \"" << userAppName << "\"\n";
    }

    // =====================================================================================
    // 1. ESCAPED STRINGS VS RAW STRING LITERALS (FILE PATHS & REGEX)
    // =====================================================================================
    cout << "\n================ 1. FILE PATHS & REGEX (NO BACKSLASH ESCAPING) ================\n";

    // Traditional C-String requirement: double backslashes \\ for every path delimiter
    string traditionalPath = "C:\\Program Files\\" + userAppName + "\\config\\settings.json";

    // Raw String Literal syntax: R"(content)" -> Backslashes do NOT need escaping!
    string rawPath = R"(C:\Program Files\)" + userAppName + R"(\config\settings.json)";

    cout << "  - Traditional Escaped Path : " << traditionalPath << "\n";
    cout << "  - Raw String Literal Path  : " << rawPath << "\n";

    // Regular Expression Example
    // Traditional Regex (Backslash hell): "\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b"
    string_view rawRegex = R"(\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b)";
    cout << "  - Raw Regex Expression     : " << rawRegex << "\n";

    // =====================================================================================
    // 2. EMBEDDED MULTI-LINE STRINGS (JSON, HTML, SQL)
    // Preservation of tabs, newlines, and unescaped double quotes inside the string.
    // =====================================================================================
    cout << "\n================ 2. EMBEDDED MULTI-LINE BLOCKS (JSON / SQL) ================\n";

    // Embedded JSON block
    string jsonPayload = R"({
    "application": ")" + userAppName + R"(",
    "version": "2026.1",
    "status": "Active",
    "features": ["Raw Strings", "Zero Escaping", "Multi-line"]
})";

    cout << "  - Embedded Multiline JSON:\n" << jsonPayload << "\n";

    // Embedded SQL Query
    string_view sqlQuery = R"(
    SELECT user_id, user_name, user_email 
    FROM system_users 
    WHERE account_status = 'ACTIVE' 
    ORDER BY created_at DESC 
    LIMIT 10;
)";

    cout << "  - Embedded Multiline SQL Query:" << sqlQuery << "\n";

    // =====================================================================================
    // 3. CUSTOM DELIMITERS FOR STRINGS CONTAINING `)"`
    // Default raw literal delimiter is `)"`. If the text content contains `)"`, 
    // we specify a custom delimiter sequence: R"d-char-sequence(...)d-char-sequence"
    // =====================================================================================
    cout << "\n================ 3. CUSTOM DELIMITERS (HANDLING `)\"` IN CONTENT) ================\n";

    // ERROR if default syntax R"( ... )" is used: R"(The expression (a + b) * (c + d)" is valid.)"
    // SOLUTION: Use custom delimiter string like "custom" -> R"custom(...)custom"

    string_view codeSnippet = R"custom(
void calculate() {
    // Content contains raw closing paren and double quotes: )"
    std::cout << "Result: " << (10 + 20) << " )\" ";
}
)custom";

    cout << "  - Code Snippet with Custom Delimiter `R\"custom(...)custom\"`:\n" 
         << codeSnippet << "\n";

    // =====================================================================================
    // 4. UNICODE & ENCODING PREFIXES WITH RAW STRINGS
    // =====================================================================================
    cout << "\n================ 4. UNICODE & ENCODING PREFIXES ================\n";

    auto utf8Raw  = u8R"(UTF-8 Raw String: C++20 Standard)";         // char8_t[] / const char*
    auto wideRaw  = LR"(Wide Raw String: L"C:\Path\With\Wide")";    // const wchar_t*
    auto utf16Raw = uR"(UTF-16 Raw String: \u0041\u0042)";          // const char16_t*
    auto utf32Raw = UR"(UTF-32 Raw String: \U00000041)";          // const char32_t*

    cout << "  - UTF-8 Raw Literal  : " << reinterpret_cast<const char*>(utf8Raw) << "\n";
    wcout << L"  - Wide Raw Literal   : " << wideRaw << L"\n";

    // RESOLVED: Inspecting length via string_view avoids pointer sizeof warnings completely
    std::u16string_view view16(utf16Raw);
    std::u32string_view view32(utf32Raw);

    cout << "  - UTF-16 Raw Literal : Length = " << view16.length() << " code units\n";
    cout << "  - UTF-32 Raw Literal : Length = " << view32.length() << " code units\n";

    // =====================================================================================
    // SUMMARY COMPARISON TABLE
    // =====================================================================================
    cout << "\n================ RAW STRING LITERALS SUMMARY ================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Feature / Trait       | Traditional Escaped String        | Raw String Literal (`R\"(...)\"`)  |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Backslash (`\\`)       | Requires double escaping (`\\\\`)    | Treated literally as `\\`           |\n"
         << "| Quotes (`\"`)          | Requires escape (`\\\"`)           | Written directly as `\"`            |\n"
         << "| Newlines              | Requires `\\n` escape character     | Literal multi-line formatting     |\n"
         << "| Delimiter Collision   | N/A                               | Fixed via `R\"delim(...)delim\"`    |\n"
         << "| Encoding Support      | Prefix before quote (`u8\"...\"`)   | Prefix before R (`u8R\"(...)\"`)    |\n"
         << "| Runtime Overhead      | Zero (Compile-time string construction)| Zero (Compile-time literal)   |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}