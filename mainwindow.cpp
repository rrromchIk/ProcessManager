#include "mainwindow.h"
#include "ui_mainwindow.h"

Ui::MainWindow *MainWindow::ui = new Ui::MainWindow;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    ui->setupUi(this);
    processManager = nullptr;
    ui->processesTable->setColumnCount(6);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_resumeButton_clicked() {
    if(processManager) {
        processManager->resumeProcess();
    }
}

void MainWindow::on_suspendButton_clicked() {
    if(processManager) {
        processManager->suspendProcess();
    }
}


void MainWindow::on_terminateButton_clicked() {
    if(processManager) {
        processManager->terminateProcess();
    }
}


void MainWindow::on_setPriorityButton_clicked() {
    int priority;
    QString comboBoxState = ui->proirityComboBox->currentText();
    if(!comboBoxState.compare("IDLE_PRIORITY_CLASS")) { priority = IDLE_PRIORITY_CLASS; }
    else if (!comboBoxState.compare("BELOW_NORMAL_PRIORITY_CLASS")) priority = BELOW_NORMAL_PRIORITY_CLASS;
    else if (!comboBoxState.compare("NORMAL_PRIORITY_CLASS")) priority = NORMAL_PRIORITY_CLASS;
    else if (!comboBoxState.compare("ABOVE_NORMAL_PRIORITY_CLASS")) priority = ABOVE_NORMAL_PRIORITY_CLASS;
    else if (!comboBoxState.compare("HIGH_PRIORITY_CLASS")) priority = HIGH_PRIORITY_CLASS;
    else if (!comboBoxState.compare("REALTIME_PRIORITY_CLASS")) priority = REALTIME_PRIORITY_CLASS;
    else priority = 0;
    if(processManager) {
        processManager->setPriority(priority);
    }
}


void MainWindow::on_startButton_clicked() {
    if (processManager) {
        delete processManager;
    }
    ui->proirityComboBox->setCurrentText("BELOW_NORMAL_PRIORITY_CLASS");
    ui->processesTable->setRowCount(1);
    int arguments[2] = {0, 0};
    QString str = ui->processesArgumentsLineEdit->text();
    if(!str.isEmpty()) {
        QStringList list = str.trimmed().split(" ");
        for(int i = 0; i < list.size(); i++) {
            arguments[i] = (list.at(i).toInt());
        }
    }

    processManager = new ProcessManager(ui->numOfProcessesLineEdit->text().trimmed().toInt(), arguments[0], arguments[1]);
}

