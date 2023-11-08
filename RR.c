/* Round Robin, pre-emptive version of FCFS that forces context switches at time slices
 Advantages:
 - Improved response time
 -Effective for general purpose interactive/time sharing systems
 Disadvantages:
 - Increased context switching and thus overhead
 -Favours CPU bound processes (which usually run long) over I/O processes (which do not run long)
 -Can be prevented by working with multiple queues?
 - Can reduce to FCFS */

/* Waiting Time: Time Difference between turnaround time and burst time.

Waiting Time = Turnaround Time – Burst Time */

/* Turnaround Time: Difference between completion time and arrival time.

Turnaround Time = Completion Time – Arrival Time */

/* this program assumen that input process is in order */
#include <stdio.h>

// Structure to represent a process

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

// Function to sort processes by arrival time
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

int main()
{
    int n, quantum;

    printf("Enter the number of processes: ");
    scanf("%d", &n);

    struct Process processes[n];

    // Input process details
    for (int i = 0; i < n; i++)
    {
        processes[i].id = i + 1;
        printf("Enter arrival time for process %d: ", i + 1);
        scanf("%d", &processes[i].arrivalTime);
        printf("Enter burst time for process %d: ", i + 1);
        scanf("%d", &processes[i].burstTime);
    }

    

    printf("Enter time quantum: ");
    scanf("%d", &quantum);

    sortArrivalTime(processes, n);
    
    // Call the Round Robin scheduler
    roundRobin(processes, n, quantum);

    for (int i = 0; i < n; i++)
    {
        printf("Process %d completed at time %d\n", processes[i].id, processes[i].completionTime);
    }

    return 0;
}
