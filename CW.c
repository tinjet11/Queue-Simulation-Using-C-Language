#include <stdio.h>
#include <stdlib.h>

struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int turnaroundTime;
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

void printMenu() {
    printf("Choose a scheduling algorithm:\n");
    printf("1. First-Come-First-Serve (FCFS)\n");
    printf("2. Round Robin (RR)\n");
    printf("3. Multi-Level Feedback Queue (MLFQ)\n");
}

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

void roundRobin(struct Process processes[], int n, int quantum)
{
    int remainingTime[n]; // Remaining burst time for each process
    int currentTime = 0;  // Current time
    int done = 0;         // Flag to check if all processes are done

    // Initialize remainingTime array with burst times
    for (int i = 0; i < n; i++)
    {
        remainingTime[i] = processes[i].burstTime;
    }

    while (1)
    {
        done = 1; // Assume all processes are done

        for (int i = 0; i < n; i++)
        {
            // Check if the process has arrived and has burst time remaining
            if (remainingTime[i] > 0 )
            {
                done = 0; // Not all processes are done

                
                while(currentTime < processes[i].arrivalTime){
                    currentTime++;
                }

                if (remainingTime[i] > quantum)
                {
                    // Process for 'quantum' time units
                    currentTime += quantum;
                    remainingTime[i] -= quantum;
                }
                else
                {
                    // Process the remaining burst time
                    currentTime += remainingTime[i];
                    remainingTime[i] = 0;
                }

                // Update completion time when the process finishes execution
                processes[i].completionTime = currentTime;
            }
            
        }

        if (done == 1)
        {
            break; // If all processes are done, break the loop
        }
    }
}

void roundRobin1(struct Node **RRqueue, struct Node **FCFSqueue, int quantum)
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

void sortArrivalTime(struct Process processes[], int n)
{
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - i - 1; j++)
        {
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime)
            {
                struct Process temp = processes[j];
                processes[j] = processes[j + 1];
                processes[j + 1] = temp;
            }
        }
    }
}

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

void printResults(struct Node *rrQueue, struct Node *fcfsQueue)
{
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");

    // Print RR Queue results
    struct Node *currentRR = rrQueue;
    while (currentRR != NULL)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentRR->data.completionTime);
        currentRR = currentRR->next;
    }

    // Print FCFS Queue results
    struct Node *currentFCFS = fcfsQueue;
    while (currentFCFS != NULL)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\n", currentFCFS->data.id, currentFCFS->data.arrivalTime, currentFCFS->data.burstTime, currentFCFS->data.completionTime);
        currentFCFS = currentFCFS->next;
    }


}

float calculateAverageWaitingTime(struct Node *queue) {
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current = queue;
    while (current != NULL) {
        // Calculate waiting time for each process
        int waitingTime = current->data.completionTime - current->data.burstTime - current->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current = current->next;
    }

    // Calculate average waiting time
    return totalWaitingTime / numberOfProcesses;
}

float calculateMLFQAverageWaitingTime(struct Node *queue1, struct Node *queue2) {
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current1 = queue1;
    while (current1 != NULL) {
        // Calculate waiting time for each process
        int waitingTime = current1->data.completionTime - current1->data.burstTime - current1->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current1 = current1->next;
    }

    struct Node *current2 = queue2;
    while (current2 != NULL) {
        // Calculate waiting time for each process
        int waitingTime = current2->data.completionTime - current2->data.burstTime - current2->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current2 = current2->next;
    }

    // Calculate average waiting time
    return totalWaitingTime / numberOfProcesses;
}

float calculateAverageTurnaroundTime(struct Node *queue)
{
    float totalTurnaroundTime = 0;
    int numberOfProcesses = 0;

    struct Node *current = queue;
    while (current != NULL)
    {
        // Calculate turnaround time for the process
        int turnaroundTime = current->data.completionTime - current->data.arrivalTime;
        totalTurnaroundTime += turnaroundTime;
        numberOfProcesses++;

        current = current->next;
    }

    // Calculate average turnaround time
    return totalTurnaroundTime / numberOfProcesses;
}

float calculateMLFQAverageTurnaroundTime(struct Node *queue1, struct Node *queue2)
{
    float totalTurnaroundTime = 0;
    int numberOfProcesses = 0;

    struct Node *current1 = queue1;
    struct Node *current2 = queue2;
    while (current1 != NULL)
    {
        // Calculate turnaround time for the process
        int turnaroundTime = current1->data.completionTime - current1->data.arrivalTime;
        totalTurnaroundTime += turnaroundTime;
        numberOfProcesses++;

        current1 = current1->next;
    }

    while (current2 != NULL)
    {
        // Calculate turnaround time for the process
        int turnaroundTime = current2->data.completionTime - current2->data.arrivalTime;
        totalTurnaroundTime += turnaroundTime;
        numberOfProcesses++;

        current2 = current2->next;
    }

    // Calculate average turnaround time
    return totalTurnaroundTime / numberOfProcesses;
}

float calculateThroughput(struct Node *queue)
{
    int numberOfProcesses = 0;
    int totalTime = 0;

    struct Node *current = queue;
    while (current != NULL)
    {
        numberOfProcesses++;
        totalTime = (current->data.completionTime > totalTime) ? current->data.completionTime : totalTime;
        current = current->next;
    }

    // Calculate throughput
    float throughput = (float)numberOfProcesses / totalTime;

    return throughput;
}

