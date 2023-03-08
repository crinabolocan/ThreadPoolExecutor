#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "Stack.h"
#include "Node.h"
#include "Queue.h"

typedef struct _Thread
{
    int ID;
} Thread;

typedef struct _Task
{
    int ID;
    Thread *t;         // thread
    int p;             // prioritate
    unsigned int timp; // timp de executie
} Task;

void freeTask(void *value)
{
    //Task *val = (Task *)value;
    // free(val->t);
    // free(val);
}

int ordonare_task(Queue *coada)
{
    Stack *stiva_aux = CreateStack();
    int n = coada->size;
    for (int i = 0; i < n; i++) //merg prin coada
    {
        int minindex = -1;
        Task *minvalue = calloc(1, sizeof(Task));
        minvalue->p = INT_MAX; //initializare valoare maxima ca sa stim cu ce comparam
        for (int j = 0; j < coada->size; j++)
        {
            Task *task0 = calloc(1, sizeof(Task));
            task0 = (Task *)coada->front->value;
            if (task0->p < minvalue->p) //verificare prioritate task
            {
                minvalue = task0; //schimb
                minindex = j;
            }
            else if (task0->p == minvalue->p)
            {
                if (task0->timp > minvalue->timp) //verificare timp_executie task
                {
                    minvalue = task0; //schimb
                    minindex = j;
                }
                else if (task0->timp == minvalue->timp) 
                {
                    if (task0->ID > minvalue->ID) //verificare ID task
                    {
                        minvalue = task0; //schimb
                        minindex = j;
                    }
                }
            }

            dequeue(coada, freeTask);//extrag din coada un element
            enqueue(coada, task0);// introduc ce element am gasit potrivit conditiilor
        }

        for (int l = 0; l < minindex; l++)
        {
            Task *task1 = calloc(1, sizeof(Task));
            task1 = (Task *)coada->front->value;
            dequeue(coada, freeTask);
            enqueue(coada, task1);
        }
        push(stiva_aux, minvalue); //introduc in stiva
        dequeue(coada, freeTask); //scot din coada
    }
    Node *m = calloc(1, stiva_aux->size);
    for (m = (Node *)stiva_aux->top; m != NULL; m = m->next) //parcurg stiva
    {
        Task *task3 = calloc(1, sizeof(Task));
        task3 = (Task *)m->value;
        enqueue(coada, task3); //introduc ce am salvat in stiva inapoi in coada
    }
    return 0;
}
void add_task(Queue *wait, int *aux_id, FILE *f, FILE *output)
{
    int number_task = 0;
    int exec_time = 0;
    int priority = 0;
    int contor = 0;
    fscanf(f, "%d", &number_task);
    fscanf(f, "%d", &exec_time);
    fscanf(f, "%d", &priority); //am decis sa citesc aici lucrurile 
    //de care am nevoie si sa nu le dau ca parametru
    int j = 0;
    int i = 0;
    for (i = 1; i <= number_task; i++) //parcurg cate numere de taskuri trebuie
    {
        j = 0; //pornesc de  la 0 ca atunci cand il gaseste sa adauge 
        //incepand de la 1
        while (j + 1 == aux_id[j])
            j++; //adauga
        Task *task = calloc(1, sizeof(Task));
        aux_id[j] = j + 1;
        task->ID = j + 1;
        contor = j;
        task->p = priority;
        task->timp = exec_time; //introduce valorile
        enqueue(wait, task); //introduc in coada task-ul salvat
    }
    for (i = contor - number_task + 1; i <= contor; i++) //merg prin ID-uri
    {
        fprintf(output, "Task created successfully : ID %d.\n", aux_id[i]);
    }
}

void get_task(Queue *wait, FILE *f, FILE *output)
{
    Node *node = calloc(1, sizeof(Node));
    int task_id = 0;
    fscanf(f, "%d", &task_id);
    // printf("%d", task_id);
    for (node = (Node *)wait->front; node != NULL; node = node->next) //parcurg coada
    {
        Task *task = calloc(1, sizeof(Task));
        task = node->value; //salvez valoare din task(structura)
        // printf("%d\n", task->ID);
        if (task->ID == task_id) //daca gaseste printeaza
            fprintf(output, "Task %d is waiting (remaining_time = %d).\n", task->ID, task->timp);
    }
}

