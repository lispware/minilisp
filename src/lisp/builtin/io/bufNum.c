#include <lisp.h>

void bufNum(char *b, word n)
{
    int i = 0, k = 0;
    char buf[BITS];

    b[0] = 0;

    if (n < 0)
    {
        b[k++] = '-';
        n*=-1;
    }

    if (n == 0)
    {
        b[0] = '0';
        b[1] = 0;
        return;
    }
    
    while (n)
    {
        int x = n % 10;
        n = n / 10;
        buf[i++]='0' + x;
    }

    for(int j = i - 1; j >= 0; j--)
    {
        b[k++]=buf[j];
        b[k]=0;
    }

}
