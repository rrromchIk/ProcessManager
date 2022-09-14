#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <QDebug>

class ProcessManager {
private:
    static std::string processPath;
    int numOfProcesses;
    int rowSize, colSize;
    STARTUPINFOA *si;
    PROCESS_INFORMATION *pi;
    HANDLE *handles;
    HANDLE thread;
    DWORD threadID;
public:
    ProcessManager(int numOfProcesses, int rowSize, int colSize);
    void suspendProcess();
    void resumeProcess();
    void terminateProcess();
    void setPriority(int priority);
    ~ProcessManager();

    friend DWORD WINAPI checkStatus(LPVOID lpParameter);
};
