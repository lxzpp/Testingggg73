#include <stdio.h>

void test_func() {
    printf("In test_func\n");
}

int main() {
    int a = 5;
    int b = 10;
    int result = a + b;

    printf("Result: %d\n", result);

    test_func();

    return 0;
}
