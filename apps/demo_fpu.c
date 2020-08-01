#include <tea.h>

int demo_fpu(void)
{
    volatile double x = 7.0;
    volatile double y = 3.0;
    volatile double d = x / y;

    if (d == x / y)
        pr_info("Equal. div result %.18f\n", d);
    else
        pr_info("Not equal. Memory stored result %.18f\n", d);

	return 0;
}
