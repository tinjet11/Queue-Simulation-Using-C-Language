#include <stdio.h>
#include <stdlib.h>

struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
};

struct Node
{
    struct Process data;
    struct Node *next;
};

void deleteNode(struct Node **head, int id)
{
    // Handle the case where the list is empty
    if (*head == NULL)
    {
        return;
    }

    // If the head node contains the value to delete
    if ((*head)->data.id == id)
    {
        struct Node *temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }

    // Traverse the list to find the node before the one to delete
    struct Node *current = *head;
    while (current->next != NULL && current->next->data.id != id)
    {
        current = current->next;
    }

    // If the value was not found in the list
    if (current->next == NULL)
    {
        printf("Value not found in the list.\n");
        return;
    }

    // Remove the node
    struct Node *temp = current->next;
    current->next = current->next->next;
    free(temp);
}

// Function to add a process to the linked list
void addToLinkedList(struct Node **head, struct Process process)
{
    // Implementation of the addToLinkedList function
    /* 1. allocate node */
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));

    struct Node *last = *head; /* used in step 5*/

    /* 2. put in the data */
    new_node->data = process;

    /* 3. This new node is going to be the last node, so
    make next of it as NULL*/
    new_node->next = NULL;

    /* 4. If the Linked List is empty, then make the new
     * node as head */
    if (*head == NULL)
    {
        *head = new_node;
        return;
    }

    /* 5. Else traverse till the last node */
    while (last->next != NULL)
        last = last->next;

    /* 6. Change the next of last node */
    last->next = new_node;
}

// Function to perform Round Robin scheduling
void roundRobin(struct Node **RRqueue, struct Node **FCFSqueue, int quantum)
{
    // Implementation of Round Robin scheduling
    struct Node *current = *RRqueue;
    struct Node *temp = NULL; //this is important, dont remove this

        int executeTime;
    while (current != NULL)
    {
        if (current->data.remainingTime > 0)
        {
            // Execute the process for the quantum or its remaining time, whichever is smaller
            executeTime = (current->data.remainingTime < quantum) ? current->data.remainingTime : quantum;
            current->data.remainingTime -= executeTime;

            // Update completionTime if the process is completed
            if (current->data.remainingTime == 0)
            {
                current->data.completionTime += executeTime;
            }else{
            addToLinkedList(FCFSqueue,current->data);
            }
            temp = current->next;//important step, else will lost the reference to next node 
            deleteNode(RRqueue, current->data.id);
        }
        current = temp; // Move to the next node
        
        // printf("%d",current->data.id);
    }
}

// Function to print the content of a queue
void printQueue(struct Node *queue, const char *queueName)
{
    
    // Implementation of printing the queue
    printf("%s\n", queueName);
    struct Node *current = queue;
    if(current == NULL){
        printf("%s","Queue is Empty\n\n");
    }
    while (current != NULL)
    {
        printf("Process with Id %d have remaining time %d \n", current->data.id, current->data.remainingTime);
        current = current->next;
    }
}

int main()
{
    int n, quantumRR;

    struct Node *rrQueue = NULL;   // Interactive RR queue
    struct Node *fcfsQueue = NULL; // FCFS queue

    // Input process details and add them to the RR queue
    // ...
    struct Process process1 = {1, 0, 10, 10, 0};
    struct Process process2 = {2, 2, 5, 5, 0};
    struct Process process3 = {3, 3, 8, 8, 0};
    struct Process process4 = {4, 4, 15, 15, 0};

    n = 4;
    quantumRR = 4;

    addToLinkedList(&rrQueue, process1);
    addToLinkedList(&rrQueue, process2);
    addToLinkedList(&rrQueue, process3);
    addToLinkedList(&rrQueue, process4);

    printQueue(rrQueue, "Interactive RR Queue");
    printQueue(fcfsQueue, "FCFS Queue");

    //To Be implement: sort function to sort the rrQueue according to the arrivalTime

    //Currently the roundrobin function is not affected by the arrival time, may need to implement it
    roundRobin(&rrQueue,&fcfsQueue,quantumRR);

    printQueue(rrQueue, "Interactive RR Queue");
    printQueue(fcfsQueue, "FCFS Queue");

    return 0;
}
