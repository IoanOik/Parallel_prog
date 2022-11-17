#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        printf("This executable requires 3 args!\n");
        exit(EXIT_SUCCESS);
    }

    int arr_rows = atoi(argv[1]);
    int arr_cols = atoi(argv[2]);
    int arr_2_cols = atoi(argv[3]);

    srand(time(NULL));

    // first array memory allocation
    int **arr = (int **)malloc(arr_rows * sizeof(int *));
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        arr[i] = (int *)malloc(arr_cols * sizeof(int));
        for (unsigned int j = 0; j < arr_cols; j++)
        {
            arr[i][j] = rand() % 100;
        }
    }

    // second array memory allocation
    int **arr_2 = (int **)malloc(arr_cols * sizeof(int *));
    for (unsigned int i = 0; i < arr_cols; i++)
    {
        arr_2[i] = (int *)malloc(arr_2_cols * sizeof(int));
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            arr_2[i][j] = rand() % 100;
        }
    }

    // Third array memory allocation
    int **arr_3 = (int **)malloc(arr_rows * sizeof(int *));
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        arr_3[i] = (int *)malloc(arr_2_cols * sizeof(int));
    }

    int processors = omp_get_num_procs();
    omp_set_num_threads(processors);
    printf("%d\n", processors);
    omp_set_nested(1);

    double t1 = omp_get_wtime();
    #pragma omp parallel
    {
        printf("Thread in first level %d\n", omp_get_num_threads());

        #pragma omp parallel for num_threads(2)
        for (unsigned int k = 0; k < arr_2_cols; k++)
        {
            printf("Thread in second level %d\n", omp_get_num_threads());
            int master = omp_get_thread_num();
            #pragma omp task
            {
                int tid = omp_get_thread_num();
                printf("Task executed by thread %d, made by thread %d\n", tid, master);
                for (unsigned int i = 0; i < arr_rows; i++)
                {
                    int psum = 0;
                    for (unsigned int j = 0; j < arr_cols; j++)
                    {
                        psum += arr[i][j] * arr_2[j][k];
                    }
                    arr_3[i][k] = psum;
                    // printf("Partial sum of thread %d: %d\n", tid, psum);
                }
            }
        }
    }
    double t2 = omp_get_wtime();
    printf("\n\nParallel region execution time: %f\n\n", t2 - t1);
    // printing the arrays
    // printf("First array:\n");
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        for (unsigned int j = 0; j < arr_cols; j++)
        {
            // printf("%d\t", arr[i][j]);
        }
        free(arr[i]);
        // printf("\n");
    }
    free(arr);

    // printf("Second array:\n");
    for (unsigned int i = 0; i < arr_cols; i++)
    {
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            // printf("%d\t", arr_2[i][j]);
        }
        free(arr_2[i]);
        // printf("\n");
    }
    free(arr_2);

    printf("Third array:\n");
    for (unsigned int i = 0; i < arr_rows; i++)
    {
        for (unsigned int j = 0; j < arr_2_cols; j++)
        {
            printf("%d\t", arr_3[i][j]);
        }
        free(arr_3[i]);
        printf("\n");
    }
    free(arr_3);

    return 0;
}
