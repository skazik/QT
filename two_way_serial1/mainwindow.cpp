#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "communication.h"

MainWindow * MainWindow::pMainWindow = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pMainWindow = this;
    send_message("Hey,");
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow * MainWindow:: getMainWinPtr() {return pMainWindow;}

void MainWindow::send_message(const char *txt)
{
    static SerialCommunication* Serial = get_instance();
    if (!Serial)
    {
        statusBar()->showMessage("SerialCommunication failed", 5000);
        return;
    }
    QString tmp = "Send to ESP32: ";
    tmp += txt;
    statusBar()->showMessage(tmp, 3000);
    Serial->sendData(txt);
    Serial->readData();
}
void MainWindow::on_serial_input(QString line)
{
    ui->textEdit->append(line);
}

void MainWindow::on_pushButton_up_clicked()
{
    send_message("upA,");
}

void MainWindow::on_pushButton_down_clicked()
{
    send_message("dnB,");
}

void MainWindow::on_pushButton_right_clicked()
{
    send_message("rtC,");
}

void MainWindow::on_pushButton_left_clicked()
{
    send_message("lfD,");
}

void MainWindow::on_pushButton_ok_clicked()
{
    send_message("ok!,");
}

void MainWindow::on_pushButton_rear_clicked()
{
    send_message("reR,");
}
