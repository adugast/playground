#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/*
 *  CONCAT STR 2 - concat str with varargs -> number of string limited by the macro COUNT_ARGS
 */
#define _COUNT_ARGS(_9,_8,_7,_6,_5,_4,_3,_2,_1,_0, nbr,...) nbr
#define COUNT_ARGS(...) \
    _COUNT_ARGS(,##__VA_ARGS__,9,8,7,6,5,4,3,2,1,0)

#define concats2(...) \
    _concats2(COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

char *_concats2(int count, ...)
{
    va_list ap;
    int size = 0;
    int i = 0;

    char *test[count];

    va_start(ap, count);
    for (i = 0; i < count; i++) {
        test[i] = va_arg(ap, char *);
        size += strlen(test[i]);
    }
    va_end(ap);

    char *res = calloc(size + 1, sizeof(char));
    for (i = 0; i < count; i++)
        strcat(res, test[i]);

    return res;
}


/*
 *  CONCAT STR - concat str with varargs -> Uses a special macro TAG_END to always know
 *  the end of the va_list -> means unlmited number of args
 */

#define TAG_END "\0"
#define concats(...) \
    _concats(-1, ##__VA_ARGS__, TAG_END)


char *_concats(int last, ...)
{
    va_list ap;
    size_t size = 0;

    /* first loop count the size of the buffer */
    va_start(ap, last);
    char *token = va_arg(ap, char *);
    while (strcmp(token, TAG_END) != 0) {
        size += strlen(token);
        token = va_arg(ap, char *);
    }
    va_end(ap);

    char *res = calloc(size + 1, sizeof(char));
    if (res == NULL)
        return NULL;

    /* second loop concat the strings */
    va_start(ap, last);
    token = va_arg(ap, char *);
    while (strcmp(token, TAG_END) != 0) {
        strcat(res, token);
        token = va_arg(ap, char *);
    }
    va_end(ap);

    return res;
}


int test_concat()
{
    char *tmp = NULL;

    tmp = concats();
    printf("[%s]\n", tmp);
    free(tmp);
    tmp = concats("concats2", "qw");
    printf("[%s]\n", tmp);
    free(tmp);
    tmp = concats("concats2", "tata", "titi", "toto");
    printf("[%s]\n", tmp);
    free(tmp);
    tmp = concats("concats2", "toto", "tata");
    printf("[%s]\n", tmp);

    free(tmp);

    char *test = " tralala ";
    tmp = concats("concats3", test, "titi", "tata");
    printf("[%s]\n", tmp);
    free(tmp);

    /* doesn't work with empty str */
    tmp = concats2("");
    printf("[%s]\n", tmp);
    free(tmp);

    tmp = concats2("2");
    printf("[%s]\n", tmp);
    free(tmp);

    tmp = concats2("62", "234");
    printf("[%s]\n", tmp);
    free(tmp);

    printf("\nNEXT\n\n");

    return 0;
}


/*
 *  function FOR_EACH_TOKEN - special macro over strtok_r to loop over
 *  every token splited by delim.
 *  The UNIQUE variable is only a temporary unique name variable to store strtok_r saveptr
 */

#define UNIQUEPASTE(x, y)   x ## y
#define UNIQUEPASTE2(x, y)  UNIQUEPASTE(x, y)
#define UNIQUE              UNIQUEPASTE2(_tmp_, __LINE__)


#define for_each_token2(str, delim, saveptr, token) \
    for (token = strtok_r(str, delim, saveptr); token != NULL; token = strtok_r(NULL, delim, saveptr)) \


#define for_each_token(str, delim, token) char *UNIQUE = NULL; \
    for (token = strtok_r(str, delim, &UNIQUE); token != NULL; token = strtok_r(NULL, delim, &UNIQUE)) \


int test_for_each_token()
{
    char *token = NULL;

    char str[] = "How is it going ?";
    for_each_token(str, " ", token) {
        printf("token_test1[%s]\n", token);
    }

    char str2[] = "How is it going ?";
    for_each_token(str2, "\n", token) {
        printf("token_test2[%s]\n", token);
    }

    char str3[] = "How is it going ?";
    for_each_token(str3, "", token) {
        printf("token_test3[%s]\n", token);
    }

    for_each_token(NULL, " ", token) {
        printf("token_test4[%s]\n", token);
    }


    char str4[] = "ls -l -a | grep main.c | wc -l ; echo 1";
    char *token2 = NULL;

    for_each_token(str4, ";", token2) {
        printf("\n");
        for_each_token(token2, "|", token2) {
            printf("\n");
            for_each_token(token2, " ", token2) {
                printf("token2[%s]\n", token2);
            }
        }
    }
    printf("\n");

//    char *token3 = NULL;
//    char *stack[] = { ";", "|", " ", NULL };

    return 0;
}


/*  __attribute__((constructor (PRIORITY)))
 *  multiple constructor and destructor can be defined based on the priority level
 *  level => number, i.e __attribute__((constructor (101)))
 */
__attribute__((constructor)) void constructor()
{
    printf("constructor\n");
}

__attribute__((destructor)) void destructor()
{
    printf("destructor\n");
}

void free_cb(void **ptr)
{
    printf("buffer freed\n");
    free(*ptr);
}

#define alloc(ptr, size, fct_cb) \
    __attribute__((__cleanup__(fct_cb))) void *UNIQUE = calloc(1, size); \
    ptr = UNIQUE;


void clean_up(int *var)
{
    printf("cleanup: %d\n", *var);
}


int test_cleanup_attr()
{
    /* the variable var is automatically cleanup when the scoop ends */
    {
        __attribute__((__cleanup__(clean_up))) int var = 3;
        var = 10;
    }


    char *buffer = NULL;
    printf("buffer declared\n");

    alloc(buffer, 25, free_cb);
    printf("buffer allocced\n");
    strcpy(buffer, "Trying special feature");
    printf("buffer used[%s]\n", buffer);

    char *other = NULL;
    alloc(other, 30, free_cb);
    strncpy(other, "abcdefghijklnopqrstuvwxyz", 30);
    printf("other[%s]\n", other);

    return 0;
}

int main()
{
    test_concat();
    printf("\n");
    test_for_each_token();
    test_cleanup_attr();

    return 0;
}
