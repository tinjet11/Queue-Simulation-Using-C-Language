/* First Come First Served (FCFS), 
non-preemptive algorithm (process are only interupted voluntarily)
- Advantages: positional fairness and easy to implement  
- Disadvantages: Favours long processes over short ones, could compromise */


/* Waiting Time: Time Difference between turnaround time and burst time.

Waiting Time = Turnaround Time – Burst Time */

/* Turnaround Time: Difference between completion time and arrival time.

Turnaround Time = Completion Time – Arrival Time */


#include <stdio.h>
int main()
{
    int pid[15];
    int bt[15];
    int n;
    printf("Enter the number of processes: ");
    scanf("%d",&n);
 
    printf("Enter process id of all the processes: \n");
    for(int i=0;i<n;i++)
    {
        printf("Enter process id: ");
        scanf("%d",&pid[i]);
    }
 
    printf("Enter burst time of all the processes: \n");
    for(int i=0;i<n;i++)
    {
        printf("Enter burst time for process with id %d: ",pid[i]);
        scanf("%d",&bt[i]);
    }
 
    int i, wt[n];
    wt[0]=0;
 
    //for calculating waiting time of each process
    for(i=1; i<n; i++)
    {
        wt[i]= bt[i-1]+ wt[i-1];
    }
 
    printf("Process ID     Burst Time     Waiting Time     TurnAround Time\n");
    float twt=0.0;
    float tat= 0.0;
    for(i=0; i<n; i++)
    {
        printf("%d\t\t", pid[i]);
        printf("%d\t\t", bt[i]);
        printf("%d\t\t", wt[i]);
 
        //calculating and printing turnaround time of each process
        printf("%d\t\t", bt[i]+wt[i]);
        printf("\n");
 
        //for calculating total waiting time
        twt += wt[i];
 
        //for calculating total turnaround time
        tat += (wt[i]+bt[i]);
    }
    float att,awt;
 
    //for calculating average waiting time
    awt = twt/n;
 
    //for calculating average turnaround time
    att = tat/n;
    printf("Avg. waiting time= %f\n",awt);
    printf("Avg. turnaround time= %f",att);
}
