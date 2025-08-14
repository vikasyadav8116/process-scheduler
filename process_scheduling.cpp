#include <bits/stdc++.h>
using namespace std;

struct Process {
    int pid;
    int priority;
    int arrival_time;
    int burst_time;
    int completion_time;
    int turn_around_time;
    int waiting_time;
};

struct Summary {
    string algo_name;
    double avg_waiting_time;
    double avg_turnaround_time;
};

bool compareArrival(Process a, Process b) {
    return a.arrival_time < b.arrival_time;
}

void display(Process proc[], int n) {
    cout << "Process ID\tPriority\tArrival Time\tBurst Time\tCompletion Time\tTurn Around Time\tWaiting Time\n";
    for (int i = 0; i < n; i++) {
        cout << proc[i].pid << "\t\t" << proc[i].priority << "\t\t" << proc[i].arrival_time << "\t\t" 
             << proc[i].burst_time << "\t\t" << proc[i].completion_time << "\t\t" << proc[i].turn_around_time 
             << "\t\t\t" << proc[i].waiting_time << endl;
    }
}

double averageWaitingTime(Process proc[], int n) {
    double totalWaitingTime = 0.00;
    for (int i = 0; i < n; i++) {
        totalWaitingTime += proc[i].waiting_time;
    }
    return totalWaitingTime / n;
}

double averageTurnaroundTime(Process proc[], int n) {
    double totalTAT = 0.00;
    for (int i = 0; i < n; i++) {
        totalTAT += proc[i].turn_around_time;
    }
    return totalTAT / n;
}

Summary PriorityQueue(Process proc[],int n){
    sort(proc,proc+n,compareArrival);
    vector<int>remaining_time(n);
    for (int i = 0; i < n; i++) {
        remaining_time[i] = proc[i].burst_time;
    }
    int currentTime=0;
    int complete=0;
    while(complete<n){
        bool found=false;
        int index=-1;
        int maxPriority=INT_MIN;
        for(int i=0;i<n;i++){
            if(proc[i].arrival_time<=currentTime && remaining_time[i]>0 && proc[i].priority>maxPriority){
                maxPriority=proc[i].priority;
                index=i;
                found=true;
            }
        }
        if(!found){
            currentTime++;
            continue;
        }
        remaining_time[index]--;
        currentTime++;
        if(remaining_time[index]==0){
            complete++;
            proc[index].completion_time=currentTime;
            proc[index].turn_around_time=proc[index].completion_time-proc[index].arrival_time;
            proc[index].waiting_time=proc[index].turn_around_time-proc[index].burst_time;
        }
    }
    display(proc,n);
    double awt = averageWaitingTime(proc, n);
    double atat = averageTurnaroundTime(proc, n);
    cout << "Average Waiting Time: " << awt << endl;
    return {"Priority Queue", awt, atat};
}

Summary FCFS(Process proc[], int n) {
    sort(proc, proc + n, compareArrival);
    proc[0].completion_time = proc[0].arrival_time + proc[0].burst_time;
    for (int i = 1; i < n; i++) {
        if (proc[i].arrival_time > proc[i - 1].completion_time)
            proc[i].completion_time = proc[i].arrival_time + proc[i].burst_time;
        else
            proc[i].completion_time = proc[i - 1].completion_time + proc[i].burst_time;
    }

    for (int i = 0; i < n; i++) {
        proc[i].turn_around_time = proc[i].completion_time - proc[i].arrival_time;
        proc[i].waiting_time = proc[i].turn_around_time - proc[i].burst_time;
    }
    display(proc,n);
    double awt = averageWaitingTime(proc, n);
    double atat = averageTurnaroundTime(proc, n);
    cout << "Average Waiting Time: " << awt << endl;
    return {"FCFS", awt, atat};
}

Summary SJF(Process proc[], int n) {
    sort(proc, proc + n, compareArrival);
    vector<bool> done(n, false);
    int currentTime = 0, completed = 0;

    while (completed < n) {
        int index = -1;
        int minBurst = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!done[i] && proc[i].arrival_time <= currentTime && proc[i].burst_time < minBurst) {
                minBurst = proc[i].burst_time;
                index = i;
            }
        }

        if (index == -1) {
            currentTime++;
        } else {
            currentTime += proc[index].burst_time;
            proc[index].completion_time = currentTime;
            proc[index].turn_around_time = proc[index].completion_time - proc[index].arrival_time;
            proc[index].waiting_time = proc[index].turn_around_time - proc[index].burst_time;
            done[index] = true;
            completed++;
        }
    }
    display(proc,n);
    double awt = averageWaitingTime(proc, n);
    double atat = averageTurnaroundTime(proc, n);
    cout << "Average Waiting Time: " << awt << endl;
    return {"SJF", awt, atat};
}

