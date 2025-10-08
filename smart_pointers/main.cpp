#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <utility>

// ============================================================
// Struct definitions for all exercises
// ============================================================

struct foo {
    int a;
    double b;
    std::string c;

    foo(int a_value = 0, double b_value = 0.0, const std::string& c_value = "")
        : a(a_value), b(b_value), c(c_value) {}

    ~foo() {
        std::cout << "Destroying foo\n";
    }

    void print() const {
        std::cout << "a: " << a << ", b: " << b << ", c: \"" << c << "\"\n";
    }
};

struct foo_deleter {
    void operator()(foo* p) const {
        std::cout << "foo_deleter invoked\n";
        delete p;
    }
};

// ============================================================
// Exercises
// ============================================================

int main() { 
    // --------------------------------------------------------
    // EXERCISE 1: Default vs. user-provided construction
    // --------------------------------------------------------
    // TODO:
    // 1. Create one foo with default values.
    // 2. Create another foo with custom values (e.g., 42, 3.14, "hello").
    // 3. Call print() for both objects.
    foo myFoo;
    std::cout << "myFoo default values: ";
    myFoo.print();
    foo customFoo(1, 2.0, "Test String");
    std::cout << "Custom foo: ";
    customFoo.print();

    // --------------------------------------------------------
    // EXERCISE 2: Const-correct printing
    // --------------------------------------------------------
    // TODO:
    // 1. Create a const foo.
    // 2. Verify that print() can be called on it without error.


    // --------------------------------------------------------
    // EXERCISE 3: Unique ownership with std::unique_ptr
    // --------------------------------------------------------
    // TODO:
    // 1. Create a std::unique_ptr<foo> using std::make_unique.
    // 2. Call print() through the pointer.
    std::unique_ptr<foo> uniquePtrFoo = std::make_unique<foo>(2,3.0, "Hello");
    std::cout << "uniquePtrfoo: ";
    uniquePtrFoo->print();
    // --------------------------------------------------------
    // EXERCISE 4: Custom deleter with std::unique_ptr
    // --------------------------------------------------------
    // TODO:
    // 1. Use std::unique_ptr<foo, foo_deleter> to manage a foo.
    // 2. Call print().
    // 3. Observe deleter message when the pointer goes out of scope.
    std::unique_ptr<foo, foo_deleter> deletedFooPtr(new foo(3, 4.0, "String"));
    std::cout << "Custom deleter foo: ";
    deletedFooPtr->print();

    // --------------------------------------------------------
    // EXERCISE 5: Vector of std::unique_ptr<foo>
    // --------------------------------------------------------
    // TODO:
    // 1. Create a std::vector<std::unique_ptr<foo>>.
    // 2. Add several elements using std::make_unique and emplace_back().
    // 3. Loop over the vector and call print() for each element.
    foo srcFoo(1,1.0, "A");
    std::vector<std::unique_ptr<foo>> v;
    for (int i = 0; i < 5; ++i)
    {
        v.emplace_back(std::make_unique<foo>(srcFoo.a + i, static_cast<double>(srcFoo.b + i), srcFoo.c + "A"));
    }
    for (const auto &elem : v)
    {
        std::cout << "Foo pointed to by elem in vector: ";
        elem->print();
    }

    // --------------------------------------------------------
    // EXERCISE 7: Move-only semantics practice
    // --------------------------------------------------------
    // TODO:
    // 1. Create a std::unique_ptr<foo>.
    // 2. Attempt to copy it (should fail to compile).
    // 3. Move it to another std::unique_ptr and verify the first becomes null.
    std::unique_ptr<foo> moveFoo(new foo(42,42.0, "Move"));
    std::cout << "moveFoo ";
    moveFoo->print();
    // Uncomment below and rebuild to observe compilation failure.
    //std::unique_ptr<foo> newFoo = moveFoo;
    std::unique_ptr<foo> movedToFoo(std::move(moveFoo));
    if (moveFoo.get() == nullptr)
    {
        std::cout << "moveFoo was moved from so it is null\n";
    }

    // --------------------------------------------------------
    // EXERCISE 8: Erase-remove with predicates (vector of unique_ptr)
    // --------------------------------------------------------
    // TODO:
    // 1. Create a vector of std::unique_ptr<foo> with various values for 'a'.
    // 2. Use std::remove_if to remove elements where a < 3.
    // 3. Print the remaining elements to confirm.
    std::vector<std::unique_ptr<foo>> removeVec;
    for (int i = 0; i < 10; ++i)
    {
        removeVec.emplace_back(std::make_unique<foo>(i, 1.0, "Foo"));
    }
    auto new_end = std::remove_if(removeVec.begin(), removeVec.end(), [&](const auto& vecPtr) { return (vecPtr->a < 3); });
    // Erase the "removed" elements (the ones moved to the end)
    removeVec.erase(new_end, removeVec.end());
    for (const auto &elem: removeVec)
    {
        std::cout << "Remaining element after remove if: ";
        elem->print();
    }
    // --------------------------------------------------------
    // EXERCISE 9: Exception safety with factory function
    // --------------------------------------------------------
    // TODO:
    // 1. Write a function make_foo(int, double, std::string) that returns std::unique_ptr<foo>.
    // 2. Throw an exception if 'a' < 0.
    // 3. Call the function inside a try-catch block to handle the exception.
    // 4. Construct a valid foo and call print().


    // --------------------------------------------------------
    // EXERCISE 10: Move into containers vs copy into containers
    // --------------------------------------------------------
    // TODO:
    // 1. Create a std::vector<std::unique_ptr<foo>>.
    // 2. Add one element using emplace_back(std::make_unique<foo>(...)).
    // 3. Add another by constructing a unique_ptr first and moving it into the vector.
    // 4. Print all elements.


    // --------------------------------------------------------
    // EXERCISE 11: Formatting with to_string
    // --------------------------------------------------------
    // TODO:
    // 1. Implement a free function to_string(const foo&).
    // 2. Return a formatted string with all member values.
    // 3. Call it on a foo object and print the resulting string.


    // --------------------------------------------------------
    // EXERCISE 12: Borrow vs transfer ownership
    // --------------------------------------------------------
    // TODO:
    // 1. Write a function observe(const foo&) that just prints the object.
    // 2. Write a function consume(std::unique_ptr<foo>) that prints and destroys it.
    // 3. Call observe() with a stack foo.
    // 4. Call consume() with a dynamically created foo (std::make_unique).
    // 5. Verify the pointer passed to consume() becomes null after moving.

    return 0;
}

