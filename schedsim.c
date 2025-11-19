// implementation of Scheduling Simulation
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "process.h"
#include "util.h"

// --------------------- FCFS (with arrival times) ---------------------
void findWaitingTimeFCFS(ProcessType plist[], int n)
{
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        // If CPU is idle until this process arrives
        if (current_time < plist[i].art) {
            current_time = plist[i].art;
        }

        plist[i].wt = current_time - plist[i].art;
        if (plist[i].wt < 0) {
            plist[i].wt = 0;
        }

        current_time += plist[i].bt;
    }
}

// --------------------- SJF (Non-preemptive, with arrival) ---------------------
void findWaitingTimeSJF(ProcessType plist[], int n)
{
    int completed = 0;
    int current_time = 0;
    int is_completed[n];

    for (int i = 0; i < n; i++) {
        is_completed[i] = 0;
    }

    while (completed != n) {
        int idx = -1;
        int min_bt = INT_MAX;

        // find process with shortest burst time that has arrived
        for (int i = 0; i < n; i++) {
            if (plist[i].art <= current_time && is_completed[i] == 0) {
                if (plist[i].bt < min_bt) {
                    min_bt = plist[i].bt;
                    idx = i;
                } else if (plist[i].bt == min_bt && idx != -1) {
                    // tie-breaker: earlier arrival time
                    if (plist[i].art < plist[idx].art) {
                        idx = i;
                    }
                }
            }
        }

        if (idx != -1) {
            plist[idx].wt = current_time - plist[idx].art;
            if (plist[idx].wt < 0) {
                plist[idx].wt = 0;
            }

            current_time += plist[idx].bt;
            is_completed[idx] = 1;
            completed++;
        } else {
            // No process has arrived yet at current_time
            current_time++;
        }
    }
}

// --------------------- SRTF (Preemptive SJF, with arrival) ---------------------
void findWaitingTimeSRTF(ProcessType plist[], int n)
{
    int rt[n];                // remaining times
    for (int i = 0; i < n; i++) {
        rt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    int complete = 0;
    int t = 0;
    int shortest = -1;
    int minm = INT_MAX;
    int finish_time;
    int check = 0;

    while (complete != n) {
        minm = INT_MAX;
        shortest = -1;
        check = 0;

        // find process with smallest remaining time among arrived ones
        for (int j = 0; j < n; j++) {
            if (plist[j].art <= t && rt[j] > 0 && rt[j] < minm) {
                minm = rt[j];
                shortest = j;
                check = 1;
            } else if (plist[j].art <= t && rt[j] > 0 && rt[j] == minm) {
                // tie-breaker: earlier arrival
                if (shortest != -1 && plist[j].art < plist[shortest].art) {
                    shortest = j;
                }
            }
        }

        if (check == 0) {
            // no process is currently ready; CPU idle
            t++;
            continue;
        }

        // run this process for 1 time unit
        rt[shortest]--;
        t++;

        // if this process just finished
        if (rt[shortest] == 0) {
            complete++;
            finish_time = t;
            plist[shortest].wt = finish_time - plist[shortest].bt - plist[shortest].art;
            if (plist[shortest].wt < 0) {
                plist[shortest].wt = 0;
            }
        }
    }
}

// --------------------- Priority (Non-preemptive, with arrival) ---------------------
// Highest priority = largest pri value
void findWaitingTimePriorityAlgo(ProcessType plist[], int n)
{
    int completed = 0;
    int current_time = 0;
    int is_completed[n];

    for (int i = 0; i < n; i++) {
        is_completed[i] = 0;
    }

    while (completed != n) {
        int idx = -1;
        int best_pri = INT_MIN;

        // among arrived processes, pick highest priority (largest pri)
        for (int i = 0; i < n; i++) {
            if (plist[i].art <= current_time && is_completed[i] == 0) {
                if (plist[i].pri > best_pri) {
                    best_pri = plist[i].pri;
                    idx = i;
                } else if (plist[i].pri == best_pri && idx != -1) {
                    // tie-breaker: earlier arrival
                    if (plist[i].art < plist[idx].art) {
                        idx = i;
                    }
                }
            }
        }

        if (idx != -1) {
            plist[idx].wt = current_time - plist[idx].art;
            if (plist[idx].wt < 0) {
                plist[idx].wt = 0;
            }

            current_time += plist[idx].bt;
            is_completed[idx] = 1;
            completed++;
        } else {
            // No one ready yet; CPU idle
            current_time++;
        }
    }
}

// --------------------- Round Robin (with arrival times) ---------------------
void findWaitingTimeRR(ProcessType plist[], int n, int quantum)
{
    int rem_bt[n];
    int completed = 0;
    int t = 0;

    for (int i = 0; i < n; i++) {
        rem_bt[i] = plist[i].bt;
        plist[i].wt = 0;
    }

    while (completed < n) {
        int did_something = 0;
        int next_arrival = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (rem_bt[i] > 0) {
                // process not finished yet
                if (plist[i].art <= t) {
                    did_something = 1;

                    int slice = (rem_bt[i] > quantum) ? quantum : rem_bt[i];
                    t += slice;
                    rem_bt[i] -= slice;

                    if (rem_bt[i] == 0) {
                        // completion time t, so waiting = finish - burst - arrival
                        plist[i].wt = t - plist[i].bt - plist[i].art;
                        if (plist[i].wt < 0) {
                            plist[i].wt = 0;
                        }
                        completed++;
                    }
                } else {
                    // not yet arrived, remember earliest upcoming arrival
                    if (plist[i].art < next_arrival) {
                        next_arrival = plist[i].art;
                    }
                }
            }
        }

        if (!did_something) {
            // CPU idle until next process arrives
            if (next_arrival != INT_MAX && t < next_arrival) {
                t = next_arrival;
            }
        }
    }
}

