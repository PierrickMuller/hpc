/* Pierrick Muller
 * HPC - 20.02.20
 * array_util.c
 *
 *
 * C code for array API
 */

 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "array_util.h"

/* Allocate an array of size "len" and fill it
 * with random data.
 * Return the array pointer */
uint64_t *array_init(const size_t len){
        uint64_t *result_array;
        size_t i;
        time_t t;

        /* Intializes random number generator */
        srand((unsigned) time(&t));

        result_array = (uint64_t *)malloc(sizeof(uint64_t)*len);
        if(result_array == NULL)
        {
                return 0;
        }
        for(i = 0; i < len; i++)
        {
                result_array[i] = (uint64_t)rand();
        }
        return result_array;
}

/* Liberate array memory */
void array_clear(uint64_t *data){
        free(data);
}

/* Arrange a array in increasing order of value */
/*Version 2 , with less memory access */
/*void array_sort(uint64_t *data, const size_t len){
        size_t i,j;
        uint64_t tmp,tmp2;

        for (i=1; i < len; i++) {
                j = i;
                tmp = data[j];
                tmp2 = data[j-1];
                while (j > 0 && tmp2 > tmp) {
                        data[j] = tmp2;
                        data[j-1] = tmp;
                        j--;
                        tmp = data[j];
                        tmp2 = data[j-1];
                }
        }
}*/

/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len){
        size_t i,j;
        uint64_t tmp;

        for (i=1; i < len; i++) {
                j = i;
                while (j > 0 && data[j-1] > data[j]) {
                        tmp = data[j];
                        data[j] = data[j-1];
                        data[j-1] = tmp;
                        j--;
                }
        }
}