float calculateMLFQThroughput(struct Node *queue1, struct Node *queue2)
{
    int numberOfProcesses = 0;
    int totalTime = 0;

    struct Node *current1 = queue1;
    struct Node *current2 = queue2;
    while (current1 != NULL)
    {
        numberOfProcesses++;
        totalTime = (current1->data.completionTime > totalTime) ? current1->data.completionTime : totalTime;
        current1 = current1->next;
    }

    while (current2 != NULL)
    {
        numberOfProcesses++;
        totalTime = (current2->data.completionTime > totalTime) ? current2->data.completionTime : totalTime;
        current2 = current2->next;
    }

    // Calculate throughput
    float throughput = (float)numberOfProcesses / totalTime;

    return throughput;
}



int main()
{
    int choice = 0;
    int n, quantumRR;
    float avgTime, turnaroundTime, throughput;

    struct Node *rrQueue = NULL;   // Interactive RR queue
    struct Node *fcfsQueue = NULL; // FCFS queue

    // Input process details and add them to the RR queue
    // ...

    printMenu();
    printf("Enter your choice: ");
    scanf("%d", &choice);

    printf("Enter the number of process:\n");
    scanf("%d", &n);
    struct Process process;
    struct Process processes[n];

    for (int i = 0; i < n; i++) {
        process.id = i + 1;
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &process.arrivalTime);
        printf("Enter burst time for process %d: ", i + 1);
        scanf("%d", &process.burstTime);
        process.remainingTime = process.burstTime;
        if(choice == 1 || choice == 2) {
            addToLinkedList(&fcfsQueue, process);
        } else {
            addToLinkedList(&rrQueue, process);
        }
        processes[i].id = process.id;
        processes[i].arrivalTime = process.arrivalTime;
        processes[i].burstTime = process.burstTime;
    }

    switch (choice) {
        case 1:
            struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
            newNode->data = process;
            newNode->next = NULL;

            if (fcfsQueue == NULL) {
                fcfsQueue = newNode;
            } else {
                struct Node* current = fcfsQueue;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = newNode;
            }
            sortQueueByArrivalTime(&fcfsQueue);
            fcfs(fcfsQueue);
            // Print the process details including completion time
            printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
            struct Node* current = fcfsQueue;
            while (current != NULL) {
                printf("%d\t\t%d\t\t%d\t\t%d\n", current->data.id, current->data.arrivalTime, current->data.burstTime, current->data.completionTime);
                current = current->next;
            }
            avgTime = calculateAverageWaitingTime(fcfsQueue);
            printf("Average Waiting Time: %f\n", avgTime);
            turnaroundTime = calculateAverageTurnaroundTime(fcfsQueue);
            printf("Turnaround Time: %f\n", turnaroundTime);
            throughput = calculateThroughput(fcfsQueue);
            printf("Throughput: %f\n", throughput);
            break;

        case 2:
            float totalWaitingTime = 0;
            float totalTurnAroundTime = 0;
            int numberOfProcesses = 0;
            float totalTime = 0;
            printf("Enter the time quantum:\n");
            scanf("%d", &quantumRR);
            sortArrivalTime(processes, n);
            // Call the Round Robin scheduler
            roundRobin(processes, n, quantumRR);
            printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
            for (int i = 0; i < n; i++)
            {
                printf("%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].arrivalTime, processes[i].burstTime, processes[i].completionTime);
                int waitingTime = processes[i].completionTime - processes[i].burstTime - processes[i].arrivalTime;
                totalWaitingTime += waitingTime;
                int tatTime = processes[i].completionTime - processes[i].arrivalTime;
                totalTurnAroundTime += tatTime;
                totalTime = processes[i].completionTime;
                numberOfProcesses++;
            }
            avgTime = totalWaitingTime / numberOfProcesses;
            printf("Average Waiting Time: %f\n", avgTime);
            turnaroundTime = totalTurnAroundTime / numberOfProcesses;
            printf("Turnaround time: %f\n", turnaroundTime);
            throughput = numberOfProcesses / totalTime;
            printf("Throughput: %f\n", throughput);
            break;

        case 3:
            printf("Enter the time quantum:\n");
            scanf("%d", &quantumRR);
            printQueue(rrQueue, "Interactive RR Queue");
            printQueue(fcfsQueue, "FCFS Queue");

            //To Be implement: sort function to sort the rrQueue according to the arrivalTime
            sortQueueByArrivalTime(&rrQueue);

            //Currently the roundrobin function is not affected by the arrival time, may need to implement it
            roundRobin1(&rrQueue,&fcfsQueue,quantumRR);

            fcfs(fcfsQueue);

            printQueue(rrQueue, "Interactive RR Queue");
            printQueue(fcfsQueue, "FCFS Queue");

            printResults(rrQueue, fcfsQueue);
            avgTime = calculateMLFQAverageWaitingTime(rrQueue, fcfsQueue);
            printf("Average Waiting Time: %f\n", avgTime);
            turnaroundTime = calculateMLFQAverageTurnaroundTime(rrQueue, fcfsQueue);
            printf("Average Turnaround Time %f\n", turnaroundTime);
            throughput = calculateMLFQThroughput(rrQueue, fcfsQueue);
            printf("Throughput: %f\n", throughput);
            break;

        default:
            printf("Invalid choice. Please choose again.\n");
            break;
    }

    return 0;
}