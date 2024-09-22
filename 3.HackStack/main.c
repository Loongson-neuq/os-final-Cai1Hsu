#include <stdio.h>
#include <stdlib.h>

const long SEARCH_RANGE = 1000;
const long FLAG = 0xdeadbeef;

int main()
{
    long where_flag_exists[SEARCH_RANGE] = {0};

    long flag_idx = rand() % SEARCH_RANGE;

    where_flag_exists[flag_idx] = FLAG;

    long target_value = rand() << 32 | rand();

    hack_func(FLAG, target_value, (SEARCH_RANGE + 10) * sizeof(long));

    int hacked = where_flag_exists[flag_idx] == target_value;

    if (hacked)
    {
        printf("Hacked!\n");
        return 0;
    }
    else
    {
        printf("Failed!\n");
        return 1;
    }
}
