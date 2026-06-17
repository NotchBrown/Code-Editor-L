// C test file for tree-sitter symbol extraction
// Should extract: struct, union, enum, typedef, function, macro (#define)

#include <stdio.h>
#include <stdlib.h>

// ---- Macro (constant) ----
#define MAX_SIZE 1024
#define PI 3.14159
#define APP_NAME "CodeEditorLite"

// ---- Macro (function-like) ----
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SQUARE(x) ((x) * (x))
#define PRINT_INT(x) printf("%d\n", x)

// ---- Struct ----
struct Point {
    double x;
    double y;
};

struct Rectangle {
    struct Point top_left;
    struct Point bottom_right;
    double area;
};

// ---- Union ----
union Data {
    int i;
    float f;
    char str[20];
};

union Value {
    int intValue;
    double doubleValue;
    void* pointer;
};

// ---- Enum ----
enum Color {
    RED,
    GREEN,
    BLUE
};

enum Status {
    OK = 0,
    ERROR = -1,
    PENDING = 1
};

// ---- Typedef ----
typedef unsigned long ulong;
typedef struct Point Point;
typedef int (*Callback)(int, int);

// ---- Function ----
int add(int a, int b)
{
    return a + b;
}

void printMessage(const char* msg)
{
    printf("%s\n", msg);
}

static int compare(const void* a, const void* b)
{
    return (*(int*)a - *(int*)b);
}

double calculateArea(double radius)
{
    return PI * radius * radius;
}

void processData(ulong size, Callback cb)
{
    for (ulong i = 0; i < size; i++) {
        if (cb(i, (int)size) != 0) {
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    printf("Hello from " APP_NAME "\n");
    return 0;
}
