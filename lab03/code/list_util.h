/* Pierrick Muller
 * HPC - 20.02.20
 * array_util.c
 *
 *
 * C code for list API
 */
 #include <stdint.h>
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "list_util.h"

/*
 * This function allow us to add an element at the end of the linked list.
 */
void list_push(struct list_element* head,uint64_t data)
{
        struct list_element* current = head;
        current = current;
        uint64_t temp = data;
        temp = temp;
        while(current->next != NULL)
        {
                current = current->next;
        }
        current->next = (struct list_element*) malloc(sizeof(struct list_element));
        current->next->data_value = data;
        current->next->next = NULL;
}


/* Allocate "len" linked elements and initialize them
 * with random data.
 * Return list head */
struct list_element *list_init(size_t len){

        size_t i;
        time_t t;

        /* Intializes random number generator */
        srand((unsigned) time(&t));

        struct list_element *head = (struct list_element*) malloc(sizeof(struct list_element));
        head->data_value = (uint64_t)rand();
        head->next = NULL;
        /*We add all the element needed in function of the len argument*/
        for(i = 0; i < len; i++)
        {
                list_push(head, (uint64_t)rand());
        }
        return head;
}

/* Liberate list memory */
void list_clear(struct list_element *head){

        struct list_element* current = NULL;
        while(head->next != NULL)
        {
                current = head;
                while(current->next->next != NULL)
                {
                        current = current->next;
                }
                free(current->next);
                current->next = NULL;
        }
        free(head);

}

/* Arrange a list in increasing order of value */
/* Based on : https://www.geeksforgeeks.org/c-program-bubble-sort-linked-list/*/
void list_sort(struct list_element *head){
        int swapped;
        uint64_t temp_val;
        struct list_element *ptr1;
        struct list_element *lptr = NULL;

        /* Checking for empty list */
        if (head == NULL)
                return;

        /*We loop until we find the right place for the element */
        do
        {
                swapped = 0;
                ptr1 = head;
                /*We loop until we are at the end of the linked list */
                while (ptr1->next != lptr)
                {
                        /*
                         * If the value of the actual element is greater then the value of the next
                         * element, we swap them (We swap the value )
                         */
                        if (ptr1->data_value > ptr1->next->data_value)
                        {
                                temp_val = ptr1->data_value;
                                ptr1->data_value = ptr1->next->data_value;
                                ptr1->next->data_value = temp_val;
                                swapped = 1;
                        }
                        ptr1 = ptr1->next;
                }
                lptr = ptr1;
        }
        while (swapped);
}
