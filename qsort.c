#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int comparator(const void *str1, const void *str2)
{
    const char* string1 = *(const char**)str1;
    const char* string2 = *(const char**)str2;
    printf("%s\n%s\n", string1, string2);
    if (strcmp(*(const char **)str1, *(const char **)str2) >= 0)
        return 1;
    else
        return 0;
}
int main()
{
    char *arr[] = {"Rishabh", "Jyoti", "Palak", "Akash"};
    int n = sizeof(arr) / sizeof(arr[0]);
//    printf("Given array of names: \n");
//    for (int i = 0; i < n; i++) 
//         printf("%s \t", arr[i]);
    qsort(arr, n, sizeof(const char*), comparator);
   printf("Sorted array of names: \n");
   for (int i = 0; i < n; i++)
      printf("%s \t", arr[i]);
   return 0;
}