#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QtGlobal>
#include <QMainWindow>
#include <QSerialPort>
#include <QFile>
#include "obd.h"

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class Console;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    obd *Myobd;
    QString error;
//    Console *console;
//    SettingsDialog *settings;
//    QFile *file;

public slots:
    void SendPidsHS();
    void SendPidsLS();

private slots:
    void startConnect();  // conectar dispositivo al puerto serie
    void startOBDLog();   // empieza el log
    void stopConnect();  // desconecta el dispositivo del puerto serie
    void stopOBDLog();   // termina el log
    void about();
    void writeData(const QByteArray &data);
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void handleError();
    void sendOBD();
    void sendToggleLed();
    void avPidsReceive(QByteArray avPids);
    void handleThreadError();
    int GetNumOfPIDs(void);
    void SetTimers(int num);

private:
    void initActionsConnections();

private:
    Ui::MainWindow *ui;
    Console *console;
    SettingsDialog *settings;
    QTimer *HSTimer;
    QTimer *LSTimer;
};

#endif // MAINWINDOW_H
