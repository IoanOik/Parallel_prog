#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#include <immintrin.h>
#define BLOCK_SIZE 8

float vector_sum(__m256 *vector, int size);
void print_to_file(__m256 *vector, int size, int tid);
void print_to_file_new(__m256 *vector, int size);

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("You have to execute this file with 3 arguments from the terminal.\n1) First's array rows quantity\n2) First's array columns quantity\n3) Second's array columns quantity\n");
        exit(EXIT_SUCCESS);
    }

    int arr_rows = atoi(argv[1]);
    int arr_cols = atoi(argv[2]);
    int arr_2_cols = atoi(argv[3]);

    srand(time(NULL));

    // first array memory allocation
    float **arr = (float **)malloc(arr_rows * sizeof(float *));
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        //arr[i] = (float *)malloc(arr_cols * sizeof(float));
        posix_memalign((void *)&arr[i], BLOCK_SIZE * sizeof(float), arr_cols * sizeof(float));
        for (unsigned int j = 0; j < arr_cols; j++)
        {
            arr[i][j] = (float)(rand() % 100);
        }
    }

    // second array memory allocation
    float **arr_2 = (float **)malloc(arr_cols * sizeof(float *));
    for (unsigned int i = 0; i < arr_cols; i++)
    {
        arr_2[i] = (float *)malloc(arr_2_cols * sizeof(float));
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            arr_2[i][j] = (float)(rand() % 100);
        }
    }

    // Third array memory allocation
    float **arr_3 = (float **)calloc(arr_rows, sizeof(float *));
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        arr_3[i] = (float *)calloc(arr_2_cols, sizeof(float));
    }

    int processors = omp_get_num_procs();
    omp_set_num_threads(processors);
    printf("%d\n", processors);

    double t1 = omp_get_wtime();

    #pragma omp parallel
    {
        #pragma omp single
        {
            for (unsigned int k = 0; k < arr_2_cols; k++)
            {
                #pragma omp task
                {
                    __m256 subvector_A, subvector_B, partial_mul;
                    float partial_res = 0;
                    float *column_B = NULL;// (float *)malloc(arr_cols * sizeof(float));
                    posix_memalign((void *)&column_B, BLOCK_SIZE * sizeof(float), arr_cols * sizeof(float));

                    // getting the column from the second array
                    for (unsigned int i = 0; i < arr_cols; i++)
                    {
                        column_B[i] = arr_2[i][k];
                    }

                    for (unsigned int i = 0; i < arr_cols - BLOCK_SIZE + 1; i += BLOCK_SIZE)
                    {
                        subvector_B = _mm256_load_ps(&column_B[i]);
                        for (unsigned int j = 0; j < arr_rows; j++)
                        {
                            subvector_A = _mm256_load_ps(&arr[j][i]);
                            partial_mul = _mm256_setzero_ps();
                            partial_mul = _mm256_fmadd_ps(subvector_A, subvector_B, partial_mul);
                            partial_res = vector_sum(&partial_mul, BLOCK_SIZE);
                            //#pragma omp atomic
                            arr_3[j][k] += partial_res;
                            // print_to_file(&subvector_A, BLOCK_SIZE, omp_get_thread_num());
                            // print_to_file_new(&subvector_B, BLOCK_SIZE);
                        }
                    }
                    free(column_B);
                }
            }
        }
    }
    double t2 = omp_get_wtime();

    int remain_rows_B = arr_cols % BLOCK_SIZE;

    if (remain_rows_B > 0)
    {
        for (unsigned int i = 0; i < arr_rows; i++)
        {
            for (unsigned int j = 0; j < arr_2_cols; j++)
            {
                for (unsigned int k = arr_cols - remain_rows_B; k < arr_cols; k++)
                {
                    arr_3[i][j] += arr[i][k] * arr_2[k][j];
                }
            }
        }
    }

    printf("\n\nParallel region execution time: %f\n\n", t2 - t1);

    // printing the arrays
    //printf("First array:\n");
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        for (unsigned int j = 0; j < arr_cols; j++)
        {
            //printf("%0.0f\t", arr[i][j]);
        }
        free(arr[i]);
        //printf("\n");
    }
    free(arr);

    //printf("Second array:\n");
    for (unsigned int i = 0; i < arr_cols; i++)
    {
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            //printf("%0.0f\t", arr_2[i][j]);
        }
        free(arr_2[i]);
        //printf("\n");
    }
    free(arr_2);

    //printf("Third array:\n");
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            //printf("%0.0f\t", arr_3[i][j]);
        }
        free(arr_3[i]);
        //printf("\n");
    }
    free(arr_3);

    return 0;
}

float vector_sum(__m256 *vector, int size)
{
    float temp[size];
    float sum = 0;
    memcpy(temp, vector, sizeof(float) * size);
    for (int i = 0; i < size; i++)
    {
        sum += temp[i];
    }
    return sum;
}

void print_to_file(__m256 *vector, int size, int tid)
{
    float temp[size];
    FILE *fp;
    memcpy(temp, vector, sizeof(float) * size);
    fp = fopen("A.txt", "a");
    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%0.0f\t", temp[i]);
    }
    fprintf(fp, "%d\n", tid);
    fclose(fp);
}

void print_to_file_new(__m256 *vector, int size)
{
    float temp[size];
    FILE *fp;
    memcpy(temp, vector, sizeof(float) * size);
    fp = fopen("B.txt", "a");
    for (int i = 0; i < size; i++)
    {
        fprintf(fp, "%0.0f\t", temp[i]);
    }
    fprintf(fp, "\n");
    fclose(fp);
}