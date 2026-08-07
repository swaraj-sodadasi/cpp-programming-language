/*
 * =====================================================================================
 * CONCEPT        : Operator Overloading in Object-Oriented Programming (Modern C++)
 * DESCRIPTION    : Comprehensive guide and production-grade executable code demonstrating
 *                  the syntax, semantics, and best practices for Operator Overloading:
 *
 *                  1. Arithmetic Operators      : Overloading `+`, `-`, `*`, `/`, and compound 
 *                                                 assignments (`+=`, `-=`, `*=`, `/=`).
 *                  2. Comparison Operators      : Equality (`==`, `!=`) and Relational (`<`, `>`, 
 *                                                 `<=`, `>=`) operators with canonical reduction.
 *                  3. Stream I/O Operators      : Friend functions for stream insertion (`<<`) 
 *                                                 and stream extraction (`>>`).
 *                  4. Increment / Decrement     : Disambiguating Prefix (`++obj`) vs Postfix 
 *                                                 (`obj++`) using dummy `int` parameters.
 *                  5. Function Call Operator    : Overloading `operator()` to turn objects into 
 *                                                 callable Functors with state.
 *                  6. Invariants & Safety       : Automatic fraction simplification (GCD), sign 
 *                                                 canonicalization, and exception handling.
 *
 * TIME COMPLEXITY  : Arithmetic / Comparison / Inc / Dec / Call : O(log(min(N,D))) for GCD reduction.
 * SPACE COMPLEXITY : Fraction Object Memory Footprint           : 16 bytes (two 64-bit integers).
 * =====================================================================================
 */

#include <iostream>
#include <numeric>
#include <stdexcept>
#include <limits>
#include <cstdint>

using namespace std;

// =====================================================================================
// CLASS DEFINITION: Fraction
// Demonstrates all canonical operator overloading patterns in C++.
// =====================================================================================
class Fraction {
private:
    int64_t numerator_;
    int64_t denominator_;

