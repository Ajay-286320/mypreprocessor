#include <stdio.h>
#include "myheader.h"

#define PI 3.14
#define SQUARE(x) ((x) * (x))

int main(void)
{
    // Example single-line comment
    int value = SQUARE(5);
    float area = PI * SQUARE(2);

    display();
    printf("value = %d, area = %f\\n", value, area);
    return 0;
}
