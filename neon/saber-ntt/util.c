#include <arm_neon.h>
#include <stdio.h>
#include "util.h"
#include "params.h"

void print_vector(int16x8x4_t a, const char *string)
{
    for (int i = 0; i < 4; i += 2)
    {
        for (int j = 0; j < 8; j++)
        {
            printf("%d, ", (int16_t)(a.val[i][j] & 0xffff));
        }
        printf("\\\\ %s", string);
        printf("\n");
        for (int j = 0; j < 8; j++)
        {
            printf("%d, ", (int16_t)(a.val[i + 1][j] & 0xffff));
        }
        printf("\\\\ %s", string);
        printf("\n");
    }
}

void print_array(int16_t *a, int bound, const char *string)
{
    printf("%s = [", string);
    for (int i = 0; i < bound; i++)
    {
        printf("%d, ", a[i]);
    }
    printf("]\n");
}


int compare_array(int16_t *a, int16_t *b)
{
    int count = 0;
    for (int i = 0; i < 256; i++)
    {
        if ( (a[i] - b[i]) % KEM_Q)
        {
            printf("%d: %d !=  %d\n", i, a[i], b[i]);
            count++;
        }
        if (count > 8)
        {
            printf("ERROR\n");
            return 1;
        }
    }
    if (count)
    {
        printf("ERROR\n");
        return 1;
    }

    return 0;
}
