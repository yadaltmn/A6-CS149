/*
 * countnames.c
 * Assignment 6: count names from multiple files using one pthread per file.
 * GitHub : https://github.com/yadaltmn/A6-CS149
 * Author :
 * Jesse Mendoza
 * Jada-Lien Nguyen
 * Date modified : 05/03/2026
 *
 */

#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINE_BUFFER_SIZE 256
#define INITIAL_HASH_CAPACITY 10

typedef struct NameCountData NameCountData;

struct NameCountData
{
    char *name;
    int count;
    NameCountData *next;
};

typedef struct
{
    const char *filename;
} ThreadArgs;

/* Shared dynamic table: hashBuckets gives fast lookup, insertionOrder keeps print order. */
static NameCountData **hashBuckets = NULL;
static NameCountData **insertionOrder = NULL;
static int hashBucketCount = 0;
static int insertionCount = 0;
static int insertionCapacity = 0;
static int hadError = 0;
/* Protects shared table updates so multiple threads cannot modify it at the same time. */
static pthread_mutex_t countsMutex = PTHREAD_MUTEX_INITIALIZER;

static int redirect(void)
{
    char out[64];
    char errName[64];
    int outFd;
    int errFd;

    snprintf(out, sizeof(out), "%d.out", getpid());
    snprintf(errName, sizeof(errName), "%d.err", getpid());

    outFd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (outFd == -1)
    {
        perror("open output file");
        return 1;
    }

    errFd = open(errName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (errFd == -1)
    {
        perror("open error file");
        close(outFd);
        return 1;
    }

    if (dup2(outFd, STDOUT_FILENO) == -1 || dup2(errFd, STDERR_FILENO) == -1)
    {
        perror("dup2");
        close(outFd);
        close(errFd);
        return 1;
    }

    close(outFd);
    close(errFd);
    return 0;
}

static void mark_error(void)
{
    pthread_mutex_lock(&countsMutex);
    hadError = 1;
    pthread_mutex_unlock(&countsMutex);
}

static char *duplicate_string(const char *s)
{
    size_t length = strlen(s) + 1;
    char *copy = (char *)malloc(length);

    if (copy != NULL)
    {
        memcpy(copy, s, length);
    }

    return copy;
}

static unsigned hash_name(const char *s, int bucketCount)
{
    unsigned hashval = 0;

    while (*s != '\0')
    {
        hashval = (unsigned)(*s) + 31 * hashval;
        s++;
    }

    return hashval % (unsigned)bucketCount;
}

static int init_table(void)
{
    /* Start with a small table and grow it when more unique names are added. */
    hashBuckets = (NameCountData **)malloc(sizeof(NameCountData *) * INITIAL_HASH_CAPACITY);
    insertionOrder = (NameCountData **)malloc(sizeof(NameCountData *) * INITIAL_HASH_CAPACITY);
    if (hashBuckets == NULL || insertionOrder == NULL)
    {
        free(hashBuckets);
        free(insertionOrder);
        hashBuckets = NULL;
        insertionOrder = NULL;
        return 1;
    }

    for (int i = 0; i < INITIAL_HASH_CAPACITY; i++)
    {
        hashBuckets[i] = NULL;
        insertionOrder[i] = NULL;
    }

    hashBucketCount = INITIAL_HASH_CAPACITY;
    insertionCount = 0;
    insertionCapacity = INITIAL_HASH_CAPACITY;
    return 0;
}

static NameCountData *lookup_name(const char *name)
{
    unsigned hashIndex = hash_name(name, hashBucketCount);
    NameCountData *node = hashBuckets[hashIndex];

    while (node != NULL)
    {
        if (strcmp(name, node->name) == 0)
        {
            return node;
        }
        node = node->next;
    }

    return NULL;
}

static int grow_table(void)
{
    int newCapacity = hashBucketCount * 2;
    NameCountData **newBuckets;
    NameCountData **newInsertionOrder;

    /* Rebuild the hash table because each name may belong in a different bucket. */
    newBuckets = (NameCountData **)malloc(sizeof(NameCountData *) * newCapacity);
    newInsertionOrder = (NameCountData **)realloc(insertionOrder, sizeof(NameCountData *) * newCapacity);
    if (newBuckets == NULL || newInsertionOrder == NULL)
    {
        free(newBuckets);
        if (newInsertionOrder != NULL)
        {
            insertionOrder = newInsertionOrder;
        }
        return 1;
    }

    for (int i = 0; i < newCapacity; i++)
    {
        newBuckets[i] = NULL;
    }
    for (int i = insertionCapacity; i < newCapacity; i++)
    {
        newInsertionOrder[i] = NULL;
    }

    free(hashBuckets);
    hashBuckets = newBuckets;
    insertionOrder = newInsertionOrder;
    hashBucketCount = newCapacity;
    insertionCapacity = newCapacity;

    for (int i = 0; i < insertionCount; i++)
    {
        unsigned hashIndex = hash_name(insertionOrder[i]->name, hashBucketCount);
        insertionOrder[i]->next = hashBuckets[hashIndex];
        hashBuckets[hashIndex] = insertionOrder[i];
    }

    return 0;
}

static int add_name_count(const char *name)
{
    NameCountData *node;

    /* Lookup, insert, resize, and increment all touch shared data. */
    pthread_mutex_lock(&countsMutex);

    node = lookup_name(name);
    if (node != NULL)
    {
        node->count++;
        pthread_mutex_unlock(&countsMutex);
        return 0;
    }

    if (insertionCount == insertionCapacity && grow_table() != 0)
    {
        pthread_mutex_unlock(&countsMutex);
        return 1;
    }

    node = (NameCountData *)malloc(sizeof(*node));
    if (node == NULL)
    {
        pthread_mutex_unlock(&countsMutex);
        return 1;
    }

    node->name = duplicate_string(name);
    if (node->name == NULL)
    {
        free(node);
        pthread_mutex_unlock(&countsMutex);
        return 1;
    }

    node->count = 1;
    {
        unsigned hashIndex = hash_name(node->name, hashBucketCount);
        node->next = hashBuckets[hashIndex];
        hashBuckets[hashIndex] = node;
    }
    insertionOrder[insertionCount++] = node;

    pthread_mutex_unlock(&countsMutex);
    return 0;
}

static void count_stream(FILE *fp, const char *sourceName)
{
    char buffer[LINE_BUFFER_SIZE];
    int lineNum = 0;

    /* Read one stream and add every non-empty line as a name. */
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        lineNum++;
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0)
        {
            fprintf(stderr, "Warning - file %s line %d is empty.\n", sourceName, lineNum);
            continue;
        }

        if (add_name_count(buffer) != 0)
        {
            fprintf(stderr, "error: unable to allocate memory while reading %s\n", sourceName);
            mark_error();
            break;
        }
    }
}

