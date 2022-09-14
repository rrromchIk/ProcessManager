#include "processmanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

std::string ProcessManager::processPath = "findEqualsInMatrix.exe ";

std::string getPriority(HANDLE handle) {
    int priority = GetPriorityClass(handle);
    switch(priority) {
        case IDLE_PRIORITY_CLASS: return "Idle";
        case BELOW_NORMAL_PRIORITY_CLASS: return "Below normal";
        case NORMAL_PRIORITY_CLASS: return "Normal";
        case ABOVE_NORMAL_PRIORITY_CLASS: return "Above normal";
        case HIGH_PRIORITY_CLASS: return "High";
        case REALTIME_PRIORITY_CLASS: return "Real time";
        default: return "Unknown";
    }
}

std::string getStatus(HANDLE handle) {
    DWORD status = WaitForSingleObject(handle, 0);
        switch(status) {
            case WAIT_OBJECT_0: return "Terminated";
            default:
                int value = SuspendThread(handle);
                ResumeThread(handle);
                if(value == 0){
                    return "Running";
                }
                return "Suspended";
        }
}

DWORD WINAPI checkStatus(LPVOID lpParameter) {
    ProcessManager* manager = (ProcessManager*) lpParameter;

    int numberOfProcesses = manager->numOfProcesses;
    bool *statuses = new bool[numberOfProcesses];
    for(int i = 0; i < numberOfProcesses; i++) {
        statuses[i] = true;
    }

    FILETIME creationTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    std::string status;

    for(int i = 0; i < MainWindow::ui->processesTable->rowCount(); ++i) {
        for(int j = 0; j < MainWindow::ui->processesTable->columnCount(); ++j) {
            QTableWidgetItem *item= new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignCenter);
            MainWindow::ui->processesTable->setItem(i, j, item);
        }
    }

    while(1) {
        for(int i = 0; i < numberOfProcesses; i++) {
            if(!statuses[i]) continue;
            status = getStatus(manager->pi[i].hThread);
            GetProcessTimes(manager->pi[i].hProcess, &creationTime, &exitTime, &kernelTime, &userTime);

            unsigned long long cr = (((unsigned long long) creationTime.dwHighDateTime) << 32) + creationTime.dwLowDateTime;
            unsigned long long ex = (((unsigned long long) exitTime.dwHighDateTime) << 32) + exitTime.dwLowDateTime;
            unsigned long long ke = (((unsigned long long) kernelTime.dwHighDateTime) << 32) + kernelTime.dwLowDateTime;
            unsigned long long us = (((unsigned long long) userTime.dwHighDateTime) << 32) + userTime.dwLowDateTime;

            MainWindow::ui->processesTable->item(i, 0)->setText(QString::number((long long)manager->pi[i].dwProcessId));
            MainWindow::ui->processesTable->item(i, 1)->setText(QString::fromStdString(status));
            MainWindow::ui->processesTable->item(i, 2)->setText(QString::fromStdString(getPriority(manager->pi[i].hProcess)));
            MainWindow::ui->processesTable->item(i, 3)->setText("Not Terminated");
            MainWindow::ui->processesTable->item(i, 4)->setText(QString::number(ke / 1.0e7, 'g', 4) + "s");
            MainWindow::ui->processesTable->item(i, 5)->setText(QString::number(us / 1.0e7, 'g', 4) + "s");

            if(status == "Terminated") {
                MainWindow::ui->processesTable->item(i, 3)->setText(QString::number(((ex - cr) / 1.0e7), 'g', 4) + "s");
                statuses[i] = false;
            }
        }
        Sleep(100);
    } 
}


ProcessManager::ProcessManager(int numOfProcesses, int rowSize, int colSize) {
    this->numOfProcesses = numOfProcesses;
    this->rowSize = rowSize;
    this->colSize = colSize;
    this->pi = new PROCESS_INFORMATION[numOfProcesses];
    this->si = new STARTUPINFOA[numOfProcesses];
    this->handles = new HANDLE[numOfProcesses];

    std::string* arguments = new std::string[numOfProcesses];
    for (int i = 0; i < numOfProcesses; i++) {
        arguments[i] += std::to_string(rowSize / numOfProcesses) + " " + std::to_string(colSize / numOfProcesses);
    }
    std::string finalComandLine;

    for(int i = 0; i < numOfProcesses; ++i) {
        ZeroMemory(&si[i], sizeof(STARTUPINFOA));
        si[i].cb = sizeof(STARTUPINFOA);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
        si[i].lpTitle = (LPSTR)"MyProcess";
        finalComandLine = (processPath + arguments[i]);
        if (CreateProcessA(NULL, (LPSTR)finalComandLine.c_str(), NULL, NULL, NULL,
                           CREATE_NEW_CONSOLE | BELOW_NORMAL_PRIORITY_CLASS, NULL, NULL, &si[i], &pi[i])) {
            handles[i] = pi[i].hProcess;
        } else
          throw std::runtime_error("process failed to create");
    }
    thread = CreateThread(NULL, NULL, checkStatus, this, NULL, &threadID);
    SetThreadPriority(thread, REALTIME_PRIORITY_CLASS);
    MainWindow::ui->processesTable->setRowCount(this->numOfProcesses);
    delete[] arguments;
}

void ProcessManager::resumeProcess() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();;
    ResumeThread(pi[items[0]->row()].hThread);
}

void ProcessManager::suspendProcess() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();;
    SuspendThread(pi[items[0]->row()].hThread);
}

void ProcessManager::terminateProcess() {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    TerminateProcess(handles[items[0]->row()], EXIT_SUCCESS);
}

void ProcessManager::setPriority(int priority) {
    QList<QTableWidgetItem*> items = MainWindow::ui->processesTable->selectedItems();
    SetPriorityClass(handles[items[0]->row()], priority);
}

ProcessManager::~ProcessManager() {
    for (int i = 0; i < this->numOfProcesses; ++i) {
        TerminateProcess(handles[i], EXIT_SUCCESS);
        CloseHandle(handles[i]);
    }
    delete[] this->pi;
    delete[] this->si;
    delete[] this->handles;
    MainWindow::ui->processesTable->setRowCount(1);
}

