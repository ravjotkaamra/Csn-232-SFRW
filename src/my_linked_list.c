#include "../include/my_linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __node_t
{
    int val;
    struct __node_t *next;
} node_t;

node_t *head;

void list_init()
{
    head = NULL;
}

// writer call this method
// to insert a new node at
// the front of the linkedlist
void list_insert(int value)
{
    // malloc a new node
    node_t *new = malloc(sizeof(node_t));

    // check whether memory was allocated
    assert(new != NULL);

    // set the value and next to NULL
    new->val = value;
    new->next = NULL;

    // insertion in the front of the linked list
    new->next = head;
    head = new;
}

// readers call this method to get
// the sum of values in linked list
int list_get_sum()
{
    int sum = 0;

    node_t *curr = head;

    while (curr != NULL)
    {
        // we are writing to a stack variable
        // so no synchronisation issue occurs
        sum = sum + curr->val;
        curr = curr->next;
    }

    return sum;
}