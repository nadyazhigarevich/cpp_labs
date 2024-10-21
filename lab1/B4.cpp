#include <iostream>
#include <vector>
#include <cassert>
#include <iomanip>
#include <stdexcept>

using namespace std;

class Polynomial {
private:
    vector<double> coefficients; // Coefficients of the polynomial

public:
    // Constructor to initialize a polynomial of degree n
    Polynomial(int degree) : coefficients(degree + 1, 0.0) {}

    // Set coefficient for a specific degree
    void setCoefficient(int degree, double value) {
        if (degree < 0 || degree >= coefficients.size()) {
            throw out_of_range("Degree is out of range.");
        }
        coefficients[degree] = value;
    }
    
    // Get coefficient for a specific degree
    double getCoefficient(int pos) {
        return this->coefficients.at(pos);
    }

    // Addition of two polynomials
    Polynomial operator+(const Polynomial& other) const {
        int maxDegree = max(coefficients.size(), other.coefficients.size());
        Polynomial result(maxDegree - 1);
        for (int i = 0; i < maxDegree; ++i) {
            double thisCoeff = (i < coefficients.size()) ? coefficients[i] : 0;
            double otherCoeff = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result.setCoefficient(i, thisCoeff + otherCoeff);
        }
        return result;
    }

    // Subtraction of two polynomials
    Polynomial operator-(const Polynomial& other) const {
        int maxDegree = max(coefficients.size(), other.coefficients.size());
        Polynomial result(maxDegree - 1);
        for (int i = 0; i < maxDegree; ++i) {
            double thisCoeff = (i < coefficients.size()) ? coefficients[i] : 0;
            double otherCoeff = (i < other.coefficients.size()) ? other.coefficients[i] : 0;
            result.setCoefficient(i, thisCoeff - otherCoeff);
        }
        return result;
    }

    // Multiplication of two polynomials
    Polynomial operator*(const Polynomial& other) const {
        int resultDegree = coefficients.size() + other.coefficients.size() - 2;
        Polynomial result(resultDegree);
        for (int i = 0; i < coefficients.size(); ++i) {
            for (int j = 0; j < other.coefficients.size(); ++j) {
                result.setCoefficient(i + j, result.coefficients[i + j] + coefficients[i] * other.coefficients[j]);
            }
        }
        return result;
    }

    // Division of two polynomials (simple case, no remainder)
    Polynomial operator/(const Polynomial& other) const {
        if (other.coefficients.size() == 1 && other.coefficients[0] == 0) {
            throw domain_error("Division by zero polynomial.");
        }
        
        int resultDegree = coefficients.size() - other.coefficients.size() + 1;
        Polynomial result(resultDegree);
        Polynomial remainder = *this;

        // Perform polynomial long division
        for (int i = 0; i <= resultDegree; ++i) {
            double coeff = remainder.coefficients[remainder.coefficients.size() - 1] / other.coefficients[other.coefficients.size() - 1];
            result.setCoefficient(resultDegree - i, coeff);
            Polynomial temp = other * Polynomial(resultDegree - i);
            remainder = remainder - temp;
        }
        return result;
    }

    // Display the polynomial
    void display() const {
        for (int i = coefficients.size() - 1; i >= 0; --i) {
            if (coefficients[i] != 0) {
                if (i != coefficients.size() - 1) {
                    cout << " + ";
                }
                cout << fixed << setprecision(2) << coefficients[i] << "x^" << i;
            }
        }
        cout << endl;
    }
};

// Test methods

// Test setting coefficients for a polynomial
void testSetCoefficient() {
    Polynomial p(2);
    p.setCoefficient(2, 2);
    p.setCoefficient(1, 3);
    p.setCoefficient(0, 1);
    assert(p.getCoefficient(2) == 2);
    assert(p.getCoefficient(1) == 3);
    assert(p.getCoefficient(0) == 1);
}

