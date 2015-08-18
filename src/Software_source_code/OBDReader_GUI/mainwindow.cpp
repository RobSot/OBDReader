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
    settings = new SettingsDialog(this); //antes no tenia el this
    Myobd = new obd(this);

    ui->actionConnect->setEnabled(true);
    ui->actionStartLog->setEnabled(false);
    ui->actionDisconnect->setEnabled(false);
    ui->actionStopLog->setEnabled(false);
    ui->actionQuit->setEnabled(true);
    ui->actionConfigure->setEnabled(true);
//    ui->actionSendOBD->setEnabled(false);
    Myobd->stop = false;
    Myobd->firstEntryHS = true;
    Myobd->firstEntryLS = true;

    initActionsConnections();

    connect(Myobd->serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));
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
    int numPIDs = 0;

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
//            ui->actionSendOBD->setEnabled(true);

            //iniciar hilo de envio de pids
            Myobd->stop = false;
            Myobd->avPids = Myobd->avaliablePids();
            Myobd->file->write(EXCELL_PID_HEADER);

            qDebug() << Myobd->avPids;

            //TODO calcular el tiempo del timer en función de los settings y los avPIDs
            numPIDs = GetNumOfPIDs();
            Myobd->numPIDs = numPIDs;

            qDebug() << "numero: " << numPIDs;

            SetTimers(numPIDs);

            console->putData("Loggeando...");
        }
       else
       {
           handleError();
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
        ui->statusBar->showMessage(tr("Conectado a %1 : %2").arg(p.name).arg(OBDREADER_VERSION));

        console->putData("Ok!\n");
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
                          "fin de carrera de la <b>Universidad de Vigo</b>. v1.0.0"));
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


void MainWindow::handleError()
{
    QMessageBox::critical(this, tr("Error"), "No se ha podido empezar el log. Revise y/o reinicie el dispositivo.");

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
//    ui->actionSendOBD->setEnabled(false);
    console->putData("\nTerminando Log... Ok!\n");

    stopConnect();
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
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
            if ((pid != 0x20) && (pid != 0x40) && (pid != 0x60) && (pid != 0x80) && (pid != 0xA0) && (pid != 0xC0) && (pid != 0xE0))
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
    uchar pid = 0x01;
    int numPids = 0;

    for (int i = 0; i < Myobd->avPids.size(); i++)
    {
        if (Myobd->avPids.at(i))
        {
            if ((pid != 0x20) && (pid != 0x40) && (pid != 0x60) && (pid != 0x80) && (pid != 0xA0) && (pid != 0xC0) && (pid != 0xE0))
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

void MainWindow::avPidsReceive(QByteArray avPids)
{
    console->putData(avPids.toHex());
    console->putData("\n");
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
    connect(ui->actionConfigure, SIGNAL(triggered()), settings, SLOT(show()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
//    connect(ui->actionSendOBD, SIGNAL(triggered()), this, SLOT(sendOBD()));
//    connect(ui->actionLed, SIGNAL(triggered()), this, SLOT(sendToggleLed()));
}
