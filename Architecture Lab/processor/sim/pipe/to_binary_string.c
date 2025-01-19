#include <stdio.h>

typedef long word_t;

word_t arr[4];
unsigned char buff[4][8];

/* $begin to_binary_string */
/*
 * to_binary_string - Convert len values in arr to binary string and
 * store in buff. Return sum of the values
 */
word_t to_binary_string(word_t *arr, unsigned char **buff, word_t len)
{
    word_t sum = 0;
    word_t val;
    int pow = 128;

    while (len > 0) {
        val = *arr++;
        sum += val;
        unsigned char * temp = buff;
        for(pow = 128; pow > 0; pow >>=1){
        	if (val >= pow) {
        		*temp++ = '1';
        		val -= pow;
        	}
        	else
        		*temp++ = '0';
        }
        len--;
        buff++;
    }

    return sum;
}
/* $end to_binary_string */

int main()
{
    word_t i, count;

    arr[0] = 3;
    arr[1] = 12;
    arr[2] = 48;
    arr[3] = 192;
    count = to_binary_string(arr, buff, 4);
    printf ("sum=%ld\n", count);
    for (i = 0; i < 4; i++)
        printf("%.8s\n", buff[i]);

    return 0;
}