Summary RoundRobin(Process proc[], int n, int quantum) {
    queue<int> q;
    sort(proc, proc + n, compareArrival);

    vector<int> remainingTime(n);
    for (int i = 0; i < n; i++) {
        remainingTime[i] = proc[i].burst_time;
    }

    vector<bool> inQueue(n, false);
    int currentTime = 0;
    int complete = 0;
    q.push(0);
    inQueue[0] = true;

    while (!q.empty()) {
        int idx = q.front();
        q.pop();

        if (remainingTime[idx] > quantum) {
            currentTime += quantum;
            remainingTime[idx] -= quantum;
        } else {
            currentTime += remainingTime[idx];
            remainingTime[idx] = 0;
            complete++;
            proc[idx].completion_time = currentTime;
            proc[idx].turn_around_time = proc[idx].completion_time - proc[idx].arrival_time;
            proc[idx].waiting_time = proc[idx].turn_around_time - proc[idx].burst_time;
        }

        for (int i = 0; i < n; i++) {
            if (!inQueue[i] && proc[i].arrival_time <= currentTime && remainingTime[i] > 0) {
                q.push(i);
                inQueue[i] = true;
            }
        }

        if (remainingTime[idx] > 0) {
            q.push(idx);
        }

        if (q.empty()) {
            for (int i = 0; i < n; i++) {
                if (remainingTime[i] > 0) {
                    q.push(i);
                    inQueue[i] = true;
                    currentTime = max(currentTime, proc[i].arrival_time);
                    break;
                }
            }
        }
    }

    display(proc,n);
    double awt = averageWaitingTime(proc, n);
    double atat = averageTurnaroundTime(proc, n);
    cout << "Average Waiting Time: " << awt << endl;
    return {"Round Robin", awt, atat};
}

Summary SRTF(Process proc[],int n){
    vector<int>remaining_time(n);
    for(int i=0;i<n;i++){
        remaining_time[i]=proc[i].burst_time;
    }
    int currentTime=0;
    int complete=0;
    while(complete<n){
        bool found=false;
        int index=-1;
        int minBurst=INT_MAX;
        for(int i=0;i<n;i++){
            if(proc[i].arrival_time<=currentTime && remaining_time[i]>0 && remaining_time[i]<minBurst){
                minBurst=remaining_time[i];
                index=i;
                found=true;
            }
        }
        if(!found){
            currentTime++;
            continue;
        }
        remaining_time[index]--;
        currentTime++;
        if(remaining_time[index]==0){
            complete++;
            proc[index].completion_time=currentTime;
            proc[index].turn_around_time=proc[index].completion_time-proc[index].arrival_time;
            proc[index].waiting_time=proc[index].turn_around_time-proc[index].burst_time;
        }

    }
    display(proc,n);
    double awt = averageWaitingTime(proc, n);
    double atat = averageTurnaroundTime(proc, n);
    cout << "Average Waiting Time: " << awt << endl;
    return {"SRTF", awt, atat};
}

int main() {
    int n;
    cout << "Number of processes: ";
    cin >> n;
    Process proc[n];

    cout << "Enter the priority of all processes:\n";
    for (int i = 0; i < n; i++) {
        proc[i].pid = i + 1;
        cin >> proc[i].priority;
    }

    cout << "Enter the arrival time of all processes:\n";
    for (int i = 0; i < n; i++) {
        cin >> proc[i].arrival_time;
    }

    cout << "Enter the burst time of all processes:\n";
    for (int i = 0; i < n; i++) {
        cin >> proc[i].burst_time;
    }

    vector<Summary> summaries;

    while (true) {
        Process temp[n];
        memcpy(temp, proc, sizeof(proc));

        cout << "\nChoose a scheduling algorithm:\n";
        cout << "1. FCFS\n2. SJF\n3. SRTF\n4. Round Robin\n5. Priority Queue\n6. Exit\n";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1: {
                Summary s = FCFS(temp, n);
                summaries.push_back(s);
                break;
            }
            case 2: {
                Summary s = SJF(temp, n);
                summaries.push_back(s);
                break;
            }
            case 3: {
                Summary s = SRTF(temp, n);
                summaries.push_back(s);
                break;
            }
            case 4: {
                int q;
                cout << "Enter the quantum time for Round Robin: ";
                cin >> q;
                Summary s = RoundRobin(temp, n, q);
                summaries.push_back(s);
                break;
            }
            case 5: {
                Summary s = PriorityQueue(temp, n);
                summaries.push_back(s);
                break;
            }
            case 6:
                cout << "Exiting...\n";
                if (!summaries.empty()) {
                    cout << "\nComparative Summary Table:\n";
                    cout << left << setw(20) << "Algorithm"
                    << setw(20) << "Avg Waiting Time"
                     << setw(20) << "Avg Turnaround Time" << endl;

                for (auto &s : summaries) {
                    cout << left << setw(20) << s.algo_name
                    << setw(20) << fixed << setprecision(2) << s.avg_waiting_time
                    << setw(20) << fixed << setprecision(2) << s.avg_turnaround_time << endl;
                    }

                }
                return 0;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
