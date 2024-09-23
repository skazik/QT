#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow * getMainWinPtr();

    void on_serial_input(QString line);

private slots:
    void send_message(const char *txt);

    void on_pushButton_up_clicked();

    void on_pushButton_down_clicked();

    void on_pushButton_left_clicked();

    void on_pushButton_ok_clicked();

    void on_pushButton_right_clicked();

    void on_pushButton_rear_clicked();

private:
    Ui::MainWindow *ui;
    static MainWindow * pMainWindow;
};
#endif // MAINWINDOW_H
