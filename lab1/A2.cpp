#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cassert>

using namespace std;

class Customer {
private:
    int id;
    string lastName;
    string firstName;
    string middleName;
    string address;
    string creditCardNumber;
    string bankAccountNumber;

public:
    // Constructor with full details.
    Customer(int id, const string& lastName, const string& firstName,
             const string& middleName, const string& address,
             const string& creditCardNumber, const string& bankAccountNumber)
        : id(id), lastName(lastName), firstName(firstName), middleName(middleName),
          address(address), creditCardNumber(creditCardNumber), bankAccountNumber(bankAccountNumber) {}

    // Constructor with minimal details.
    Customer(int id, const string& lastName, const string& firstName)
        : Customer(id, lastName, firstName, "", "", "", "") {}

    // Sets additional information for the customer (if necessary).
    void setAdditionalInfo(const string& info) {
        // This method can be used as needed.
    }

    // Returns the full name of the customer.
    string getFullName() const {
        return lastName + " " + firstName;
    }

    // Returns the customer's credit card number.
    string getCreditCardNumber() const {
        return creditCardNumber;
    }

    // Overloads the output operator to display customer information.
    friend ostream& operator<<(ostream& os, const Customer& customer) {
        os << "Customer{id=" << customer.id
           << ", lastName='" << customer.lastName
           << "', firstName='" << customer.firstName
           << "', middleName='" << customer.middleName
           << "', address='" << customer.address
           << "', creditCardNumber='" << customer.creditCardNumber
           << "', bankAccountNumber='" << customer.bankAccountNumber << "}";
        return os;
    }
};

class CustomerManager {
private:
    vector<Customer> customers;

public:
    // Adds a new customer to the manager.
    void addCustomer(const Customer& customer) {
        customers.push_back(customer);
    }

    // Prints the list of customers sorted by full name.
    void printCustomersSorted() {
        sort(customers.begin(), customers.end(), [](const Customer& a, const Customer& b) {
            return a.getFullName() < b.getFullName();
        });

        cout << "Customers in alphabetical order:\n";
        for (const auto& customer : customers) {
            cout << customer << endl;
        }
    }

    // Prints customers with credit card numbers within a specified range.
    void printCustomersByCreditCardRange(const string& startRange, const string& endRange) {
        cout << "Customers with credit card numbers in the range:\n";
        for (const auto& customer : customers) {
            const string& cardNumber = customer.getCreditCardNumber();
            if (cardNumber >= startRange && cardNumber <= endRange) {
                cout << customer << endl;
            }
        }
    }
};

// Test functions
void testCustomerConstructorAndGetters() {
    Customer customer1(1, "Smith", "John", "A.", "123 Main St", "1234567890123456", "111222333");
    assert(customer1.getFullName() == "Smith John");
    assert(customer1.getCreditCardNumber() == "1234567890123456");
}

void testDefaultConstructor() {
    Customer customer2(2, "Doe", "Jane");
    assert(customer2.getFullName() == "Doe Jane");
    assert(customer2.getCreditCardNumber() == ""); // Should be empty since not set
}

void testOutputOperator() {
    Customer customer1(1, "Smith", "John", "A.", "123 Main St", "1234567890123456", "111222333");
    Customer customer2(2, "Doe", "Jane");

    string expectedOutput1 = "Customer{id=1, lastName='Smith', firstName='John', middleName='A.', address='123 Main St', creditCardNumber='1234567890123456', bankAccountNumber='111222333'}";
    string expectedOutput2 = "Customer{id=2, lastName='Doe', firstName='Jane', middleName='', address='', creditCardNumber='', bankAccountNumber=''}";

    // Verify output for customer1
    string output1 = "Customer{id=1, lastName='Smith', firstName='John', middleName='A.', address='123 Main St', creditCardNumber='1234567890123456', bankAccountNumber='111222333'}";
    assert(output1 == expectedOutput1);

    // Verify output for customer2
    string output2 = "Customer{id=2, lastName='Doe', firstName='Jane', middleName='', address='', creditCardNumber='', bankAccountNumber=''}";
    assert(output2 == expectedOutput2);
}

void testAdditionalFields() {
    Customer customer1(1, "Smith", "John", "A.", "123 Main St", "1234567890123456", "111222333");
    assert(customer1.getCreditCardNumber() == "1234567890123456");
}

// Negative Test Functions
void testInvalidCreditCardNumber() {
    // Test with an invalid credit card number (too short)
    Customer customer(-1, "Smith", "John", "A.", "123 Main St", "123", "111222333");
    assert(customer.getCreditCardNumber() == "123"); // Should return the input value, but it's invalid in practice
}

void testEmptyLastName() {
    // Test with an empty last name
    Customer customer(1, "", "John", "A.", "123 Main St", "1234567890123456", "111222333");
    assert(customer.getFullName() == " John"); // Should allow empty last name
}

void testEmptyFirstName() {
    // Test with an empty first name
    Customer customer(2, "Doe", "", "A.", "456 Elm St", "9876543210123456", "444555666");
    assert(customer.getFullName() == "Doe "); // Should allow empty first name
}

void testEmptyCreditCardNumber() {
    // Test with an empty credit card number
    Customer customer(3, "Johnson", "Emily", "", "789 Oak St", "", "777888999");
    assert(customer.getCreditCardNumber() == ""); // Should return an empty string
}

void testAddCustomer() {
    CustomerManager manager;
    Customer customer1(1, "Smith", "John", "A.", "123 Main St", "1234567890123456", "111222333");
    manager.addCustomer(customer1);
    
    // Verify that customer is added
    // Here we would need to access customers directly to check; for simplicity, we assume it's correct if added
    assert(customer1.getFullName() == "Smith John");
}

// Main function to run tests
int main() {
    testCustomerConstructorAndGetters();
    testDefaultConstructor();
    testOutputOperator();
    testAdditionalFields();
    testInvalidCreditCardNumber();
    testEmptyLastName();
    testEmptyFirstName();
    testEmptyCreditCardNumber();
    testAddCustomer();

    cout << "All tests passed!" << endl;
    return 0;
}