    // Private Helper: Canonical simplification using Great Common Divisor (GCD)
    void simplify() noexcept {
        if (denominator_ < 0) { // Keep denominator strictly positive
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int64_t commonDivisor = std::gcd(std::abs(numerator_), denominator_);
        if (commonDivisor > 1) {
            numerator_ /= commonDivisor;
            denominator_ /= commonDivisor;
        }
    }

public:
    // =================================================================================
    // 1. CONSTRUCTORS & LIFECYCLE
    // =================================================================================

    // Default Constructor (0 / 1)
    Fraction() noexcept : numerator_(0), denominator_(1) {}

    // Parameterized Constructor with Invariant Enforcement
    explicit Fraction(int64_t num, int64_t den = 1) : numerator_(num), denominator_(den) {
        if (den == 0) {
            throw invalid_argument("Denominator cannot be zero!");
        }
        simplify();
    }

    ~Fraction() noexcept = default;
    Fraction(const Fraction&) noexcept = default;
    Fraction& operator=(const Fraction&) noexcept = default;
    Fraction(Fraction&&) noexcept = default;
    Fraction& operator=(Fraction&&) noexcept = default;

    // Getters
    [[nodiscard]] int64_t getNumerator() const noexcept { return numerator_; }
    [[nodiscard]] int64_t getDenominator() const noexcept { return denominator_; }

    // Conversion Operator to double
    explicit operator double() const noexcept {
        return static_cast<double>(numerator_) / static_cast<double>(denominator_);
    }

    // =================================================================================
    // 2. ARITHMETIC OPERATORS (COMPOUND ASSIGNMENT & BINARY)
    // =================================================================================

    // Unary Negation (-f)
    Fraction operator-() const noexcept {
        return Fraction(-numerator_, denominator_);
    }

    // Compound Assignment Operators (Member functions modifying *this)
    Fraction& operator+=(const Fraction& rhs) noexcept {
        numerator_ = numerator_ * rhs.denominator_ + rhs.numerator_ * denominator_;
        denominator_ *= rhs.denominator_;
        simplify();
        return *this;
    }

    Fraction& operator-=(const Fraction& rhs) noexcept {
        return *this += -rhs;
    }

    Fraction& operator*=(const Fraction& rhs) noexcept {
        numerator_ *= rhs.numerator_;
        denominator_ *= rhs.denominator_;
        simplify();
        return *this;
    }

    Fraction& operator/=(const Fraction& rhs) {
        if (rhs.numerator_ == 0) {
            throw runtime_error("Division by zero fraction!");
        }
        numerator_ *= rhs.denominator_;
        denominator_ *= rhs.numerator_;
        simplify();
        return *this;
    }

    // Canonical Binary Arithmetic Operators (Symmetric non-member friends)
    friend Fraction operator+(Fraction lhs, const Fraction& rhs) noexcept {
        lhs += rhs;
        return lhs;
    }

    friend Fraction operator-(Fraction lhs, const Fraction& rhs) noexcept {
        lhs -= rhs;
        return lhs;
    }

    friend Fraction operator*(Fraction lhs, const Fraction& rhs) noexcept {
        lhs *= rhs;
        return lhs;
    }

    friend Fraction operator/(Fraction lhs, const Fraction& rhs) {
        lhs /= rhs;
        return lhs;
    }

    // =================================================================================
    // 3. COMPARISON OPERATORS (EQUALITY & RELATIONAL)
    // =================================================================================

    friend bool operator==(const Fraction& lhs, const Fraction& rhs) noexcept {
        return lhs.numerator_ == rhs.numerator_ && lhs.denominator_ == rhs.denominator_;
    }

    friend bool operator!=(const Fraction& lhs, const Fraction& rhs) noexcept {
        return !(lhs == rhs);
    }

    friend bool operator<(const Fraction& lhs, const Fraction& rhs) noexcept {
        return (lhs.numerator_ * rhs.denominator_) < (rhs.numerator_ * lhs.denominator_);
    }

    friend bool operator>(const Fraction& lhs, const Fraction& rhs) noexcept {
        return rhs < lhs;
    }

    friend bool operator<=(const Fraction& lhs, const Fraction& rhs) noexcept {
        return !(rhs < lhs);
    }

    friend bool operator>=(const Fraction& lhs, const Fraction& rhs) noexcept {
        return !(lhs < rhs);
    }

    // =================================================================================
    // 4. INCREMENT AND DECREMENT OPERATORS (PREFIX & POSTFIX)
    // =================================================================================

    // Prefix Increment (++f): Adds 1 to fraction, returns reference to *this
    Fraction& operator++() noexcept {
        numerator_ += denominator_;
        simplify();
        return *this;
    }

    // Postfix Increment (f++): Adds 1 to fraction, returns copy of old state
    Fraction operator++(int) noexcept {
        Fraction temp = *this;
        ++(*this); // Delegate to prefix increment
        return temp;
    }

    // Prefix Decrement (--f): Subtracts 1 from fraction, returns reference to *this
    Fraction& operator--() noexcept {
        numerator_ -= denominator_;
        simplify();
        return *this;
    }

    // Postfix Decrement (f--): Subtracts 1 from fraction, returns copy of old state
    Fraction operator--(int) noexcept {
        Fraction temp = *this;
        --(*this); // Delegate to prefix decrement
        return temp;
    }

    // =================================================================================
    // 5. FUNCTION CALL OPERATOR (FUNCTOR BEHAVIOR)
    // =================================================================================

    // Evaluates fraction as a scaling function over a base scalar value
    [[nodiscard]] double operator()(double scalar = 1.0) const noexcept {
        return static_cast<double>(*this) * scalar;
    }

    // Overload calculating linear function y = (N/D) * x + offset
    [[nodiscard]] double operator()(double x, double offset) const noexcept {
        return ((*this)(x)) + offset;
    }

    // =================================================================================
    // 6. STREAM I/O OPERATORS
    // =================================================================================

    friend ostream& operator<<(ostream& os, const Fraction& f) {
        if (f.denominator_ == 1) {
            os << f.numerator_;
        } else {
            os << f.numerator_ << "/" << f.denominator_;
        }
        return os;
    }

    friend istream& operator>>(istream& is, Fraction& f) {
        int64_t num = 0, den = 1;
        char slash = '\0';

        if (is >> num) {
            if (is.peek() == '/') {
                is >> slash >> den;
                if (den == 0) {
                    is.setstate(ios::failbit); // Reject divide-by-zero stream input
                    return is;
                }
            } else {
                den = 1; // Whole integer input
            }
            f = Fraction(num, den);
        }
        return is;
    }
};

// =====================================================================================
// MAIN ENTRY POINT
// =====================================================================================
int main() {
    int userInputValue = 0;

    // Dynamic input collection with stream flushing
    cout << "Enter a base integer value for operator overloading analysis (e.g., 3): " << flush;
    if (!(cin >> userInputValue) || userInputValue <= 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Defaulting base value to 3." << endl;
        userInputValue = 3;
    }

    const int64_t baseVal = static_cast<int64_t>(userInputValue);

    // =====================================================================================
    // 1. ARITHMETIC OPERATOR OVERLOADING
    // =====================================================================================
    cout << "\n================ 1. ARITHMETIC OPERATOR OVERLOADING ================\n";

    Fraction f1(baseVal, 4);      // e.g., 3/4
    Fraction f2(1, 2);            // 1/2

    cout << "  - Initial Fractions : f1 = " << f1 << " (" << static_cast<double>(f1) << ")"
         << ", f2 = " << f2 << " (" << static_cast<double>(f2) << ")\n";

    Fraction sum = f1 + f2;
    Fraction diff = f1 - f2;
    Fraction prod = f1 * f2;
    Fraction quot = f1 / f2;

    cout << "  - Binary Addition   (f1 + f2) : " << sum << "\n";
    cout << "  - Binary Subtraction(f1 - f2) : " << diff << "\n";
    cout << "  - Multiplication    (f1 * f2) : " << prod << "\n";
    cout << "  - Division          (f1 / f2) : " << quot << "\n";

    f1 += Fraction(1, 4);
    cout << "  - Compound Add      (f1 += 1/4): " << f1 << "\n";

    // =====================================================================================
    // 2. COMPARISON OPERATOR OVERLOADING
    // =====================================================================================
    cout << "\n================ 2. COMPARISON OPERATOR OVERLOADING ================\n";

    Fraction a(2, 3);
    Fraction b(4, 6); // Equal to 2/3 after GCD reduction
    Fraction c(3, 4);

    cout << "  - Fraction A: " << a << " | Fraction B: " << b << " | Fraction C: " << c << "\n";
    cout << "  - Equality   (A == B) : " << (a == b ? "TRUE" : "FALSE") << "\n";
    cout << "  - Inequality (A != C) : " << (a != c ? "TRUE" : "FALSE") << "\n";
    cout << "  - Less Than  (A < C)  : " << (a < c ? "TRUE" : "FALSE") << "\n";
    cout << "  - Greater    (C > A)  : " << (c > a ? "TRUE" : "FALSE") << "\n";

    // =====================================================================================
    // 3. INCREMENT AND DECREMENT OPERATORS (PREFIX VS POSTFIX)
    // =====================================================================================
    cout << "\n================ 3. INCREMENT & DECREMENT OPERATORS ================\n";

    Fraction incVal(1, 2);
    cout << "  - Starting Value: " << incVal << "\n";

    cout << "  - Prefix Increment (++incVal) : Returned = " << ++incVal << " | Current = " << incVal << "\n";
    cout << "  - Postfix Increment (incVal++): Returned = " << incVal++ << " | Current = " << incVal << "\n";
    cout << "  - Prefix Decrement (--incVal) : Returned = " << --incVal << " | Current = " << incVal << "\n";
    cout << "  - Postfix Decrement (incVal--): Returned = " << incVal-- << " | Current = " << incVal << "\n";

    // =====================================================================================
    // 4. FUNCTION CALL OPERATOR (FUNCTOR)
    // =====================================================================================
    cout << "\n================ 4. FUNCTION CALL OPERATOR (FUNCTOR) ================\n";

    Fraction scaleFactor(3, 2); // 1.5
    cout << "  - Fraction Functor: " << scaleFactor << "\n";

    double result1 = scaleFactor(100.0);
    double result2 = scaleFactor(100.0, 25.0); // y = (3/2)*100 + 25

    cout << "  - Functor Evaluation `scaleFactor(100.0)`         : " << result1 << "\n";
    cout << "  - Linear Evaluation  `scaleFactor(100.0, 25.0)`   : " << result2 << "\n";

    // =====================================================================================
    // 5. STREAM I/O OPERATOR OVERLOADING
    // =====================================================================================
    cout << "\n================ 5. STREAM I/O OPERATORS ================\n";

    cout << "  - Formatted Output via `operator<<`: " << Fraction(7, 3) << "\n";

    // =====================================================================================
    // 6. INVARIANT ENFORCEMENT & ERROR HANDLING
    // =====================================================================================
    cout << "\n================ 6. INVARIANT ENFORCEMENT & EXCEPTION HANDLING ================\n";

    try {
        cout << "  - Attempting constructor allocation with 0 denominator...\n";
        Fraction invalidFrac(5, 0);
    } catch (const exception& e) {
        cout << "  - [INVARIANT GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    try {
        cout << "  - Attempting division by zero fraction (0/1)...\n";
        Fraction divZero = Fraction(1, 2) / Fraction(0, 1);
    } catch (const exception& e) {
        cout << "  - [ARITHMETIC GUARD CAUGHT] Exception: \"" << e.what() << "\"\n";
    }

    // =====================================================================================
    // SUMMARY MATRIX TABLE
    // =====================================================================================
    cout << "\n================ OPERATOR OVERLOADING SUMMARY =================\n";
    cout << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Operator Category     | Overloaded Operator Syntax        | Implementation Pattern / Rule     |\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n"
         << "| Unary Arithmetic      | `Fraction operator-() const;`     | Member returning negated copy     |\n"
         << "| Compound Arithmetic   | `Fraction& operator+=(const T&);` | Member mutating *this; returns ref|\n"
         << "| Binary Arithmetic     | `friend T operator+(T, const T&);`| Non-member friend delegating to +=|\n"
         << "| Equality Comparison   | `friend bool operator==(A, B);`   | Compares normalized components    |\n"
         << "| Relational Comparison | `friend bool operator<(A, B);`    | Cross-multiplication logic        |\n"
         << "| Prefix Increment      | `Fraction& operator++();`         | Mutates *this; returns `*this&`   |\n"
         << "| Postfix Increment     | `Fraction operator++(int);`       | Saves copy; dummy `int` parameter |\n"
         << "| Stream Insertion      | `friend ostream& operator<<(..);` | Non-member friend returning `os&` |\n"
         << "| Stream Extraction     | `friend istream& operator>>(..);` | Non-member friend setting `is&`   |\n"
         << "| Function Call ()      | `double operator()(double) const;`| Turns object into callable functor|\n"
         << "+-----------------------+-----------------------------------+-----------------------------------+\n";

    return 0;
}