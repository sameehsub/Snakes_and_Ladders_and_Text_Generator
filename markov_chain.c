#include "markov_chain.h"
#include <string.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr)
{
    Node* curr_node = markov_chain->database->first;
    while (curr_node!=NULL)
    {
        if (markov_chain->comp_func(data_ptr, curr_node->data->data) == 0)
        {
            return curr_node;
        }
        Node *temp = curr_node;
        curr_node = temp->next;
    }
    return NULL;
}

Node *add_to_database(MarkovChain *markov_chain, void *data_ptr)
{
    Node *target = get_node_from_database(markov_chain, data_ptr);
    if (target)
    {
        return target;
    }

    MarkovNode *new_markov_node = malloc(sizeof(MarkovNode));
    if (!new_markov_node)
    {
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    new_markov_node->data = markov_chain->copy_func(data_ptr);
    new_markov_node->frequency_list = NULL;
    new_markov_node->frequency_size = 0;

    if (add(markov_chain->database, new_markov_node) != 0 )
    {
        free(new_markov_node);
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }
    return markov_chain->database->last;
}

int add_node_to_frequency_list(MarkovNode *first_node,
                               MarkovNode *second_node)
{
    for (int i=0; i<first_node->frequency_size;i++)
    {
        if (first_node->frequency_list[i].markov_node == second_node)
        {
            first_node->frequency_list[i].frequency++;
            return 0;
        }
    }

    MarkovNodeFrequency *new_list = realloc(first_node->frequency_list, sizeof(MarkovNodeFrequency)*(first_node->frequency_size+1));
    if (!new_list)
    {
        return 1;
    }
    first_node->frequency_list = new_list;
    first_node->frequency_list[first_node->frequency_size].markov_node = second_node;
    first_node->frequency_list[first_node->frequency_size].frequency = 1;
    first_node->frequency_size++;

    return 0;
}

void free_database(MarkovChain ** ptr_chain)
{
    LinkedList *database = (*ptr_chain)->database;
    Node *current = database->first;

    while (current)
    {
        Node *next = current->next;
        MarkovNode *markov_node = current->data;
        if (markov_node)
        {
            (*ptr_chain)->free_data(markov_node->data);
            free(markov_node->frequency_list);
            free(markov_node);
        }
        free(current);
        current = next;
    }
    free(database);
    free(*ptr_chain);
    *ptr_chain = NULL;
}

MarkovNode* get_first_random_node(MarkovChain *markov_chain)
{
    int random_index = get_random_number(markov_chain->database->size);
    Node* random_node = markov_chain->database->first;
    for (int i=0; i < random_index ; i++)
    {
        random_node = random_node->next;
    }
    while (random_node && markov_chain->is_last(random_node->data->data))
    {
        random_index = get_random_number(markov_chain->database->size);
        random_node = markov_chain->database->first;
        for (int i=0; i<random_index; i++)
        {
            random_node = random_node->next;
        }
    }
    return random_node ? random_node->data : NULL;
}

MarkovNode *get_next_random_node(MarkovNode *cur_markov_node)
{
    int frequencies_sum = 0;
    for (int i=0; i < cur_markov_node->frequency_size; i++)
    {
        frequencies_sum += cur_markov_node->frequency_list[i].frequency;
    }
    int random_index = get_random_number(frequencies_sum);

    for (int i=0; i < cur_markov_node->frequency_size; i++)
    {
        if (random_index < cur_markov_node->frequency_list[i].frequency)
        {
            return cur_markov_node->frequency_list[i].markov_node;
        }
        random_index = random_index - cur_markov_node->frequency_list[i].frequency;
    }

    return cur_markov_node->frequency_list[cur_markov_node->frequency_size - 1].markov_node;
}

void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length)
{
    MarkovNode *current_node = first_node;
    int word_count = 0;
    markov_chain->print_func(current_node->data);
    word_count++;

    while (word_count<max_length)
    {
        current_node = get_next_random_node(current_node);
        if (!current_node)
        {
            printf("\n");
            return;
        }
        markov_chain->print_func(current_node->data);
        if (markov_chain->is_last(current_node->data))
        {
            printf("\n");
            return;
        }
        word_count++;
    }
    printf("\n");
}

