#include <stdio.h>

extern int _printf(char *format, ... );

int main()
{
    int retvalue = _printf("_printf test:\n %s\n %c\n %d\n %b\n %o\n %x\n", "simple str", '@', 12345, 130, 071, 0xFEE1DEAD);
    printf("_printf returned %d\n", retvalue);
    return 0;
}
