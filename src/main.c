#include <stdio.h>
#include <stdint.h>
#include <shunting_yard.h>

int main(void)
{
    const char* expression = "a + 2 * 4 - 3";
    sy_initialize();

    printf("Result: %lf", sy_solve(expression));

    sy_shutdown();
    return 0;
}
