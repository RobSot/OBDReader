#include <QMessageBox>
#include <QSerialPort>
#include <QTextStream>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "console.h"
#include "settingsdialog.h"
#include "pidselectdialog.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    console = new Console;
    console->setReadOnly(true);
    console->setEnabled(false);
    setCentralWidget(console);
    console->putData(OBDREADER_VERSION); console->putData("\n");

    HSTimer = new QTimer(this);
    LSTimer = new QTimer(this);
    settings = new SettingsDialog(this);
    pidselection = new PidSelectDialog(this);
    Myobd = new obd(this);

    ui->actionConnect->setEnabled(true);
    ui->actionStartLog->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionStopLog->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
    ui->actionSelectPid->setEnabled(false);
    Myobd->stop = false;
    Myobd->firstEntryHS = true;
    Myobd->firstEntryLS = true;
    Myobd->Pid_O2_voltage = true;

    initActionsConnections();

    connect(Myobd->serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleSerialError(QSerialPort::SerialPortError)));
    connect(Myobd,SIGNAL(SendPidsHS()),this,SLOT(SendPidsHS()));
    connect(Myobd,SIGNAL(threadError()),this,SLOT(handleThreadError()));
    connect(HSTimer, SIGNAL(timeout()), this, SLOT(SendPidsHS()));
    connect(LSTimer, SIGNAL(timeout()), this, SLOT(SendPidsLS()));
}


MainWindow::~MainWindow()
{
    delete settings;
    delete ui;
}


void MainWindow::startOBDLog()
{
    console->putData("Iniciando Log... ");
    if(Myobd->initFile())
    {
       //solicitud cambio de toggle time status led
       Myobd->sendCommand(CMD_STATUS_LED,11,1);
       if (Myobd->queryAndResponse(CMD_START,1,1,2) == "ON")
        {
            console->putData("Ok!\n");
            Myobd->PidVector.fill(0.00);
            Myobd->firstEntryHS = true;
            Myobd->firstEntryLS = true;

            ui->actionStartLog->setEnabled(false);
            ui->actionStopLog->setEnabled(true);
            ui->actionDisconnect->setEnabled(false);
            ui->actionSelectPid->setEnabled(false);

            Myobd->stop = false;

            Myobd->avPids = pidselection->avPids();

            Myobd->Pid_O2_voltage = pidselection->O2SensorsConf();

            fillLogHeaderPIDs();

            qDebug() << Myobd->avPids;

            Myobd->numPIDs = GetNumOfPIDs();

            qDebug() << "numero: " << Myobd->numPIDs;

            if (Myobd->numPIDs == 0)
            {
                stopOBDLog();
                Myobd->file->remove();
                QMessageBox::warning(this,"Error","No se ha podido empezar el Log. Por favor, seleccione algún PID");

            }
            else
            {
                SetTimers(Myobd->numPIDs);

                console->putData("Loggeando...");
            }
        }
       else
       {
           handleError(OBDREADER_NOT_RESPONDING1);
       }
    }
    else
    {
        console->putData("Fail!\n");
    }
}

void MainWindow::startConnect()
{
    console->putData("Estableciendo comunicación... ");
    SettingsDialog::Settings p = settings->settings();
    if (Myobd->initSerialCom(p.name,error))
    {
        console->setEnabled(true);
        ui->actionConnect->setEnabled(false);
        ui->actionStartLog->setEnabled(true);
        ui->actionDisconnect->setEnabled(true);
        ui->actionConfigure->setEnabled(false);
        ui->actionSelectPid->setEnabled(true);
        ui->statusBar->showMessage(tr("Conectado a %1 : %2").arg(p.name).arg(OBDREADER_VERSION));

        if (ChekForAvPids() == EXIT_SUCCESS)
        {
            console->putData("Ok!\n");
        }
    }
    else
    {
        QMessageBox::critical(this, tr("Error de Conexión"), error); //ver como enviar el error
        ui->statusBar->showMessage(tr("Open error"));
        console->putData("Fail!\n");
    }
}