static void *count_file(void *arg)
{
    ThreadArgs *threadArgs = (ThreadArgs *)arg;
    FILE *fp = fopen(threadArgs->filename, "r");

    if (fp == NULL)
    {
        fprintf(stderr, "error: cannot open file %s\n", threadArgs->filename);
        mark_error();
        return NULL;
    }

    /* Each worker thread reads one input file. */
    count_stream(fp, threadArgs->filename);

    fclose(fp);
    return NULL;
}

static void print_summary_table(void)
{
    /* Print in first-seen order instead of hash bucket order. */
    for (int i = 0; i < insertionCount; i++)
    {
        printf("%s: %d\n", insertionOrder[i]->name, insertionOrder[i]->count);
    }
}

static void free_table(void)
{
    /* Free each stored name and node before freeing the table arrays. */
    if (insertionOrder != NULL)
    {
        for (int i = 0; i < insertionCount; i++)
        {
            free(insertionOrder[i]->name);
            free(insertionOrder[i]);
        }
    }

    free(hashBuckets);
    free(insertionOrder);
    hashBuckets = NULL;
    insertionOrder = NULL;
    hashBucketCount = 0;
    insertionCount = 0;
    insertionCapacity = 0;
}

int main(int argc, char *argv[])
{
    int threadCount = argc - 1;
    pthread_t *threads;
    ThreadArgs *threadArgs;
    int status = 0;

    if (redirect() != 0)
    {
        return 1;
    }

    if (init_table() != 0)
    {
        fprintf(stderr, "error: unable to allocate name count table\n");
        return 1;
    }

    if (argc == 1)
    {
        count_stream(stdin, "stdin");
        if (hadError != 0)
        {
            status = 1;
        }

        print_summary_table();
        free_table();
        pthread_mutex_destroy(&countsMutex);
        return status;
    }

    threads = (pthread_t *)malloc(sizeof(pthread_t) * (size_t)threadCount);
    threadArgs = (ThreadArgs *)malloc(sizeof(ThreadArgs) * (size_t)threadCount);
    if (threads == NULL || threadArgs == NULL)
    {
        fprintf(stderr, "error: unable to allocate thread data\n");
        free(threads);
        free(threadArgs);
        free_table();
        return 1;
    }

    /* Create one pthread for each input file. */
    for (int i = 0; i < threadCount; i++)
    {
        threadArgs[i].filename = argv[i + 1];
        if (pthread_create(&threads[i], NULL, count_file, &threadArgs[i]) != 0)
        {
            fprintf(stderr, "error: unable to create thread for %s\n", argv[i + 1]);
            mark_error();
            threadCount = i;
            break;
        }
    }

    /* Wait for every file-reading thread before printing final counts. */
    for (int i = 0; i < threadCount; i++)
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            fprintf(stderr, "error: unable to join thread\n");
            status = 1;
        }
    }

    if (hadError != 0)
    {
        status = 1;
    }

    print_summary_table();
    free(threads);
    free(threadArgs);
    free_table();
    pthread_mutex_destroy(&countsMutex);

    return status;
}
