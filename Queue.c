#include "Queue.h"

Queue* CreateQueue()
{
    Queue* q = malloc(sizeof(struct _Queue));
    q->back = q->front = NULL;
    q->size = 0;
    return q;
}

void freeQueue(Queue** q, void (*freeEL)(void*))
{
    while( !EmptyQueue(*q) )
        dequeue(*q, freeEL);
    *q = NULL;
}

void enqueue(Queue* q, void* value)
{
    if( q->back == NULL )
    {
        q->back = q->front = CreateNode(NULL, value);
        q->size++;
        return;
    }

    Node* n = CreateNode(NULL, value);
    q->back->next = n;
    q->back = n;
    q->size++;
}

void dequeue(Queue* q, void (*freeEL)(void*))
{
    if( q->size == 1 )
    {
        freeEL(q->front->value);
        q->front = q->back = NULL;
        q->size = 0;
        return;
    }

    Node* frontNode = q->front;
    q->front = frontNode->next;

    freeEL(frontNode->value);
    free(frontNode);
    q->size--;
}

void* front(Queue* q)
{
    return q->front->value;
}

