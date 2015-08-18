#ifndef obd_H
#define obd_H

#include <QtCore>
#include <QThread>
#include <QSerialPort>
#include <QTimer>
#include "defines.h"

class Console;

class obd : public QThread
{
    Q_OBJECT
public:
    explicit obd(QObject *parent = 0);
    void run();
    bool initSerialCom(QString name, QString &error);
    bool initFile();
    void sendCommand(uchar cmd, uchar mode, uchar pid);
    void parseOBD(QByteArray obd);
    void parseOBD_Vector(QByteArray obd);
    QBitArray ByteArrayToBitArray(QByteArray data);
    QByteArray queryAndResponse(uchar cmd, uchar mode, uchar pid, int buffSize);
    QBitArray avaliablePids();
    QBitArray avPids;
    bool stop, error, firstEntryHS, firstEntryLS;
    QSerialPort *serial;
    Console *console;
    QFile *file;
    QVector<double> PidVector;
    int numPIDs;


private:
    QTimer *timeouTtimer;
    bool isTimeout;

signals:
    void SendPidsHS();
    void avPidsSignal(QByteArray avPids);
    void threadError();

public slots:
    void enableAsincronousReceive();
    void disableAsincronousReceive();

private slots:
    void readData();
};

#endif // obd_H
