#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSqlDatabase mydb = QSqlDatabase::addDatabase("QSQLITE", "SQLITE");
    mydb.setDatabaseName("/home/skazik/workspace/Qt/WidgetSqlite/mysql.db");

    if (mydb.open()) {
        ui->label->setText("mysql.db opened");
    }
    else {
        ui->label->setText("failed to open mysql.db");
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QString uname = ui->username->text();
    ui->label->setText(uname);
}
