#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QFile>
QT_USE_NAMESPACE
namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openSerialPort();
    void readData();
    void writeData();

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

    void on_sendButton_clicked();

    void on_actionClear_triggered();

    void on_baudRateBox_currentIndexChanged(int index);

    void on_actionSave_triggered();

    void on_actionQuit_triggered();

    void on_actionAbout_triggered();

private:
    void fillPortPara();
    void setEnableAll(bool status);

    QSerialPort *serial;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
