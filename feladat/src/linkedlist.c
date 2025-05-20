#include "linkedlist.h"

LinkedList* new_linked_list()
{
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    return list;
}

void init_linked_list(LinkedList* list)
{
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void* add_node(LinkedList* list, void* data)
{
    Node* node = (Node*)malloc(sizeof(Node));

    node->data = data;
    node->next = NULL;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

    list->size++;

    return data;
}

void remove_node(LinkedList* list, void* data)
{
    Node* current = list->head;
    Node* previous = NULL;

    while (current != NULL)
    {
        if (current->data == data)
        {
            if (previous == NULL)
            {
                list->head = current->next;
            }
            else
            {
                previous->next = current->next;
            }

            free(current);
            list->size--;

            return;
        }

        previous = current;
        current = current->next;
    }
}

void* get_node(const LinkedList* list, size_t index)
{
    Node* current = list->head;
    size_t i = 0;

    while (current != NULL)
    {
        if (i == index)
        {
            return current->data;
        }

        current = current->next;
        i++;
    }

    return NULL;
}

size_t get_size(const LinkedList* list)
{
    return list->size;
}

bool is_empty(const LinkedList* list)
{
    return list->size == 0;
}

void clear_linked_list(LinkedList* list)
{
    Node* current = list->head;
    Node* next = NULL;

    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void destroy_linked_list(LinkedList* list)
{
    clear_linked_list(list);
    free(list);
}