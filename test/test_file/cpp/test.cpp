// C++ test file for tree-sitter symbol extraction
// Should extract: class, struct, enum, typedef, function, member function, template, namespace

#include <vector>
#include <string>

// ---- Namespace ----
namespace MyApp {
    namespace Detail {
        // Nested namespace
    }
}

// ---- Class ----
class Animal {
public:
    Animal(const std::string& name);
    virtual ~Animal();
    virtual void speak() const;
    static int getCount();

private:
    std::string m_name;
    static int s_count;
};

// ---- Struct ----
struct Point {
    double x;
    double y;
    Point(double x, double y);
};

// ---- Enum ----
enum Color {
    Red,
    Green,
    Blue
};

enum class Status {
    Ok,
    Error,
    Pending
};

// ---- Typedef ----
typedef unsigned long ulong;
typedef std::vector<std::string> StringVector;

// ---- Regular function ----
int add(int a, int b)
{
    return a + b;
}

void printMessage(const std::string& msg)
{
    std::cout << msg << std::endl;
}

// ---- Template function ----
template<typename T>
T max(T a, T b)
{
    return (a > b) ? a : b;
}

template<typename T>
void swap(T& a, T& b)
{
    T temp = a;
    a = b;
    b = temp;
}

// ---- Template class ----
template<typename T>
class Container {
public:
    void push(const T& value);
    T pop();
private:
    std::vector<T> m_data;
};

// ---- Member function (qualified) ----
Animal::Animal(const std::string& name)
    : m_name(name)
{
    s_count++;
}

Animal::~Animal()
{
    s_count--;
}

void Animal::speak() const
{
    std::cout << "Animal speaks" << std::endl;
}

int Animal::getCount()
{
    return s_count;
}

void Container<int>::push(const int& value)
{
    m_data.push_back(value);
}

// ---- Concept (C++20) ----
template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};
