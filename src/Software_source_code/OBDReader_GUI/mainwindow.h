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
class PidSelectDialog;

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
    void handleSerialError(QSerialPort::SerialPortError error);
    void handleError(int errorCode);
    void sendOBD();
    void sendToggleLed();
    QString queryConfigFilePidName(int pid);
    int fillLogHeaderPIDs();
    int ChekForAvPids(void);
    void handleThreadError();
    int GetNumOfPIDs(void);
    void SetTimers(int num);

private:
    void initActionsConnections();

private:
    Ui::MainWindow *ui;
    Console *console;
    SettingsDialog *settings;
    PidSelectDialog *pidselection;
    QTimer *HSTimer;
    QTimer *LSTimer;
};

#endif // MAINWINDOW_H