// --------------------- Turnaround Time ---------------------
void findTurnAroundTime(ProcessType plist[], int n)
{
    for (int i = 0; i < n; i++) {
        plist[i].tat = plist[i].bt + plist[i].wt;
    }
}

// --------------------- Wrappers for each algorithm ---------------------
void runFCFS(ProcessType plist[], int n)
{
    findWaitingTimeFCFS(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nFCFS\n");
}

void runSJF(ProcessType plist[], int n)
{
    findWaitingTimeSJF(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nSJF (Non-preemptive)\n");
}

void runSRTF(ProcessType plist[], int n)
{
    findWaitingTimeSRTF(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nSRTF (Preemptive SJF)\n");
}

void runPriority(ProcessType plist[], int n)
{
    findWaitingTimePriorityAlgo(plist, n);
    findTurnAroundTime(plist, n);
    printf("\n*********\nPriority\n");
}

void runRR(ProcessType plist[], int n, int quantum)
{
    findWaitingTimeRR(plist, n, quantum);
    findTurnAroundTime(plist, n);
    printf("\n*********\nRR Quantum = %d\n", quantum);
}

// --------------------- Metrics Printing ---------------------
void printMetrics(ProcessType plist[], int n)
{
    int total_wt = 0, total_tat = 0;
    float awt, att;

    printf("\tProcess\tBurst\tArrival\tPriority\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        total_wt  += plist[i].wt;
        total_tat += plist[i].tat;
        printf("\t%d\t%d\t%d\t%d\t\t%d\t%d\n",
               plist[i].pid,
               plist[i].bt,
               plist[i].art,
               plist[i].pri,
               plist[i].wt,
               plist[i].tat);
    }

    awt = (float) total_wt / (float) n;
    att = (float) total_tat / (float) n;

    printf("\nAverage waiting time = %.2f", awt);
    printf("\nAverage turn around time = %.2f\n", att);
}

// --------------------- Init from file ---------------------
ProcessType *initProc(char *filename, int *n)
{
    FILE *input_file = fopen(filename, "r");
    if (!input_file) {
        fprintf(stderr, "Error: Invalid filepath\n");
        fflush(stdout);
        exit(1);
    }

    ProcessType *plist = parse_file(input_file, n);
    fclose(input_file);
    return plist;
}

// --------------------- main ---------------------
int main(int argc, char *argv[])
{
    int n;
    int quantum = 2;
    ProcessType *proc_list;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input-file-path>\n", argv[0]);
        fflush(stdout);
        return 1;
    }

    // FCFS
    n = 0;
    proc_list = initProc(argv[1], &n);
    runFCFS(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // SJF
    n = 0;
    proc_list = initProc(argv[1], &n);
    runSJF(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // SRTF
    n = 0;
    proc_list = initProc(argv[1], &n);
    runSRTF(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // Priority
    n = 0;
    proc_list = initProc(argv[1], &n);
    runPriority(proc_list, n);
    printMetrics(proc_list, n);
    free(proc_list);

    // RR
    n = 0;
    proc_list = initProc(argv[1], &n);
    runRR(proc_list, n, quantum);
    printMetrics(proc_list, n);
    free(proc_list);

    return 0;
}