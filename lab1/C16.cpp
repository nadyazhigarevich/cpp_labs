#include <iostream>
#include <set>
#include <cassert>
#include <algorithm>
#include <vector>

using namespace std;

class CharSet {
private:
    set<char> elements; // Set of characters

public:
    // Add an element to the set
    void add(char element) {
        elements.insert(element);
    }

    // Remove an element from the set
    void remove(char element) {
        elements.erase(element);
    }

    // Check membership of an element in the set
    bool contains(char element) const {
        return elements.find(element) != elements.end();
    }

    // Return the union of this set and another set
    CharSet unionWith(const CharSet& other) const {
        CharSet result;
        result.elements.insert(elements.begin(), elements.end());
        result.elements.insert(other.elements.begin(), other.elements.end());
        return result;
    }

    // Return the intersection of this set and another set
    CharSet intersectionWith(const CharSet& other) const {
        CharSet result;
        for (char element : elements) {
            if (other.contains(element)) {
                result.add(element);
            }
        }
        return result;
    }

    // Return the difference of this set and another set
    CharSet differenceWith(const CharSet& other) const {
        CharSet result;
        for (char element : elements) {
            if (!other.contains(element)) {
                result.add(element);
            }
        }
        return result;
    }

    // Overloading + operator for union
    CharSet operator+(const CharSet& other) const {
        return unionWith(other);
    }

    // Overloading - operator for difference
    CharSet operator-(const CharSet& other) const {
        return differenceWith(other);
    }

    // Overloading * operator for intersection
    CharSet operator*(const CharSet& other) const {
        return intersectionWith(other);
    }

    // Overloading [] operator for indexing elements
    char operator[](size_t index) const {
        if (index >= elements.size()) {
            throw out_of_range("Index out of range.");
        }
        auto it = elements.begin();
        advance(it, index);
        return *it;
    }

    // Overloading = operator for assignment
    CharSet& operator=(const CharSet& other) {
        if (this != &other) {
            elements = other.elements;
        }
        return *this;
    }

    // Display the contents of the set
    void display() const {
        cout << "{ ";
        for (char element : elements) {
            cout << element << " ";
        }
        cout << "}" << endl;
    }

    // Create a set with elements that are in only one of the two sets
    static CharSet exclusiveElements(const CharSet& set1, const CharSet& set2) {
        CharSet result;
        for (char element : set1.elements) {
            if (!set2.contains(element)) {
                result.add(element);
            }
        }
        for (char element : set2.elements) {
            if (!set1.contains(element)) {
                result.add(element);
            }
        }
        return result;
    }
};

// Class to manage pairs of CharSet objects
class SetManager {
public:
    // Create a new set from a pair of CharSet objects with exclusive elements
    static CharSet createExclusiveSet(const CharSet& set1, const CharSet& set2) {
        return CharSet::exclusiveElements(set1, set2);
    }
};

// Test adding elements and checking membership
void testAddAndContains() {
    CharSet set;
    set.add('a');
    set.add('b');

    assert(set.contains('a') == true);
    assert(set.contains('b') == true);
    assert(set.contains('c') == false);
}

// Test removing elements from the set
void testRemove() {
    CharSet set;
    set.add('a');
    set.add('b');
    set.remove('a');

    assert(set.contains('a') == false);
    assert(set.contains('b') == true);
}

// Test union operation between two sets
void testUnion() {
    CharSet setA;
    CharSet setB;
    setA.add('a');
    setA.add('b');
    setB.add('b');
    setB.add('c');

    CharSet unionSet = setA + setB; // A ∪ B
    assert(unionSet.contains('a') == true);
    assert(unionSet.contains('b') == true);
    assert(unionSet.contains('c') == true);
}

// Test intersection operation between two sets
void testIntersection() {
    CharSet setA;
    CharSet setB;
    setA.add('a');
    setA.add('b');
    setB.add('b');
    setB.add('c');

    CharSet intersectionSet = setA * setB; // A ∩ B
    assert(intersectionSet.contains('b') == true);
    assert(intersectionSet.contains('a') == false);
    assert(intersectionSet.contains('c') == false);
}

// Test difference operation between two sets
void testDifference() {
    CharSet setA;
    CharSet setB;
    setA.add('a');
    setA.add('b');
    setB.add('b');
    setB.add('c');

    CharSet differenceSet = setA - setB; // A - B
    assert(differenceSet.contains('a') == true);
    assert(differenceSet.contains('b') == false);
    assert(differenceSet.contains('c') == false);
}

// Test creating a set with exclusive elements
void testExclusiveElements() {
    CharSet setA;
    CharSet setB;
    setA.add('a');
    setA.add('b');
    setB.add('b');
    setB.add('c');

    CharSet exclusiveSet = SetManager::createExclusiveSet(setA, setB); // Exclusive elements
    assert(exclusiveSet.contains('a') == true);
    assert(exclusiveSet.contains('b') == false);
    assert(exclusiveSet.contains('c') == true);
}

// Test accessing elements by index
void testIndexing() {
    CharSet set;
    set.add('a');
    set.add('b');
    set.add('c');

    assert(set[0] == 'a'); // First element
    assert(set[1] == 'b'); // Second element
    assert(set[2] == 'c'); // Third element

    try {
        set[3]; // Out of range
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test assignment operator functionality
void testAssignment() {
    CharSet setA;
    setA.add('a');
    setA.add('b');

    CharSet setB;
    setB = setA; // Assignment

    assert(setB.contains('a') == true);
    assert(setB.contains('b') == true);
}

// Test removal of a non-existent element
void testRemoveNonExistentElement() {
    CharSet set;
    set.add('a');
    set.remove('b'); // Removing a non-existent element

    // Ensure set still contains 'a'
    assert(set.contains('a') == true);
    assert(set.contains('b') == false);
}

// Test indexing with an out-of-bounds index
void testIndexingOutOfBounds() {
    CharSet set;
    set.add('a');
    set.add('b');

    try {
        char elem = set[2]; // Out of bounds
        assert(false); // Should not reach here
    } catch (const out_of_range& e) {
        assert(true); // Expected exception
    }
}

// Test intersection with an empty set
void testIntersectionWithEmptySet() {
    CharSet setA;
    CharSet setB;

    setA.add('a');
    setA.add('b');

    CharSet intersectionSet = setA * setB; // A ∩ B where B is empty
    assert(intersectionSet.contains('a') == false);
    assert(intersectionSet.contains('b') == false);
}

// Test union with an empty set
void testUnionWithEmptySet() {
    CharSet setA;
    CharSet setB;

    setA.add('a');
    setA.add('b');

    CharSet unionSet = setA + setB; // A ∪ B where B is empty
    assert(unionSet.contains('a') == true);
    assert(unionSet.contains('b') == true);
}

// Test difference with an empty set
void testDifferenceWithEmptySet() {
    CharSet setA;
    CharSet setB;

    setA.add('a');
    setA.add('b');

    CharSet differenceSet = setA - setB; // A - B where B is empty
    assert(differenceSet.contains('a') == true);
    assert(differenceSet.contains('b') == true);
}

int main() {
    testAddAndContains();
    testRemove();
    testUnion();
    testIntersection();
    testDifference();
    testExclusiveElements();
    testIndexing();
    testAssignment();
    testRemoveNonExistentElement();
    testIndexingOutOfBounds();
    testIntersectionWithEmptySet();
    testUnionWithEmptySet();
    testDifferenceWithEmptySet();

    cout << "All tests passed!" << endl;
    return 0;
}