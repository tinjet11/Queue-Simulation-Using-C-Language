/* First Come First Served (FCFS), 
non-preemptive algorithm (process are only interupted voluntarily)
- Advantages: positional fairness and easy to implement  
- Disadvantages: Favours long processes over short ones, could compromise */


/* Waiting Time: Time Difference between turnaround time and burst time.

Waiting Time = Turnaround Time – Burst Time */

/* Turnaround Time: Difference between completion time and arrival time.

Turnaround Time = Completion Time – Arrival Time */

#include <stdio.h>
#include <stdlib.h>
struct Process
{
    int id;             // Process ID
    int arrivalTime;    // Arrival time
    int burstTime;      // Burst time
    int completionTime; // Completion Time
};

struct Node
{
    struct Process data;
    struct Node *next;
};

void fcfs(struct Node* head) {
    if (head == NULL) {
        printf("No processes to schedule.\n");
        return;
    }

    struct Node* current = head;
    int currentTime = 0;

    while (current != NULL) {
        if (current->data.arrivalTime > currentTime) {
            // If the process hasn't arrived yet, wait for it to arrive
            currentTime = current->data.arrivalTime;
        }

        // Calculate the completion time for the current process
        current->data.completionTime = currentTime + current->data.burstTime;

        // Update the current time
        currentTime = current->data.completionTime;

        current = current->next;
    }
}

int compareByArrivalTime(const void *a, const void *b) {
    return ((struct Process *)a)->arrivalTime - ((struct Process *)b)->arrivalTime;
}

void sortQueueByArrivalTime(struct Node **queue) {
    int n = 0;
    struct Node *current = *queue;

    // Count the number of elements in the queue
    while (current != NULL) {
        n++;
        current = current->next;
    }

    // Create an array of processes to sort
    struct Process *processes = (struct Process *)malloc(n * sizeof(struct Process));
    current = *queue;

    for (int i = 0; i < n; i++) {
        processes[i] = current->data;
        current = current->next;
    }

    // Sort the array by arrival time
    qsort(processes, n, sizeof(struct Process), compareByArrivalTime);

    // Reconstruct the sorted queue
    current = *queue;
    for (int i = 0; i < n; i++) {
        current->data = processes[i];
        current = current->next;
    }

    free(processes);
}

int main() {
    int n;
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Node* head = NULL;

    // Input process details and add them to the linked list
    for (int i = 0; i < n; i++) {
        struct Process process;
        process.id = i + 1;
        printf("Enter arrival time, and burst time for process %d:\n", i + 1);
        scanf("%d %d", &process.arrivalTime, &process.burstTime);

        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->data = process;
        newNode->next = NULL;

        if (head == NULL) {
            head = newNode;
        } else {
            struct Node* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
    }
    sortQueueByArrivalTime(&head);
    fcfs(head);

    // Print the process details including completion time
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
    struct Node* current = head;
    while (current != NULL) {
        printf("%d\t\t%d\t\t%d\t\t%d\n", current->data.id, current->data.arrivalTime, current->data.burstTime, current->data.completionTime);
        current = current->next;
    }

    return 0;
}