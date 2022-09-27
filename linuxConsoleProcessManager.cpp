#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <sys/resource.h>
#include <sys/types.h>
#include <chrono>

using namespace std;

int main(int argc, char *argv[]) {
    int numOfProcesses;
    int numeration = 1;
    int rows, columns;
    
    cout << "\nHello, enter amount of process: ";
    cin >> numOfProcesses;
    
    cout << "Enter num of rows: ";
    cin >> rows;
    cout << "Enter num of cols: ";
    cin >> columns;

    pid_t pids[numOfProcesses];
    
    for(int i = 0; i < numOfProcesses; ++i) {
        pids[i] = fork();
        
        if(pids[i] < 0) {
            perror("Fork failed!");
            return -1;
        } else if(pids[i] == 0) {
            int rowCorrect = (i == numOfProcesses - 1) ? (rows - (rows / numOfProcesses) * (numOfProcesses - 1)) : rows / numOfProcesses;
            execl("/home/rom4ik/OS/lab4/findEqualsMatrix/mainExe", 
                  "/home/rom4ik/OS/lab4/findEqualsMatrix/mainExe", to_string(rowCorrect).c_str(), to_string(columns).c_str(), (char*)NULL);
        } else {
            kill(pids[i], SIGSTOP);
            cout << "\n" << numeration++ << ". Child pid = " << pids[i] << " Created, Suspended";
        }
    }
    pid_t wpid;
    int status = 0;
    
    while(true) {
        int choice;
        cout << "\n\nRun all and see time - 1 \nChange priority - 2 \nKill process - 3\nQuit - 4\n";
        cin >> choice;
        switch(choice){
            case 1: {
                const auto begin = chrono::high_resolution_clock::now();
                for(int i = 0; i < numOfProcesses; ++i) {
                    kill(pids[i], SIGCONT);
                }

                while ((wpid = wait(&status)) > 0);
                auto time = chrono::high_resolution_clock::now() - begin;
                cout << "\nDuration: " << chrono::duration<double, milli>(time).count() << "ms\n";
                return 0;
            }
            case 2: {
                int num;
                int priority = 0;
                cout << "\nEnter process number: ";
                cin >> num;
                num--;
                if(num < 0 || num >= numOfProcesses) {
                    cout << "process with such number doesn't exist!\n";
                    break;
                }
                cout << "\nEnter new prority: ";
                cin >> priority;
                setpriority(PRIO_PROCESS, pids[num], priority);
                
                std::cout<<"\nNew priority is " <<
                         getpriority(PRIO_PROCESS, pids[num]);
                break;
            }
            case 3: {
                int num;
                cout << "\nEnter number of process to kill: ";
                cin >> num;
                num--;
                if(num < 0 || num >= numOfProcesses) {
                    cout << "process with such number doesn't exist!\n";
                    break;
                }
                kill(pids[num], SIGKILL);
                pids[num] = -1;
                cout << "Killed)";
                break;
            }
            case 4: {
            	for(int i = 0; i < numOfProcesses; ++i) {
                    if(pids[i] > 0)
            	       kill(pids[i], SIGKILL);
            	}
            	cout << "the end\n";
            	return 0;
            }
        }
    }
    cout << "the end\n";
    wait(NULL);
    return 0;
}


