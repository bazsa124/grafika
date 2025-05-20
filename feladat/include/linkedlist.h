#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct Node Node;

struct Node
{
    void* data;
    Node* next;
};

typedef struct LinkedList
{
    Node* head;
    Node* tail;
    size_t size;
} LinkedList;

/**
 * 
 */
LinkedList* new_linked_list();

/**
 * Initialize the linked list.
 */
void init_linked_list(LinkedList* list);

/**
 * Add a new element to the linked list.
 * Returns the data to allow chaining.
 */
void* add_node(LinkedList* list, void* data);

/**
 * Remove an element from the linked list.
 */
void remove_node(LinkedList* list, void* data);

/**
 * Gets an element from the linked list.
 */
void* get_node(const LinkedList* list, size_t index);

/**
 * Get the size of the linked list.
 */
size_t get_size(const LinkedList* list);

/**
 * Check if the linked list is empty.
 */
bool is_empty(const LinkedList* list);

/**
 * Clear the linked list.
 */
void clear_linked_list(LinkedList* list);

/**
 * Destroy the linked list.
 */
void destroy_linked_list(LinkedList* list);

#endif /* LINKEDLIST_H */