#include <stdio.h>

int main(void)
{
#ifdef PLATFORM_RPI
    printf("Running on Raspberry Pi\n");
#endif

#ifdef PLATFORM_DESKTOP
    printf("Running on desktop\n");
#endif

    return 0;
}