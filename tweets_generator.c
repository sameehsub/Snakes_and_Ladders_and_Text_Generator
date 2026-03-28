#include <string.h>
//Don't change the macros!
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"
#include <stdio.h>
#include <stdlib.h>

#include "markov_chain.h"


void printstr(void *data)
{
    const char* str = (const char*)data;
    printf(" %s", str);
}

int compstr(const void* ptr1, const void* ptr2)
{
    const char* first = (const char*)ptr1;
    const char* second = (const char*)ptr2;
    return strcmp(first, second);
}

void freestr(void *ptr)
{
    free(ptr);
}

void* copystr(const void *ptr)
{
    return (char *)ptr;
}

bool islaststr(void* data)
{
    const char *str = (const char*)data;
    return str[strlen(str) - 1] == '.';
}

int fill_database(FILE *fptr, int words_to_read, MarkovChain *markov_chain)
{
    char buffer[1000];
    MarkovNode *prev_markov = NULL;
    int words_left = words_to_read;

    /* read the file line by line */
    while (fgets(buffer, sizeof(buffer), fptr) != NULL &&
           (words_to_read == -1 || words_left > 0))
    {
        /* split the line into words using the given delimiters */
        char *token = strtok(buffer, DELIMITERS);
        while (token != NULL && (words_to_read == -1 || words_left > 0))
        {
            Node *current = get_node_from_database(markov_chain, token);
            MarkovNode *current_markov = NULL;

            if (!current)
            {
                char *copy = malloc(strlen(token) + 1);
                if (!copy)
                {
                    fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
                    return 1;
                }
                strcpy(copy, token);

                Node *new_node = add_to_database(markov_chain, copy);
                if (!new_node)
                {
                    fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
                    free(copy);
                    return 1;
                }
                current_markov = new_node->data;
            }
            else
            {
                current_markov = current->data;
            }

            /* link previous word → current word */
            if (prev_markov != NULL)
            {
                if (add_node_to_frequency_list(prev_markov, current_markov) == 1)
                {
                    return 1;
                }
            }

            /* reset at end-of-sentence, otherwise continue chain */
            if (markov_chain->is_last(current_markov->data))
            {
                prev_markov = NULL;
            }
            else
            {
                prev_markov = current_markov;
            }

            if (words_to_read != -1)
            {
                words_left--;
            }

            token = strtok(NULL, DELIMITERS);
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    if (argc != 4 && argc != 5)
    {
        fprintf(stderr, NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    FILE *fptr = fopen(argv[3], "r");
    if (fptr == NULL)
    {
        fprintf(stderr, FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    char *endptr;
    long seed = strtol(argv[1], &endptr, 10);
    long num_tweets = strtol(argv[2], &endptr, 10);
    long words_to_read = -1;

    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (!markov_chain)
    {
        fprintf(stderr, "error: memory allocation failed.\n");
        fclose(fptr);
        return EXIT_FAILURE;
    }

    markov_chain->database = malloc(sizeof(LinkedList));
    if (!markov_chain->database)
    {
        free(markov_chain);
        fprintf(stderr, "error: memory allocation failed.\n");
        fclose(fptr);
        return EXIT_FAILURE;
    }

    markov_chain->database->size=0;
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;

    markov_chain->print_func = printstr;
    markov_chain->comp_func = compstr;
    markov_chain->free_data = freestr;
    markov_chain->copy_func = copystr;
    markov_chain->is_last = islaststr;

    if (argc == 5)
    {
        words_to_read = strtol(argv[4], &endptr, 10);
    }

    if (fill_database(fptr, (int)words_to_read, markov_chain) == 1)
    {
        fclose(fptr);
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }

    fclose(fptr);

    srand(seed);
    for (int i = 0; i < num_tweets; i++)
    {
        printf("Tweet %d:", i + 1);

        MarkovNode *first = get_first_random_node(markov_chain);
        if (!first)
        {
            free_database(&markov_chain);
            return EXIT_FAILURE;
        }
        generate_random_sequence(markov_chain, first, 20);
    }
    free_database(&markov_chain);
    return EXIT_SUCCESS;
}
