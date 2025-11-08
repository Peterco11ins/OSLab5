//implementation of Scheduling Simulation 
#include <stdio.h> 
#include <limits.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"

// ================= ROUND ROBIN ==================
void findWaitingTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    int rem_bt[n];
    for (int i = 0; i < n; i++)
        rem_bt[i] = plist[i].bt;

    int t = 0;
    while (1) {
        int done = 1;
        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                done = 0;
                if (rem_bt[i] > quantum) {
                    t += quantum;
                    rem_bt[i] -= quantum;
                } else {
                    t += rem_bt[i];
                    plist[i].wt = t - plist[i].bt - plist[i].art;
                    if (plist[i].wt < 0)
                        plist[i].wt = 0;
                    rem_bt[i] = 0;
                }
            }
        }
        if (done == 1)
            break;
    }
} 

// ================= SJF (Non-Preemptive) ==================
void findWaitingTimeSJF(ProcessType plist[], int n)
{ 
    int completed = 0, current_time = 0;
    int is_completed[n];
    for (int i = 0; i < n; i++)
        is_completed[i] = 0;

    while (completed != n) {
        int idx = -1;
        int min_bt = INT_MAX;

        // find process with shortest burst time that has arrived
        for (int i = 0; i < n; i++) {
            if (plist[i].art <= current_time && is_completed[i] == 0) {
                if (plist[i].bt < min_bt) {
                    min_bt = plist[i].bt;
                    idx = i;
                }
                // if tie, choose the earlier arrival
                if (plist[i].bt == min_bt) {
                    if (plist[i].art < plist[idx].art)
                        idx = i;
                }
            }
        }

        if (idx != -1) {
            plist[idx].wt = current_time - plist[idx].art;
            if (plist[idx].wt < 0)
                plist[idx].wt = 0;

            current_time += plist[idx].bt;
            is_completed[idx] = 1;
            completed++;
        } else {
            current_time++;
        }
    }
} 

// ================= FCFS ==================
void findWaitingTime(ProcessType plist[], int n)
{ 
    plist[0].wt = 0 + plist[0].art; 
    for (int i = 1; i < n; i++) 
        plist[i].wt = plist[i-1].bt + plist[i-1].wt; 
} 
  
void findTurnAroundTime(ProcessType plist[], int n)
{ 
    for (int i = 0; i < n; i++) 
        plist[i].tat = plist[i].bt + plist[i].wt; 
} 

// ================= PRIORITY ==================
int my_comparer(const void *this, const void *that)
{ 
    const ProcessType *p1 = (const ProcessType *)this;
    const ProcessType *p2 = (const ProcessType *)that;

    // Lower number = higher priority
    if (p1->pri < p2->pri)
        return -1;
    else if (p1->pri > p2->pri)
        return 1;
    else
        return 0;
} 

void findavgTimeFCFS(ProcessType plist[], int n) 
{ 
    findWaitingTime(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nFCFS\n");
}

void findavgTimeSJF(ProcessType plist[], int n) 
{ 
    findWaitingTimeSJF(plist, n); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nSJF\n");
}

void findavgTimeRR(ProcessType plist[], int n, int quantum) 
{ 
    findWaitingTimeRR(plist, n, quantum); 
    findTurnAroundTime(plist, n); 
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

void findavgTimePriority(ProcessType plist[], int n) 
{ 
    qsort(plist, n, sizeof(ProcessType), my_comparer);
    findWaitingTime(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nPriority\n");
}

// ================= METRICS ==================
void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0; 
    float awt, att;
    
    printf("\tProcesses\tBurst time\tWaiting time\tTurn around time\n"); 
    for (int i = 0; i < n; i++) { 
        total_wt += plist[i].wt; 
        total_tat += plist[i].tat; 
        printf("\t%d\t\t%d\t\t%d\t\t%d\n", plist[i].pid, plist[i].bt, plist[i].wt, plist[i].tat); 
    } 
  
    awt = ((float)total_wt / (float)n);
    att = ((float)total_tat / (float)n);
    
    printf("\nAverage waiting time = %.2f", awt); 
    printf("\nAverage turn around time = %.2f\n", att); 
} 

// ================= INIT & MAIN ==================
ProcessType * initProc(char *filename, int *n) 
{
  	FILE *input_file = fopen(filename, "r");
	if (!input_file) {
		fprintf(stderr, "Error: Invalid filepath\n");
		fflush(stdout);
		exit(0);
	}

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}
  
int main(int argc, char *argv[]) 
{ 
    int n; 
    int quantum = 2;
    ProcessType *proc_list;
  
    if (argc < 2) {
		fprintf(stderr, "Usage: ./schedsim <input-file-path>\n");
		fflush(stdout);
		return 1;
	}

    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeFCFS(proc_list, n);
    printMetrics(proc_list, n);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeSJF(proc_list, n); 
    printMetrics(proc_list, n);

    // PRIORITY
    n = 0; 
    proc_list = initProc(argv[1], &n);
    findavgTimePriority(proc_list, n); 
    printMetrics(proc_list, n);
    
    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    findavgTimeRR(proc_list, n, quantum); 
    printMetrics(proc_list, n);
    
    return 0; 
}