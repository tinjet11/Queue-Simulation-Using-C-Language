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
int MLFQtime;


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

struct Node *findNodeById(struct Node **RRqueue, int id)
{
    struct Node *current = *RRqueue;

    while (current != NULL)
    {
        if (current->data.id == id)
        {
            // Node with the matching ID found
            return current;
        }
        current = current->next;
    }

    // ID not found in the linked list
    return NULL;
}

void MLFQfcfs(struct Node **FCFSqueue,struct Node **RRqueue)
{
    if (FCFSqueue == NULL)
    {
        printf("No processes to schedule.\n");
        return;
    }

    struct Node *current = *FCFSqueue;
    struct Node *rrcurrent = NULL;

    rrcurrent = findNodeById(RRqueue,current->data.id);

        //update MLFQTime
        MLFQtime += current->data.remainingTime;

        //update the completion time of process
        rrcurrent->data.completionTime = MLFQtime;

        //Delete the node from fcfs
        deleteNode(FCFSqueue,current->data.id);
    
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

void MLFQroundRobin(struct Node **RRqueue, struct Node **FCFSqueue, int quantum)
{

    // Implementation of Round Robin scheduling
    struct Node *current = *RRqueue;
    struct Node *temp = NULL; // this is important, dont remove this

    MLFQtime = current->data.arrivalTime;

    while (current != NULL)
    {
        //to get the latest current time
        if (current->data.arrivalTime > MLFQtime)
        {
            MLFQtime++;
            continue;
        }

        // Execute the process for the quantum or its remaining time, whichever is smaller
        int executeTime = (current->data.remainingTime < quantum) ? current->data.remainingTime : quantum;
        current->data.remainingTime -= executeTime;
        MLFQtime += executeTime;

        // if process not yet finish in one time quantum, will go inside fcfs
        if (current->data.remainingTime != 0)
        {
            current->data.MLFQrrCompletionTime = MLFQtime;

            // if next process haven't arrive
            if (current->next != NULL && current->next->data.arrivalTime > MLFQtime)
            {
                // simulate the fcfs run by adding the remaining time to MLFQtime
                addToLinkedList(FCFSqueue,current->data);

                while(current->next->data.arrivalTime > MLFQtime && current->next != NULL && *FCFSqueue != NULL){
                    MLFQfcfs(FCFSqueue,RRqueue);
                }
            }
            // next process in rr is ready already
            else
            {
                // put inside fcfs queue, but not run
                addToLinkedList(FCFSqueue,current->data);
            }
        }
        //process that finish in one time quantum
        //will have same MLFQrrCompletionTime and completionTime
        else
        {
            current->data.MLFQrrCompletionTime = MLFQtime;
            current->data.completionTime = MLFQtime;
        }
        current = current->next;
    }    

    //after finishing the round robin
    //execute whatever inside fcfs until it is empty 
    while(*FCFSqueue != NULL){
         MLFQfcfs(FCFSqueue,RRqueue);
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

void printResults(struct Node *rrResultQueue,int quantumRR)
{
    printf("RR in MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\tTurnaroundTime\tWaitingTime\n");

    // Print MLFQRR Queue results
    struct Node *currentRR = rrResultQueue;
    int turnaroundTime = 0;
    double totalTime = 0;
    int waitingTime = 0;
    int totalWaitingTime = 0;
    int totalTurnaroundTime = 0;
    double processNum = 0;

    while (currentRR != NULL)
    {
        turnaroundTime = currentRR->data.MLFQrrCompletionTime - currentRR->data.arrivalTime;
        waitingTime = ((turnaroundTime - quantumRR) < 0 ) ? 0 : turnaroundTime - quantumRR ;
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentRR->data.MLFQrrCompletionTime,turnaroundTime,waitingTime);
        currentRR = currentRR->next;
    }

    // Print MLFQFCFS Queue results
    printf("\n");
    printf("FCFS in MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\tTurnaroundTime\tWaitingTime\n");
    struct Node *currentFCFS = rrResultQueue;

    while (currentFCFS != NULL)
    {
        if (currentFCFS->data.completionTime ==currentFCFS->data.MLFQrrCompletionTime)
        {
            currentFCFS = currentFCFS->next;
            continue;
        }
        turnaroundTime = currentFCFS->data.completionTime - currentFCFS->data.MLFQrrCompletionTime;
        waitingTime = turnaroundTime - currentFCFS->data.remainingTime;
        printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", currentFCFS->data.id, currentFCFS->data.MLFQrrCompletionTime, currentFCFS->data.remainingTime, currentFCFS->data.completionTime,turnaroundTime,waitingTime);
        currentFCFS = currentFCFS->next;
    }

    // Print MLFQ Queue results
    printf("Overall MLFQ \n");
    printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\tTurnaroundTime\tWaitingTime\n");
    currentRR = rrResultQueue;
    while (currentRR != NULL)
    {        
            turnaroundTime = currentRR->data.completionTime - currentRR->data.arrivalTime;
            waitingTime = turnaroundTime - currentRR->data.burstTime;
            printf("%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", currentRR->data.id, currentRR->data.arrivalTime, currentRR->data.burstTime, currentRR->data.completionTime,turnaroundTime,waitingTime);
            processNum++;//calculate number of process
            totalWaitingTime += waitingTime;//calculate total waiting time
            totalTurnAroundTime += turnaroundTime;//calculate total turnaround time
            totalTime = (currentRR->data.completionTime > totalTime) ? currentRR->data.completionTime : totalTime;//if completion time bigger then get completion time else no
            currentRR = currentRR->next;
    }
    double avgWaitingTime = totalWaitingTime/processNum;
    double avgTurnAroundTime = totalTurnAroundTime/processNum;
    double throughput = processNum/totalTime;
    printf("Average waiting Time: %.3f\n", avgWaitingTime);
    printf("Average turnaround Time: %.3f\n", avgTurnAroundTime);
    printf("Throughput: %.3f\n", throughput);
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
        printf("Average Waiting Time: %.3f\n", avgTime);
        turnaroundTime = calculateAverageTurnaroundTime(fcfsQueue);
        printf("Turnaround Time: %.3f\n", turnaroundTime);
        throughput = calculateThroughput(fcfsQueue);
        printf("Throughput: %.3f\n", throughput);
        break;

    case 2:

        printf("Enter the time quantum:\n");
        scanf("%d", &quantumRR);
        sortArrivalTime(processes, n);
        // Call the Round Robin scheduler
        roundRobin(processes, n, quantumRR);
        printf("Process ID\tArrival Time\tBurst Time\tCompletion Time\n");
        //print the results
        for (int i = 0; i < n; i++)
        {
            printf("%d\t\t%d\t\t%d\t\t%d\n", processes[i].id, processes[i].arrivalTime, processes[i].burstTime, processes[i].completionTime);
            int waitingTime = processes[i].completionTime - processes[i].burstTime - processes[i].arrivalTime;
            //calculate total waiting time
            totalWaitingTime += waitingTime;
            int tatTime = processes[i].completionTime - processes[i].arrivalTime;
            //calculate total turnaround time, total time and number of process
            totalTurnAroundTime += tatTime;
            totalTime = processes[i].completionTime;
            numberOfProcesses++;
        }
        avgTime = totalWaitingTime / numberOfProcesses;
        printf("Average Waiting Time: %.3f\n", avgTime);
        turnaroundTime = totalTurnAroundTime / numberOfProcesses;
        printf("Turnaround time: %.3f\n", turnaroundTime);
        throughput = numberOfProcesses / totalTime;
        printf("Throughput: %.3f\n", throughput);
        break;

    case 3:
        printf("Enter the time quantum:\n");
        scanf("%d", &quantumRR);

        sortQueueByArrivalTime(&rrQueue);

        MLFQroundRobin(&rrQueue, &fcfsQueue,quantumRR);

        printResults(rrQueue,quantumRR);

        break;

    default:
        printf("Invalid choice. Please choose again.\n");
        break;
    }

    return 0;
}
