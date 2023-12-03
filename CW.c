#include <stdio.h>
#include <stdlib.h>
#include <math.h>
struct Process
{
    int id;
    int arrivalTime;
    int burstTime;
    int remainingTime;
    int completionTime;
    int MLFQrrCompletionTime;
};

struct Node
{
    struct Process data;
    struct Node *next;
};

float totalWaitingTime = 0;
float totalTurnAroundTime = 0;
int numberOfProcesses = 0;
float totalTime = 0;

int Fcfstime;

void deleteNode(struct Node **head, int id)
{
    if (head == NULL)
    {
        printf("Error: Attempting to delete a NULL node.\n");
        return;
    }

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

void printMenu()
{
    printf("Choose a scheduling algorithm:\n");
    printf("1. First-Come-First-Serve (FCFS)\n");
    printf("2. Round Robin (RR)\n");
    printf("3. Multi-Level Feedback Queue (MLFQ)\n");
}

void fcfs(struct Node *head)
{
    if (head == NULL)
    {
        printf("No processes to schedule.\n");
        return;
    }

    struct Node *current = head;
    int currentTime = 0;

    while (current != NULL)
    {
        if (current->data.arrivalTime > currentTime)
        {
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

void MLFQfcfs(struct Node *head)
{
    if (head == NULL)
    {
        printf("No processes to schedule.\n");
        return;
    }

    struct Node *current = head;
    // printf("%d\n", MLFQtime);

    while (current != NULL)
    {
        // temp = current->data.remainingTime;
        if (current->data.remainingTime == 0)
        {
            current = current->next;
            continue;
        }
        // Update completionTime if the process is completed
        //   if (temp != 0){
        if (current->data.MLFQrrCompletionTime <= Fcfstime)
        {
            current->data.completionTime = Fcfstime + current->data.remainingTime;
            Fcfstime = current->data.completionTime;
        }
        else
        {
            current->data.completionTime += current->data.remainingTime + current->data.MLFQrrCompletionTime;
            Fcfstime = current->data.completionTime;
        }
        // addToLinkedList(fcfsResultQueue,current->data);
        //     }
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
            if (remainingTime[i] > 0)
            {
                done = 0; // Not all processes are done

                while (currentTime < processes[i].arrivalTime)
                {
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

void MLFQroundRobinNew(struct Node **RRqueue, struct Node **FCFSqueue, struct Node **rrResultQueue, int quantum)
{

    // Implementation of Round Robin scheduling
    struct Node *current = *RRqueue;
    struct Node *temp = NULL; // this is important, dont remove this
    int tempQuantum;

    int MLFQtime = current->data.arrivalTime;

    while (current != NULL)
    {

        if (current->data.arrivalTime > MLFQtime)
        {
            MLFQtime++;
            continue;
        }

        while (1)
        {
            tempQuantum = quantum;

            while (1)
            {

                //  while(tempQuantum != 0 || current->data.remainingTime !=0){
                current->data.remainingTime = current->data.remainingTime - 1;
                tempQuantum--;
                MLFQtime++;

                if (tempQuantum == 0)
                {
                    break;
                }

                if (current->data.remainingTime == 0)
                {

                    break;
                }
            }

            if (current != NULL && current->data.remainingTime == 0)
            {
                break;
            }

            if (current->next != NULL && current->next->data.arrivalTime <= MLFQtime)
            {
                break;
            }
        }

        if (current->data.remainingTime != 0)
        {
            current->data.MLFQrrCompletionTime = MLFQtime;
            addToLinkedList(FCFSqueue, current->data);
        }
        else
        {
            current->data.MLFQrrCompletionTime = MLFQtime;
            addToLinkedList(FCFSqueue, current->data);
            // add to result queue
        }
        // temp = current->next;
        addToLinkedList(rrResultQueue, current->data);
        // deleteNode(RRqueue,current->data.id);
        current = current->next;
    }
}

int compareByArrivalTime(const void *a, const void *b)
{
    return ((struct Process *)a)->arrivalTime - ((struct Process *)b)->arrivalTime;
}

void sortQueueByArrivalTime(struct Node **queue)
{
    int n = 0;
    struct Node *current = *queue;

    // Count the number of elements in the queue
    while (current != NULL)
    {
        n++;
        current = current->next;
    }

    // Create an array of processes to sort
    struct Process *processes = (struct Process *)malloc(n * sizeof(struct Process));
    current = *queue;

    for (int i = 0; i < n; i++)
    {
        processes[i] = current->data;
        current = current->next;
    }

    // Sort the array by arrival time
    qsort(processes, n, sizeof(struct Process), compareByArrivalTime);

    // Reconstruct the sorted queue
    current = *queue;
    for (int i = 0; i < n; i++)
    {
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
    if (current == NULL)
    {
        printf("%s", "Queue is Empty\n\n");
    }
    while (current != NULL)
    {
        printf("Process with Id %d have remaining time %d \n", current->data.id, current->data.remainingTime);
        current = current->next;
    }
}

float AverageWaitingTimeMLFQ_RR(struct Node *rrResultQueue)
{
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current = rrResultQueue;
    while (current != NULL)
    {
        // Calculate waiting time for each process
        int waitingTime = current->data.MLFQrrCompletionTime - (current->data.burstTime - current->data.remainingTime) - current->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current = current->next;
    }

    // Calculate average waiting time
    return totalWaitingTime / numberOfProcesses;
}

float AverageWaitingTimeMLFQ_FCFS(struct Node *fcfsResultQueue)
{
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current = fcfsResultQueue;
    while (current != NULL)
    {

        if (current->data.completionTime == 0)
        {
            current = current->next;
            continue;
        }

        // Calculate waiting time for each process
        int waitingTime = current->data.completionTime - (current->data.MLFQrrCompletionTime - current->data.remainingTime);
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current = current->next;
    }

    // Calculate average waiting time
    return totalWaitingTime / numberOfProcesses;
}

float AverageTurnaroundTimeMLFQ_RR(float averageWaitingTime_MLFQ_RR, struct Node *rrResultQueue)
{
    struct Node *current = rrResultQueue;
    int totalBurstTime = 0;
    while (current != NULL)
    {
        totalBurstTime += current->data.burstTime - current->data.remainingTime;
        current = current->next;
    }

    return averageWaitingTime_MLFQ_RR + totalBurstTime;
}

float AverageTurnaroundTimeMLFQ_FCFS(float averageWaitingTime_MLFQ_FCFS, struct Node *fcfsResultQueue)
{
    struct Node *current = fcfsResultQueue;
    int totalBurstTime = 0;
    while (current != NULL)
    {
        if (current->data.completionTime == 0)
        {
            current = current->next;
            continue;
        }
        totalBurstTime += current->data.burstTime - current->data.remainingTime;
        current = current->next;
    }

    return averageWaitingTime_MLFQ_FCFS + totalBurstTime;
}

float AverageWaitingTimeMLFQ(float averageWaitingTime_MLFQ_RR, float averageWaitingTime_MLFQ_FCFS)
{
    if (isnan(averageWaitingTime_MLFQ_FCFS))
    {
        return averageWaitingTime_MLFQ_RR;
    }
    else
    {
        return (averageWaitingTime_MLFQ_RR + averageWaitingTime_MLFQ_FCFS) / 2;
    }
}

float AverageTurnaroundTimeMLFQ(float averageWaitingTime_MLFQ, struct Node *rrResultQueue)
{
    struct Node *current = rrResultQueue;
    int totalBurstTime = 0;
    while (current != NULL)
    {
        totalBurstTime += current->data.burstTime;
        current = current->next;
    }

    return averageWaitingTime_MLFQ + totalBurstTime;
}

float ThroughputMLFQ_RR(struct Node *rrResultQueue)
{
    int totalProcess = 0;
    int startTime = 0;
    int endTime = 0;
    int totalTime = 0;

    struct Node *current = rrResultQueue;
    startTime = current->data.arrivalTime;
    while (current != NULL)
    {

        totalProcess++;
        if (current->next == NULL)
        {
            endTime = current->data.MLFQrrCompletionTime;
        }
        current = current->next;
    }
    totalTime = (endTime - startTime);

    return totalTime / totalProcess;
}

float ThroughputMLFQ_FCFS(struct Node *fcfsResultQueue)
{
    int totalProcess = 0;
    int startTime = 0;
    int endTime = 0;
    int totalTime = 0;
    struct Node *current = fcfsResultQueue;


    while (current != NULL)
    {
    if(current->data.completionTime == 0){
        current = current->next;
        continue;
    }

    if(startTime == 0){
     startTime = current->data.MLFQrrCompletionTime;
    }


        totalProcess++;
        if (current->next->data.completionTime == 0)
        {
            endTime = current->data.completionTime;
            break;
        }
        current = current->next;
    }
    totalTime = (endTime - startTime);
    return totalTime / totalProcess;
}
float ThroughputMLFQ(struct Node *rrResultQueue)
{
    int totalProcess = 0;
    int startTime = 0;
    int endTime = 0;
    int totalTime = 0;
    struct Node *current = rrResultQueue;
    startTime = current->data.arrivalTime;
    while (current != NULL)
    {

        totalProcess++;
        if (current->next == NULL)
        {
            endTime = current->data.MLFQrrCompletionTime;
            break;
        }
        current = current->next;
    }
    totalTime = (endTime - startTime);
    return totalTime / totalProcess;
}

void printResults(struct Node *rrResultQueue, struct Node *fcfsResultQueue)
{
    printf("RR in MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");

    // Print MLFQRR Queue results
    struct Node *currentRR = rrResultQueue;
    while (currentRR != NULL)
    {
        printf("%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentRR->data.MLFQrrCompletionTime);
        currentRR = currentRR->next;
    }

    // Print MLFQFCFS Queue results
    printf("\n");
    printf("FCFS in MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
    struct Node *currentFCFS = fcfsResultQueue;
    // if currentFCFS->data.completionTime == 0, means that it is already finish inside RR,
    // thus, no need to print it

    while (currentFCFS != NULL)
    {
        if (currentFCFS->data.completionTime == 0)
        {
            currentFCFS = currentFCFS->next;
            continue;
        }
        printf("%d\t\t%d\t\t%d\t\t%d\n", currentFCFS->data.id, currentFCFS->data.MLFQrrCompletionTime, currentFCFS->data.remainingTime, currentFCFS->data.completionTime);
        currentFCFS = currentFCFS->next;
    }

    // Print MLFQ Queue results
    printf("Overall MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
    currentFCFS = fcfsResultQueue;
    currentRR = rrResultQueue;
    while (currentRR != NULL)
    {
        if (currentFCFS->data.completionTime == 0)
        {
            printf("%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentRR->data.MLFQrrCompletionTime);
            currentFCFS = currentFCFS->next;
            currentRR = currentRR->next;
        }
        else
        {
            printf("%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentFCFS->data.completionTime);
            currentFCFS = currentFCFS->next;
            currentRR = currentRR->next;
        }
    }
}

float calculateAverageWaitingTime(struct Node *queue)
{
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current = queue;
    while (current != NULL)
    {
        // Calculate waiting time for each process
        int waitingTime = current->data.completionTime - current->data.burstTime - current->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current = current->next;
    }

    // Calculate average waiting time
    return totalWaitingTime / numberOfProcesses;
}

float calculateMLFQAverageWaitingTime(struct Node *queue1, struct Node *queue2)
{
    float totalWaitingTime = 0;
    int numberOfProcesses = 0;

    struct Node *current1 = queue1;
    while (current1 != NULL)
    {
        // Calculate waiting time for each process
        int waitingTime = current1->data.completionTime - current1->data.burstTime - current1->data.arrivalTime;
        totalWaitingTime += waitingTime;
        numberOfProcesses++;

        current1 = current1->next;
    }

    struct Node *current2 = queue2;
    while (current2 != NULL)
    {
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
    int currentMLFQTime;
    int choice = 0;
    int n, quantumRR;
    float avgTime, turnaroundTime, throughput;

    struct Node *rrQueue = NULL;         // Interactive RR queue
    struct Node *fcfsQueue = NULL;       // FCFS queue
    struct Node *rrResultQueue = NULL;   // Interactive RR queue
    struct Node *fcfsResultQueue = NULL; // FCFS queue

    // Input process details and add them to the RR queue
    // ...

    printMenu();
    do
    {
        printf("Enter your choice (1, 2, or 3): ");
        scanf("%d", &choice);

        if (choice < 1 || choice > 3)
        {
            printf("Invalid choice. Please enter 1, 2, or 3.\n");
        }

    } while (choice < 1 || choice > 3);

    printf("Enter the number of process:\n");
    scanf("%d", &n);
    struct Process process;
    struct Process processes[n];

    for (int i = 0; i < n; i++)
    {
        process.id = i + 1;
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &process.arrivalTime);
        printf("Enter burst time for process %d: ", i + 1);
        scanf("%d", &process.burstTime);
        process.remainingTime = process.burstTime;
        if (choice == 1)
        {
            addToLinkedList(&fcfsQueue, process);
        }
        else
        {
            addToLinkedList(&rrQueue, process);
        }
        processes[i].id = process.id;
        processes[i].arrivalTime = process.arrivalTime;
        processes[i].burstTime = process.burstTime;
    }

    switch (choice)
    {
    case 1:
        sortQueueByArrivalTime(&fcfsQueue);
        fcfs(fcfsQueue);
        // Print the process details including completion time
        printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
        struct Node *current = fcfsQueue;
        while (current != NULL)
        {
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
        // printQueue(rrQueue, "Interactive RR Queue");
        // printQueue(fcfsQueue, "FCFS Queue");

        // To Be implement: sort function to sort the rrQueue according to the arrivalTime
        sortQueueByArrivalTime(&rrQueue);

        // Currently the roundrobin function is not affected by the arrival time, may need to implement it
        MLFQroundRobinNew(&rrQueue, &fcfsQueue, &rrResultQueue, quantumRR);

        //        // printQueue(rrQueue, "Interactive RR Queue");
        //        // printQueue(fcfsQueue, "FCFS Queue");

        Fcfstime = fcfsQueue->data.MLFQrrCompletionTime;
        MLFQfcfs(fcfsQueue);

        printResults(rrResultQueue, fcfsQueue);

        float AverageWaitingTime_MLFQ_RR, AverageTurnAroundTime_MLFQ_RR, Throughput_MLFQ_RR;

        AverageWaitingTime_MLFQ_RR = AverageWaitingTimeMLFQ_RR(rrResultQueue);
        printf("Average Waiting Time MLFQ_RR: %f\n", AverageWaitingTime_MLFQ_RR);

        AverageTurnAroundTime_MLFQ_RR = AverageTurnaroundTimeMLFQ_RR(AverageWaitingTime_MLFQ_RR, rrResultQueue);
        printf("Average Turnaround Time MLFQ_RR: %f\n", AverageTurnAroundTime_MLFQ_RR);

        Throughput_MLFQ_RR = ThroughputMLFQ_RR(rrResultQueue);
        printf("Throughput: %f\n", Throughput_MLFQ_RR);

        float AverageWaitingTime_MLFQ_FCFS, AverageTurnAroundTime_MLFQ_FCFS, Throughput_MLFQ_FCFS;

        AverageWaitingTime_MLFQ_FCFS = AverageWaitingTimeMLFQ_FCFS(fcfsQueue);
        printf("Average Waiting Time MLFQ_FCFS: %f\n", AverageWaitingTime_MLFQ_FCFS);

        AverageTurnAroundTime_MLFQ_FCFS = AverageTurnaroundTimeMLFQ_FCFS(AverageWaitingTime_MLFQ_FCFS, fcfsQueue);
        printf("Average Turnaround Time MLFQ_FCFS: %f\n", AverageTurnAroundTime_MLFQ_FCFS);

        Throughput_MLFQ_FCFS = ThroughputMLFQ_FCFS(fcfsQueue);
        printf("Throughput: %f\n", Throughput_MLFQ_FCFS);

        float AverageWaitingTime_MLFQ, AverageTurnAroundTime_MLFQ, Throughput_MLFQ;

        AverageWaitingTime_MLFQ = AverageWaitingTimeMLFQ(AverageWaitingTime_MLFQ_RR, AverageWaitingTime_MLFQ_FCFS);
        printf("Average Waiting Time MLFQ: %f\n", AverageWaitingTime_MLFQ);

        AverageTurnAroundTime_MLFQ = AverageTurnaroundTimeMLFQ(AverageWaitingTime_MLFQ, rrResultQueue);
        printf("Average Turnaround Time MLFQ: %f\n", AverageTurnAroundTime_MLFQ);

        Throughput_MLFQ = ThroughputMLFQ(rrQueue);
        printf("Throughput: %f\n", Throughput_MLFQ);
        // throughput = calculateMLFQThroughput(rrQueue, fcfsQueue);
        // printf("Throughput: %f\n", throughput);
        break;

    default:
        printf("Invalid choice. Please choose again.\n");
        break;
    }

    return 0;
}