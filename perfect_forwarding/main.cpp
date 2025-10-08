#include <iostream>
#include <vector>
#include <utility>

struct foo {
    int a;
    double b;
    std::string c;
    std::vector<int> v;

    foo(int a_value = 0, double b_value = 0.0, const std::string& c_value = "", const std::vector<int>& v_vec = {0})
        : a(a_value), b(b_value), c(c_value), v(v_vec) {    std::cout << "Vector was constructed from l-value\n"; }

    foo(int a_value = 0, double b_value = 0.0, const std::string& c_value = "", const std::vector<int>&& v_vec = {0})
        : a(a_value), b(b_value), c(c_value), v(v_vec) {    std::cout << "Vector was constructed from r-value\n"; }
    ~foo() {
        std::cout << "Destroying foo\n";
    }

    void print() const {
        std::cout << "a: " << a << ", b: " << b << ", c: \"" << c << "\"\n";
    }
};


struct Bar
{
    template <typename... Args>
    void addFoo(Args&& ...args)
    {
        v.emplace_back(std::forward<Args>(args)...);
    }
private:
    std::vector<foo> v;
};

int main(void)
{
    // Mini perfect forwarding demo.
    Bar b;
    std::vector<int> v = {1,2,3,4};
    // addFoo method uses perfect forwarding to preserve the argument type
    // When we construct the vector in the storage of the Bar object, this will invoke
    // foo's constructor where the vector is constructed from an l-value reference 
    b.addFoo(1, 2.0, "Hello", v);

    Bar c;
    std::vector<int> v2 = {1,2,3};
    c.addFoo(2,3.0, "Hi", std::move(v2));
    return 0;
}