void MainWindow::stopConnect()
{
    Myobd->serial->close();

    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    ui->actionConfigure->setEnabled(true);
    ui->actionStartLog->setEnabled(false);
    ui->actionSelectPid->setEnabled(false);
    ui->statusBar->showMessage(tr("Desconectado"));
    console->putData("Desconectandose... Ok!\n");
}

void MainWindow::stopOBDLog()
{
    if (Myobd->queryAndResponse(CMD_STOP,1,1,2) == "KO")
    {
        HSTimer->stop();
        while (HSTimer->isActive());
//        LSTimer->stop();
//        while (LSTimer->isActive());

        //solicitud cambio de toggle time status led
        Myobd->sendCommand(CMD_STATUS_LED,107,1);

        Myobd->file->close();
        ui->actionStartLog->setEnabled(true);
        ui->actionStopLog->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
        ui->actionSelectPid->setEnabled(true);
//        ui->actionSendOBD->setEnabled(false);
        console->putData("\nTerminando Log... Ok!\n");
//        Myobd->stop = true;
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr(OBDREADER_VERSION),
                       tr("Interfaz gráfica para el loggeo de datos del <b>OBD Reader</b>. "
                          "Diseñado y desarrollado por <b>Roberto Rodríguez Soto</b> como proyecto "
                          "fin de carrera de la <b>Universidad de Vigo</b>."));
}

void MainWindow::writeData(const QByteArray &data)
{
    Myobd->serial->write(data); // escribe en el puerto serie
}

void MainWindow::readData()
{
    QByteArray data = Myobd->serial->readAll();  // leo lo que hay en el uart rx, salta cuando llega algo, está definido arriba
    console->putData(data.toHex()); // con esto escribo en la consola
    if ((data != "ON") & (data != "KO"))
    {
        Myobd->file->write(data);
    }
}


void MainWindow::handleError(int errorCode)
{
    if (errorCode == OBDREADER_NOT_RESPONDING2)
    {
        console->putData("Fail!\n");
        QMessageBox::critical(this, tr("Error"), "El dispositivo no responde. Revise y/o reinicie el dispositivo");
    }
    else if (errorCode == PID_ERROR)
    {
        Myobd->queryAndResponse(CMD_STOP,1,1,2);
        console->putData("Fail!\n");
        QMessageBox::critical(this, tr("Error"), "El dispositivo parece estar desconectado del puerto OBD. Revise la conexión");
    }
    else if (errorCode == OBDREADER_NOT_RESPONDING1)
    {
        console->putData("Fail!\n");
        QMessageBox::critical(this, tr("Error"), "No se ha podido empezar el log. Revise y/o reinicie el dispositivo");
        HSTimer->stop();
        while (HSTimer->isActive());
        //    LSTimer->stop();
        //    while (LSTimer->isActive());

        //solicitud cambio de toggle time status led
        Myobd->sendCommand(CMD_STATUS_LED,107,1);

        Myobd->file->close();
        Myobd->file->remove();

        ui->actionStartLog->setEnabled(true);
        ui->actionStopLog->setEnabled(false);
        ui->actionDisconnect->setEnabled(true);
    }
    stopConnect();
}

void MainWindow::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Error"), Myobd->serial->errorString());
        stopOBDLog();
        stopConnect();
    }
}

void MainWindow::sendOBD()
{

}

void MainWindow::sendToggleLed()
{
    Myobd->parseOBD(Myobd->queryAndResponse(CMD_OBD,MODE_01,O2_S1_LAMBDA_RATIO_AND_CURRENT,7));
}

void MainWindow::SendPidsHS()
{
    qDebug() << "SendPidsHS()";
    uchar pid = 0x01;
    for (int i = 0; i < Myobd->avPids.size(); i++)
    {
        if (Myobd->avPids.at(i))
        {
            if (ISNT_AV_PID_COMMAND(pid))
            {
                Myobd->parseOBD_Vector(Myobd->queryAndResponse(CMD_OBD,MODE_01,pid,7));
                qDebug() << QString::number(pid,16);
            }
        }
        pid += 0x01;
    }
    console->putData(".");
}