void get_thread(Queue *wait, int N, FILE *f, FILE *output)
{
    Node *node = calloc(1, sizeof(Node));
    int thread_id = 0;
    fscanf(f, "%d", &thread_id);
    if (thread_id < N)
    {
        for (node = (Node *)wait->front; node != NULL; node = node->next)
        {
            Task *task = calloc(1, sizeof(Task));
            task = node->value;
            Thread *thread = calloc(1, sizeof(Thread));
            thread = task->t;//am facut acelasi lucru doar ca
                             // am intrat si in structura de thread ca sa stiu daca e ceva acolo
            // ar fi mai trebuit introdus un else if
            //  in caz ca era diferit(in caz in care il gasea) sa afisez :
            //Thread <thread_id> is running task <task_id> (remaining_time = <timp_ramas>)
            if (thread == NULL)
            {
                fprintf(output, "Thread %d is idle.\n", thread_id);
                break;
            }
        }
    }
}
void print_waiting(Queue *wait, FILE *f, FILE *output)
{
    ordonare_task(wait);//am apelat functia de ordonare
    fprintf(output, "====== Waiting queue =======\n[");
    Node *node = calloc(1, sizeof(Node));
    for (node = (Node *)wait->front; node != NULL; node = node->next)
    {//merg prin coada
        Task *task = calloc(1, sizeof(Task));
        task = node->value;//atribui si printez ce am salvatin coada
        fprintf(output, "(%d: priority = %d, remaining_time = %d)", task->ID, task->p, task->timp);
        if (node->next == NULL)
            fprintf(output, "]\n");
        else
            fprintf(output, ",\n");
    }
    if (wait->front == NULL)
        fprintf(output, "]\n");
}

void print_running(Queue *wait, FILE *f, FILE *output)
{//asemanator ca la coada de waiting
    ordonare_task(wait); // am apelat functia de ordonare
    fprintf(output, "====== Running in parallel =======\n[");
    Node *node = calloc(1, sizeof(Node));
    for (node = (Node *)wait->front; node != NULL; node = node->next)
    {
        Task *task = calloc(1, sizeof(Task));
        task = node->value;
        Thread *thread = calloc(1, sizeof(Thread));
        thread = task->t;
        fprintf(output, "(%d: priority = %d, remaining_time = %d, running_thread = %d)", task->ID, task->p, task->timp, thread->ID);
        if (node->next == NULL)
            fprintf(output, "]\n");
        else
            fprintf(output, ",\n");
    }
    if (wait->front == NULL)
        fprintf(output, "]\n");
}

void print_finished(Queue *wait, FILE *f, FILE *output)
{//asemanator ca la coada de waiting
    ordonare_task(wait); // am apelat functia de ordonare
    fprintf(output, "====== Finished queue =======\n[");
    Node *node = calloc(1, sizeof(Node));
    for (node = (Node *)wait->front; node != NULL; node = node->next)
    {
        Task *task = calloc(1, sizeof(Task));
        task = node->value;
        //Thread *thread = calloc(1, sizeof(Thread));
        //thread = task->t;
        fprintf(output, "(%d: priority = %d, executed_time = %d)", task->ID, task->p, task->timp);
        if (node->next == NULL)
            fprintf(output, "]\n");
        else
            fprintf(output, ",\n");
    }
    if (wait->front == NULL)
        fprintf(output, "]\n");
}

int main(int argc, char *argv[])
{
    FILE *f = fopen(argv[1], "rt");
    FILE *output = fopen(argv[2], "wt");
    if (f == NULL)
    {
        printf("Eroare\n");
        return 0;
    }
    int Q = 0;
    int C = 0;
    char *s = calloc(1, 10 * sizeof(char));
    fscanf(f, "%d", &Q);//cuanta de timp
    fscanf(f, "%d", &C);//core
    int N;
    N = 2 * C;//nr max de thread-uri care ruleaza in paralel
    Stack *Poll = CreateStack();
    for (int i = N - 1; i > 0; i--)
    {
        Thread *thread = calloc(1, sizeof(Thread));
        thread->ID = i;
        push(Poll, thread);//introduc in stack pool
    }
    Queue *waiting = CreateQueue();
    Queue *running = CreateQueue();
    Queue *finished = CreateQueue();//creez cele 3 cozi
    int *aux_id = calloc(32767, sizeof(int));
    while (fscanf(f, "%s", s) == 1) //citesc input-urile sa stiu ce comanda execut
    {
        if (strcmp(s, "add_tasks") == 0)
        {
            add_task(waiting, aux_id, f, output);
        }
        else if (strcmp(s, "get_task") == 0)
        {
            get_task(waiting, f, output);
        }
        else if (strcmp(s, "get_thread") == 0)
        {
            get_thread(waiting, N, f, output);
        }
        else if (strcmp(s, "print") == 0)
        {
            fscanf(f, "%s", s);
            if (strcmp(s, "waiting") == 0)
                print_waiting(waiting, f, output);
            else if (strcmp(s, "running") == 0)
                print_running(running, f, output);
            else if (strcmp(s, "finished") == 0)
                print_finished(finished, f, output);
        }
    }
    fclose(f);
    fclose(output);
}