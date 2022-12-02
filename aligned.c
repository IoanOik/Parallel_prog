#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>
#include <string.h>
#define N 10
int main(int argc, char const *argv[])
{
    float *arr = NULL;
    posix_memalign((void *)&arr, 4 * sizeof(float), N * sizeof(float));
    for (unsigned int j = 0; j < N; j++)
    {
        arr[j] = (float)(rand() % 100);
    }

    float **arr_2 = NULL;
    //posix_memalign((void *)&arr_2, 4 * sizeof(float *), N * sizeof(float *));
    arr_2 = (float **)malloc(N * sizeof(float *));
    for (unsigned int i = 0; i < N; i++)
    {
        // arr_2[i] = malloc(N * sizeof(float));
        posix_memalign((void *)&arr_2[i], 8 * sizeof(float), N * sizeof(float));
        for (unsigned int j = 0; j < N; j++)
        {
            arr_2[i][j] = (float)(rand() % 100);
        }
    }
    //__m256 vector = _mm256_loadu_ps(&(arr_2[0][0]));
    __m256 vector = _mm256_load_ps(&arr_2[0][0]);
    puts("hi");

    float v[8];
    memcpy(v, &vector, sizeof(vector));
    for (size_t i = 0; i < 8; i++)
    {
        printf("%0.0f\t", v[i]);
    }
    puts("\n\n");
    for (size_t i = 0; i < N; i++)
    {
        printf("%0.0f\t", arr[i]);
    }
    free(arr);
    puts("\n");
    for (size_t i = 0; i < N; i++)
    {
        for (size_t j = 0; j < N; j++)
        {
            printf("%0.0f\t", arr_2[i][j]);
        }
        printf("\n");
    }

    printf("Base adress of arr_2: %p\n", arr_2);
    // printf("adress of arr_2[1]: %p\n", &arr_2[1]);
    for (unsigned int i = 0; i < N; i++)
    {
        free(arr_2[i]);
    }
    free(arr_2);
    return 0;
}