void MainWindow::SendPidsLS()
{
    qDebug() << "SendPidsLS()";
//    char pid = 0x01;
//    for (int i = 0; i < Myobd->avPids.size(); i++)
//    {
//        if (Myobd->avPids.at(i))
//        {
//            Myobd->parseOBD_Vector(Myobd->queryAndResponse(CMD_OBD,MODE_01,O2_S1_LAMBDA_RATIO_AND_CURRENT,7));
//            qDebug() << QString::number(pid,16);
//        }
//        pid += 0x01;
//    }

            if (Myobd->firstEntryLS)
            {
                Myobd->firstEntryLS = false;
            }
}

int MainWindow::GetNumOfPIDs(void)
{
    int numPids = 0, pid = 0x01;

    for (int i = 0; i < Myobd->avPids.size(); i++)
    {
        if (Myobd->avPids.at(i))
        {
            if (ISNT_AV_PID_COMMAND(pid))
            {
                numPids++;
            }
        }
        pid += 0x01;
    }
    return numPids;
}

void MainWindow::SetTimers(int num)
{
    int t = num * 10; // 10 es el tiempo de respuesta estimado de cada PID
    SettingsDialog::Settings p = settings->settings();

    if (p.fastSample)
    {
        HSTimer->setInterval(t);
    }
    else if (p.slowSample)
    {
        HSTimer->setInterval(t * 3);
    }
    HSTimer->start();

}

int MainWindow::ChekForAvPids(void)
{
    if (Myobd->queryAndResponse(CMD_START,1,1,2) == "ON")
     {
         Myobd->avPids = Myobd->avaliablePids();
         if (GetNumOfPIDs() != NUM_OF_PIDS - 7) // soluciona error cuando no esta conectado a CAN y si al usb.
         {
             pidselection->initPidDialog(Myobd->avPids);
             pidselection->exec();
             Myobd->queryAndResponse(CMD_STOP,1,1,2);
         }
         else
         {
            handleError(PID_ERROR);
            return EXIT_FAILURE;
         }
     }
    else
    {
        handleError(OBDREADER_NOT_RESPONDING2);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

QString MainWindow::queryConfigFilePidName(int pid)
{
    int idx;
    bool found = false;
    QString name, SearchedPid, pid_str;
    QFile PIDsFile("PIDsConf");
    pid_str = QString::number(pid,16);

    PIDsFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream text(&PIDsFile);

    /* Se da formato al PID para coincidir con el del archivo */
    SearchedPid.prepend("#");
    if (pid_str.size() < 2) {SearchedPid.append("0");}       // Añade un 0 a los simples
    SearchedPid.append(pid_str);
    SearchedPid.replace(1, 1, SearchedPid[1].toUpper()); // Cambia a uppercase
    SearchedPid.replace(2, 1, SearchedPid[2].toUpper()); // Cambia a uppercase

    while(!text.atEnd())
    {
        name = text.readLine();
        if ((idx = name.indexOf(SearchedPid,0)) != -1)
        {
            name = name.remove(idx,6);
            found = true;
            break;
        }
    }

    if (found == false)
    {
        name = SearchedPid.remove(0,1);
    }

    PIDsFile.close();

    return name;
}

int MainWindow::fillLogHeaderPIDs()
{
    QString name;
    int pid;
    QTextStream out(Myobd->file);

    pid = 0x01;
    out << "Timestamp;";

    for (int i = 0; i < NUM_OF_PIDS; i++)
    {
        if (ISNT_AV_PID_COMMAND(pid))
        {
            name = queryConfigFilePidName(pid);
            out << name << ";";
        }
        pid += 0x01;
    }

    out << "\n";

    return 0;
}

void MainWindow::handleThreadError()
{
    console->putData("Fail!\n");
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionStartLog, SIGNAL(triggered()), this, SLOT(startOBDLog()));
    connect(ui->actionConnect, SIGNAL(triggered()), this, SLOT(startConnect()));
    connect(ui->actionStopLog, SIGNAL(triggered()), this, SLOT(stopOBDLog()));
    connect(ui->actionDisconnect, SIGNAL(triggered()), this, SLOT(stopConnect()));
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(exec()));
    connect(ui->actionSelectPid, SIGNAL(triggered()), pidselection, SLOT(exec()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
}
