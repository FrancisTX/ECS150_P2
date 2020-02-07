#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

struct Node{
    void* data;
    struct Node* next;
};
struct queue {
	/* TODO Phase 1 */
    struct Node* head;
    struct Node* tail;
    int cur_size;
};


queue_t queue_create(void)
{
    struct queue* queue;
    queue =(struct queue*)malloc(sizeof(struct queue));
    queue -> head = (struct Node*)malloc(sizeof(struct Node));
    queue -> tail = (struct Node*)malloc(sizeof(struct Node));
    queue -> tail = queue->head;
    if (queue -> head == NULL) {
        return NULL;
    } else {
        queue->cur_size = 0;
    } 
    return queue;
}

int queue_destroy(queue_t queue)
{
	if (queue->cur_size != 0 || queue->head == NULL)
	    return -1;

    queue -> head = NULL;
    queue -> tail = NULL;
    queue->cur_size = 0;

    free(queue);
    return 0;
}

int queue_enqueue(queue_t queue, void *data)
{
    if (queue == NULL || data == NULL)
        return -1;

    struct Node* new_node;
    new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node -> data = data;
    new_node -> next = NULL;

    if (queue -> cur_size == 0){
        queue ->  head= new_node;
        queue -> tail = new_node;
    }else {
        queue -> tail ->next = new_node;
        queue -> tail = queue ->tail->next;
    }
    queue->cur_size++;

    return 0;
}


int queue_dequeue(queue_t queue, void **data)
{
    if (queue->head== NULL){
        return -1;
    } else if (queue->head->next == NULL){
        *data = queue->head->data;
        queue->head = NULL;
        queue->tail = NULL;
        queue->cur_size--;
        return 0;
    } else {
        *data = queue->head->data;
        queue->head = queue->head->next;
        queue->cur_size--;
        return 0;
    }
}



int queue_delete(queue_t queue, void *data)
{
    struct Node* temp = (struct Node*) malloc (sizeof(struct Node));
    struct Node* prev = (struct Node*) malloc (sizeof(struct Node));

    prev = NULL;
    temp = queue->head;

    if (temp == NULL || data == NULL){
        return -1;
    }

    if (temp->data == data){
        queue->head = temp->next;
        queue->cur_size--;
        free(temp);
        return 0;
    }
    while(temp != NULL && temp -> data != data){
        prev = temp;
        temp = temp -> next;
    }
    if (prev->next == NULL){
        free(temp);
        return -1;
    } else {
        prev->next = temp->next;
        queue->cur_size--;
        free(temp);
        return 0;
    }
}


int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
    struct Node *temp = (struct Node*) malloc (sizeof(struct Node));
    temp = queue->head;

    if (queue->head == NULL || func == NULL)
        return -1;

    for (int i = 0; i < queue->cur_size; i++) {
        if (func(temp->data, arg) == 1) {
            if (temp->data != NULL)
                *data = temp->data;
            break;
        }
        temp = temp->next;
    }

    free(temp);
    return 0;
}


int queue_length(queue_t queue)
{
	return queue == NULL ? -1 : queue->cur_size;
}