// Test addition of two polynomials
void testAddition() {
    Polynomial p1(2);
    p1.setCoefficient(2, 2);
    p1.setCoefficient(1, 3);
    p1.setCoefficient(0, 1);

    Polynomial p2(2);
    p2.setCoefficient(2, 4);

    Polynomial sum = p1 + p2; // Should be 6x^2 + 3x + 1
    assert(sum.getCoefficient(2) == 6);
    assert(sum.getCoefficient(1) == 3);
    assert(sum.getCoefficient(0) == 1);
}

// Test subtraction of two polynomials
void testSubtraction() {
    Polynomial p1(2);
    p1.setCoefficient(2, 2);
    p1.setCoefficient(1, 3);
    p1.setCoefficient(0, 1);

    Polynomial p2(2);
    p2.setCoefficient(2, 4);

    Polynomial diff = p1 - p2; // Should be -2x^2 + 3x + 1
    assert(diff.getCoefficient(2) == -2);
    assert(diff.getCoefficient(1) == 3);
    assert(diff.getCoefficient(0) == 1);
}

// Test multiplication of two polynomials
void testMultiplication() {
    Polynomial p1(2);
    p1.setCoefficient(2, 2);
    p1.setCoefficient(1, 3);
    p1.setCoefficient(0, 1);

    Polynomial p2(1);
    p2.setCoefficient(1, 2);
    p2.setCoefficient(0, 2);

    Polynomial product = p1 * p2; // Should be 4x^3 + 10x^2 + 8x + 2
    assert(product.getCoefficient(3) == 4);
    assert(product.getCoefficient(2) == 10);
    assert(product.getCoefficient(1) == 8);
    assert(product.getCoefficient(0) == 2);
}

// Test division by zero polynomial
void testDivisionByZero() {
    Polynomial p1(2);
    p1.setCoefficient(2, 2);
    
    try {
        Polynomial result = p1 / Polynomial(0);
        assert(false); // Should not reach here
    } catch (const domain_error& e) {
        assert(true); // Expected exception
    }
}

// Test setting coefficient out of range
void testOutOfRange() {
    Polynomial p(2);
    try {
        p.setCoefficient(3, 5); // Out of range
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test setting coefficient out of range
void testOutOfRangeSetCoefficient() {
    Polynomial p(2);
    try {
        p.setCoefficient(3, 5); // Out of range
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test getting coefficient out of range
void testOutOfRangeGetCoefficient() {
    Polynomial p(2);
    try {
        double coeff = p.getCoefficient(3); // Out of range
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test setting coefficient for a negative degree
void testNegativeDegree() {
    Polynomial p(2);
    try {
        p.setCoefficient(-1, 5); // Negative degree
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test addition with a zero polynomial
void testAdditionWithNullPolynomial() {
    Polynomial p1(2);
    p1.setCoefficient(2, 1);
    Polynomial p2(0); // Zero polynomial

    Polynomial sum = p1 + p2; // Should be equal to p1
    assert(sum.getCoefficient(2) == 1);
    assert(sum.getCoefficient(1) == 0);
    assert(sum.getCoefficient(0) == 0);
}

// Test subtraction with a zero polynomial
void testSubtractionWithNullPolynomial() {
    Polynomial p1(2);
    p1.setCoefficient(2, 1);
    Polynomial p2(0); // Zero polynomial

    Polynomial diff = p1 - p2; // Should be equal to p1
    assert(diff.getCoefficient(2) == 1);
    assert(diff.getCoefficient(1) == 0);
    assert(diff.getCoefficient(0) == 0);
}

int main() {
    testSetCoefficient();
    testAddition();
    testSubtraction();
    testMultiplication();
    testDivisionByZero();
    testOutOfRange();

    testOutOfRangeSetCoefficient();
    testOutOfRangeGetCoefficient();
    testNegativeDegree();
    testAdditionWithNullPolynomial();
    testSubtractionWithNullPolynomial();
    
    cout << "All tests passed!" << endl;
    return 0;
}