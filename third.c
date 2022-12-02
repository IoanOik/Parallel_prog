#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <omp.h>
#define GENERATION_SIZE 100
#define TARGET_LENGTH 5

typedef struct individual
{
    char chromosome[TARGET_LENGTH];
    int score;
} individual;

const char GENES[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890,.-;:_!'|#&/()=?@${[]}";
const char TARGET[] = "AAAAA";

char random_gene();
void create_individual(individual *data);
void mate(individual *parent, individual *parent2, individual *child);
int cal_score(char *chromosome);
int compare_scores(const void *a, const void *b);

int main(int argc, char const *argv[])
{
    double t1 = omp_get_wtime();
    srand(time(NULL));

    individual population[GENERATION_SIZE];
    bool found = false;
    int gen = 0;
    for (unsigned int i = 0; i < GENERATION_SIZE; i++)
    {
        create_individual(&population[i]);
        // printf("%s  %d\n", population[i].chromosome, population[i].score);
    }

    while (gen < 100 && !found)
    {
        qsort(population, GENERATION_SIZE, sizeof(population[0]), compare_scores);
        if (population[0].score == strlen(TARGET))
        {
            found = true;
            break;
        }
        // build new generation
        individual new_generation[GENERATION_SIZE];

        // The 10% elite goes straight to the next generation
        int n = (10 * GENERATION_SIZE) / 100;
        for (unsigned int i = 0; i < n; i++)
        {
            new_generation[i] = population[i];
        }

        // the 70% best will be allowed to mate
        int c = (90 * GENERATION_SIZE) / 100;
        int percent = (70 * GENERATION_SIZE) / 100;
        for (unsigned int i = n - 1; i < c + n; i++)
        {
            int index_of_p1 = rand() % percent;
            int index_of_p2 = rand() % percent;
            mate(&population[index_of_p1], &population[index_of_p2], &new_generation[i]);
            // new_generation[i].score = cal_score(new_generation[i].chromosome);
        }
        memcpy(population, new_generation, sizeof(population));
        // printf("%s %d %d\n", population[0].chromosome, population[0].score, gen);
        gen++;
    }
    double t2 = omp_get_wtime();
    float percent_total = (100 * population[0].score) / TARGET_LENGTH;
    printf("organism: %s score: %d percent: %0.2f generation: %d\n", population[0].chromosome, population[0].score, percent_total, gen);
    printf("\n\nExecution time: %f\n\n", t2 - t1);
    return 0;
}

char random_gene()
{
    int index = rand() % strlen(GENES);
    return GENES[index];
}

void create_individual(individual *data)
{
    int length = strlen(TARGET);
    char chromosome[length];
    for (unsigned int i = 0; i < length; i++)
    {
        chromosome[i] = random_gene();
        if (i == length - 1)
        {
            chromosome[i + 1] = '\0';
        }
    }
    strcpy(data->chromosome, chromosome);
    data->score = cal_score(data->chromosome);
}

int cal_score(char *chromosome)
{
    int score = 0;
    int length = strlen(TARGET);

    for (unsigned int i = 0; i < length; i++)
    {
        if (TARGET[i] == chromosome[i])
        {
            score++;
        }
    }
    return score;
}

void mate(individual *parent, individual *parent2, individual *child)
{
    // printf("gnome: %s score: %d\n", parent2->chromosome, parent2->score);
    float propability;
    int length = strlen(TARGET);
    char chromosome[length];
    for (unsigned int i = 0; i < length; i++)
    {
        propability = ((rand() % 101) / 100);

        if (propability < 0.4)
        {
            chromosome[i] = parent2->chromosome[i];
        }
        else if (propability < 0.8)
        {
            chromosome[i] = parent->chromosome[i];
        }
        else
        {
            chromosome[i] = random_gene();
        }
        if (i == length - 1)
        {
            chromosome[i + 1] = '\0';
        }
    }
    strcpy(child->chromosome, chromosome);
    child->score = cal_score(child->chromosome);
}

int compare_scores(const void *a, const void *b)
{
    const individual *org_a = (individual *)a;
    const individual *org_b = (individual *)b;
    return (org_b->score - org_a->score);
}