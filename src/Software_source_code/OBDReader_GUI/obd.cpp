#include "obd.h"
#include "settingsdialog.h"
#include "console.h"
#include "mainwindow.h"

obd::obd(QObject *parent) : QThread(parent)
{
    PidVector.resize(NUM_OF_PIDS);
    PidVector.fill(0.00);

    serial = new QSerialPort(this);
    timeouTtimer = new QTimer(this);
    timeouTtimer->setSingleShot(true);
    error = false;
}

bool obd::initSerialCom(QString name, QString &error)
{
    serial->setPortName(name);
    serial->setBaudRate(460800);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if (serial->open(QIODevice::ReadWrite))
    {
        return true;
    }
    else
    {
        error = serial->errorString();
        return false;
    }
}

bool obd::initFile()
{
    /* Se mira si existe el folder y si no, se crea */
    QString dirName = "logs";
    QDir dir(dirName);
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    /* Se genera el nombre del archivo del log */
    QTime time = QTime::currentTime();
    QDate date = QDate::currentDate();
    QString fileName;

    fileName = "./"; // ruta madre
    fileName = fileName.append(dirName.append("/")).append("ObdLog_"); // Nombre padre del archivo (sin fechas ni tiempo)
    fileName = fileName.append(date.toString("dd_MM_yy_"));  // Fecha
    fileName = fileName.append(time.toString("hh_mm_ss"));  // Hora
    fileName = fileName.append(".csv");  // Extension del archivo

    /* Se genera el archivo */
    file = new QFile(fileName); // crear archivo con fecha en el nombre cada vez
    if (file->open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void obd::sendCommand(uchar cmd, uchar mode, uchar pid)
{
    qDebug() << "sendCommand()";

    QByteArray datos;
    datos.resize(3);

    datos[0] = cmd;
    datos[1] = mode;
    datos[2] = pid;
    serial->write(datos);
}

QByteArray obd::queryAndResponse(uchar cmd, uchar mode, uchar pid, int buffSize)
{
    qDebug() << "queryAndResponse()";

    QByteArray buff;
    buff.resize(buffSize);
    buff.fill(0xFF);
    serial->waitForBytesWritten(20); // Espera a que se envien los bytes pendientes de otras llamadas, al dispositivo
    serial->clear(); // Borra los buffers de entrada y salida de la UART del pc. Ojo que no interfiera con nada que esté haciendo
    sendCommand(cmd,mode,pid);

    timeouTtimer->stop();
    timeouTtimer->start(50);

    while ((serial->bytesAvailable() < buffSize) && (timeouTtimer->remainingTime() != 0))
    {
        qDebug() << "waitforReadyRead() ";
        qDebug() << timeouTtimer->remainingTime();
        serial->waitForReadyRead(20);
    }
    if (timeouTtimer->remainingTime() != 0)
    {
        timeouTtimer->stop();
        buff = serial->read(buffSize);
    }
    return buff;
}

QBitArray obd::avaliablePids()
{
    qDebug() << "avaliablePids()";

    QByteArray rvbuff, buff;
    int pid, pos=0;
    buff.resize(NUM_OF_PIDS/8); // Para almacenar hasta el PID E0
    buff.fill(0);    // Llenar de 0 el buffer

    for (pid = 0; pid < 0xE0; pid += 0x20)
    {
        rvbuff = queryAndResponse(2,1,pid,7);
        buff = buff.replace(pos,4,rvbuff.right(4));
        pos += 4;

        if ((rvbuff.at(6) & 0x01) == 0) // mira si es 0 el ultimo bit del pid
        {
            break;  // Este brake es por si quiero que tan pronto no haya un pid disponible, no siga con los demas
        }
    }
    return ByteArrayToBitArray(buff);
//    return buff; // retorna un qbytearray
}

QBitArray obd::ByteArrayToBitArray(QByteArray data)
{
    QBitArray bitData(data.size()*8);
    for (int i=0;i<data.size();i++)
    {
        for (int j=0;j<8;j++)
        {
            bitData.setBit(i * 8 + j,data.at(i) & (1 << (7 - j)));
        }
    }
    return bitData;
}

void obd::parseOBD(QByteArray obd)
{
    qDebug() << QString::number(obd.size()) << " : " << obd.toHex();

    uchar pid, A, B;
//    uchar num, mode, pid, A, B, C, D;
    QTime timeStamp = QTime::currentTime();
    QTextStream text(file);
    bool notPid = true;
//    num = obd.at(0) - 2;
//    mode = obd.at(1);
    pid = obd.at(2);
    A = obd.at(3);
    B = obd.at(4);
//    C = obd.at(5);
//    D = obd.at(6);
    text << timeStamp.toString("hh:mm:ss.zzz:") << QString::number(pid,16) << ": ";

    if (pid == COOLANT_TEMP)
    {
        int temp = A - 40;
        text << QString::number(temp) << "\n";
        qDebug() << "temp = " << QString::number(temp);
        notPid = false;
    }
    else if (pid == ENGINE_RPM)
    {
        int rpm = ((A * 256) + B) / 4;
        qDebug() << "rpm = " << QString::number(rpm);
        text << QString::number(rpm) << "\n";
        notPid =false;
    }
    else if (pid == VEHICLE_SPEED)
    {
        int speed = A;
        qDebug() << "speed = " << QString::number(speed);
        text << QString::number(speed) << "\n";
        notPid =false;
    }
    else if (pid == AMBIENT_AIR_TEMPERATURE)
    {
        int temp = A - 40;
        text << QString::number(temp) << "\n";
        qDebug() << "temp = " << QString::number(temp);
        notPid = false;
    }
    else if (notPid)
    {
        text << "Not supported PID\n";
        qDebug() <<"Not supported PID";
    }
}

void obd::parseOBD_Vector(QByteArray obd)
{
    qDebug() << QString::number(obd.size()) << " : " << obd.toHex();

    static int count = 0; //Solo se inicializa la primera vez a 0 (static)
    uchar pid, A, B, C, D;
//    uchar num, mode, pid, A, B, C, D;
    QTime timeStamp = QTime::currentTime();
    QTextStream text(file);
    bool notPid = true;
//    num = obd.at(0) - 2;
//    mode = obd.at(1);
    pid = obd.at(2);
    A = obd.at(3);
    B = obd.at(4);
    C = obd.at(5);
    D = obd.at(6);
    count++;

    if      (pid == FUEL_SYSTEM_STATUS)
    {
        if (IS_FUEL_SYSTEM_MODES(A))
        {
            PidVector[FUEL_SYSTEM_STATUS] = A;
            notPid = false;
        }
    }
    else if (pid == ENGINE_LOAD)
    {
        PidVector[ENGINE_LOAD] = (double) (A * 100) / 255;
        notPid = false;
    }
    else if (pid == COOLANT_TEMP)
    {
        PidVector[COOLANT_TEMP] = A - 40;
        notPid = false;
    }
    else if (pid == SHORT_TERM_FUEL_BANK1)
    {
        PidVector[SHORT_TERM_FUEL_BANK1] = (double) ((A - 128) * 100) / 128;
        notPid = false;
    }
    else if (pid == LONG_TERM_FUEL_BANK1)
    {
        PidVector[LONG_TERM_FUEL_BANK1] = (double) ((A - 128) * 100) / 128;
        notPid = false;
    }
    else if (pid == SHORT_TERM_FUEL_BANK2)
    {
        PidVector[SHORT_TERM_FUEL_BANK2] = (double) ((A - 128) * 100) / 128;
        notPid = false;
    }
    else if (pid == LONG_TERM_FUEL_BANK2)
    {
        PidVector[LONG_TERM_FUEL_BANK2] = (double) ((A - 128) * 100) / 128;
        notPid = false;
    }
    else if (pid == FUEL_PRESSURE)
    {
        PidVector[FUEL_PRESSURE] = A * 3;
        notPid = false;
    }
    else if (pid == INTAKE_MANIFOLD_ABS_PRESSURE)
    {
        PidVector[INTAKE_MANIFOLD_ABS_PRESSURE] = A;
        notPid = false;
    }
    else if (pid == ENGINE_RPM)
    {
        PidVector[ENGINE_RPM] = (double) ((A * 256) + B) / 4;
        notPid = false;
    }
    else if (pid == VEHICLE_SPEED)
    {
        PidVector[VEHICLE_SPEED] = A;
        notPid = false;
    }
    else if (pid == TIMING_ADVANCE)
    {
        PidVector[TIMING_ADVANCE] = (double) (A - 128) / 2;
        notPid = false;
    }
    else if (pid == INTAKE_AIR_TEMP)
    {
        PidVector[INTAKE_AIR_TEMP] = A - 40;
        notPid = false;
    }
    else if (pid == MAF)
    {
        PidVector[MAF] = (double) ((A * 256) + B) / 100;
        notPid = false;
    }
    else if (pid == THROTTLE_POSITION)
    {
        PidVector[THROTTLE_POSITION] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == COMMANDED_SECONDARY_AIR_STATUS)
    {
        if (IS_OXIGEN_SYSTEM_MODES(A))
        {
            PidVector[COMMANDED_SECONDARY_AIR_STATUS] = A;
            notPid = false;
        }
    }
    else if (pid == OXYGEN_BANK1_SENSOR1)
    {
        PidVector[OXYGEN_BANK1_SENSOR1] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK1_SENSOR2)
    {
        PidVector[OXYGEN_BANK1_SENSOR2] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK1_SENSOR3)
    {
        PidVector[OXYGEN_BANK1_SENSOR3] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK1_SENSOR4)
    {
        PidVector[OXYGEN_BANK1_SENSOR4] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK2_SENSOR1)
    {
        PidVector[OXYGEN_BANK2_SENSOR1] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK2_SENSOR2)
    {
        PidVector[OXYGEN_BANK2_SENSOR2] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK2_SENSOR3)
    {
        PidVector[OXYGEN_BANK2_SENSOR3] = (double) A / 200;
        notPid = false;
    }
    else if (pid == OXYGEN_BANK2_SENSOR4)
    {
        PidVector[OXYGEN_BANK2_SENSOR4] = (double) A / 200;
        notPid = false;
    }
    else if (pid == RUN_TIME_SINCE_ENGINE_START)
    {
        PidVector[RUN_TIME_SINCE_ENGINE_START] = (A * 256) + B;
        notPid = false;
    }
    else if (pid == DISTANCE_TRAVELED_WITH_MIL_ON)
    {
        PidVector[DISTANCE_TRAVELED_WITH_MIL_ON] = (A * 256) + B;
        notPid = false;
    }
    else if (pid == FUEL_RAIL_PRESSURE)
    {
        PidVector[FUEL_RAIL_PRESSURE] = (double) ((A * 256) + B) * 0.079;
        notPid = false;
    }
    else if (pid == FUEL_RAIL_PRESSURE_DIRECT_INJECT)
    {
        PidVector[FUEL_RAIL_PRESSURE_DIRECT_INJECT] = ((A * 256) + B) * 10;
        notPid = false;
    }
    else if (pid == O2_S1_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S1_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S1_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S2_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S2_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S2_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S3_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S3_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S3_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S4_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S4_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S4_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S5_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S5_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S5_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S6_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S6_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S6_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S7_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S7_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S7_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S8_LAMBDA_RATIO_AND_VOLTAGE)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S8_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((C * 256) + D) * 8) / 65535;
        }
        else
        {
            PidVector[O2_S8_LAMBDA_RATIO_AND_VOLTAGE] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == COMMANDED_EGR)
    {
        PidVector[COMMANDED_EGR] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == EGR_ERROR)
    {
        PidVector[EGR_ERROR] = ((A - 128) * 100) / 128;
        notPid = false;
    }
    else if (pid == COMMANDED_EVAPORATIVE_PURGE)
    {
        PidVector[COMMANDED_EVAPORATIVE_PURGE] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == FUEL_LEVEL_INPUT)
    {
        PidVector[FUEL_LEVEL_INPUT] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == WARM_UPS_SINCE_DTC_CLEARED)
    {
        PidVector[WARM_UPS_SINCE_DTC_CLEARED] = A;
        notPid = false;
    }
    else if (pid == DISTANCE_SINCE_DTC_CLEARED)
    {
        PidVector[DISTANCE_SINCE_DTC_CLEARED] = (A * 256) + B;
        notPid = false;
    }
    else if (pid == EVAP_SYSTEM_VAPOR_PRESSURE)
    {
        PidVector[EVAP_SYSTEM_VAPOR_PRESSURE] = ((A * 256) + B) / 4;
        notPid = false;
    }
    else if (pid == BAROMETRIC_PRESSURE)
    {
        PidVector[BAROMETRIC_PRESSURE] = A;
        notPid = false;
    }
    else if (pid == O2_S1_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S1_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S1_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S2_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S2_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S2_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S3_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S3_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S3_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S4_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S4_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S4_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S5_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S5_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S5_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S6_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S6_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S6_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S7_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S7_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S7_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == O2_S8_LAMBDA_RATIO_AND_CURRENT)
    {
        if (Pid_O2_voltage)
        {
            PidVector[O2_S8_LAMBDA_RATIO_AND_CURRENT] = (double) ((C * 256) + D) / 256 - 128;
        }
        else
        {
            PidVector[O2_S8_LAMBDA_RATIO_AND_CURRENT] = (double) (((A * 256) + B) * 2) / 65535;
        }
        notPid = false;
    }
    else if (pid == CATALYST_TEMP_BANK1_SENSOR1)
    {
        PidVector[CATALYST_TEMP_BANK1_SENSOR1] = (((A * 256) + B) / 10) - 40;
        notPid = false;
    }
    else if (pid == CATALYST_TEMP_BANK2_SENSOR1)
    {
        PidVector[CATALYST_TEMP_BANK2_SENSOR1] = (((A * 256) + B) / 10) - 40;
        notPid = false;
    }
    else if (pid == CATALYST_TEMP_BANK1_SENSOR2)
    {
        PidVector[CATALYST_TEMP_BANK1_SENSOR2] = (((A * 256) + B) / 10) - 40;
        notPid = false;
    }
    else if (pid == CATALYST_TEMP_BANK2_SENSOR2)
    {
        PidVector[CATALYST_TEMP_BANK2_SENSOR2] = (((A * 256) + B) / 10) - 40;
        notPid = false;
    }
    else if (pid == CONTROL_MODULE_VOLTAGE)
    {
        PidVector[CONTROL_MODULE_VOLTAGE] = (double) ((A * 256) + B) / 1000;
        notPid = false;
    }
    else if (pid == ABSOLUTE_LOAD_VALUE)
    {
        PidVector[ABSOLUTE_LOAD_VALUE] = (double) (((A * 256) + B) * 100) / 255;
        notPid = false;
    }
    else if (pid == FUEL_AIR_COMMANDED_RATIO)
    {
        PidVector[FUEL_AIR_COMMANDED_RATIO] = (double) ((A * 256) + B) / 32768;
        notPid = false;
    }
    else if (pid == RELATIVE_THROTTLE_POSSITION)
    {
        PidVector[RELATIVE_THROTTLE_POSSITION] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == AMBIENT_AIR_TEMPERATURE)
    {
        PidVector[AMBIENT_AIR_TEMPERATURE] = A - 40;
        notPid = false;
    }
    else if (pid == ABSOLUTE_THROTTLE_POSITION_B)
    {
        PidVector[ABSOLUTE_THROTTLE_POSITION_B] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ABSOLUTE_THROTTLE_POSITION_C)
    {
        PidVector[ABSOLUTE_THROTTLE_POSITION_C] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ACCELERATOR_PEDAL_POSITION_D)
    {
        PidVector[ACCELERATOR_PEDAL_POSITION_D] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ACCELERATOR_PEDAL_POSITION_E)
    {
        PidVector[ACCELERATOR_PEDAL_POSITION_E] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ACCELERATOR_PEDAL_POSITION_F)
    {
        PidVector[ACCELERATOR_PEDAL_POSITION_F] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == COMMANDED_THROTTLE_ACTUATOR)
    {
        PidVector[COMMANDED_THROTTLE_ACTUATOR] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == TIME_RUN_WITH_MIL_ON)
    {
        PidVector[TIME_RUN_WITH_MIL_ON] = (A * 256) + B;
        notPid = false;
    }
    else if (pid == TIME_SINCE_DTC_CLEARED)
    {
        PidVector[TIME_SINCE_DTC_CLEARED] = (A * 256) + B;
        notPid = false;
    }
    else if (pid == MAXIMUM_VALUE_FOR_MAF)
    {
        PidVector[MAXIMUM_VALUE_FOR_MAF] = A * 10;
        notPid = false;
    }
    else if (pid == ETHANOL_FUEL_PERCENT)
    {
        PidVector[ETHANOL_FUEL_PERCENT] = (double) (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ABS_EVAP_SYSTEM_VAPOR_PRESSURE)
    {
        PidVector[ABS_EVAP_SYSTEM_VAPOR_PRESSURE] = ((A * 256) + B) / 200;
        notPid = false;
    }
    else if (pid == EVAP_SYSTEM_VAPOR_PRESSURE_2)
    {
        PidVector[EVAP_SYSTEM_VAPOR_PRESSURE_2] = ((A * 256) + B) - 32767;
        notPid = false;
    }
    else if (pid == ABSOLUTE_FUEL_RAIL_PRESSURE)
    {
        PidVector[ABSOLUTE_FUEL_RAIL_PRESSURE] = ((A * 256) + B) * 10;
        notPid = false;
    }
    else if (pid == RELATIVE_ACCELERATOR_PEDAL_POS)
    {
        PidVector[RELATIVE_ACCELERATOR_PEDAL_POS] = (A * 100) / 255;
        notPid = false;
    }
    else if (pid == HYBRID_BATTERY_PACK_REMAINING)
    {
        PidVector[HYBRID_BATTERY_PACK_REMAINING] = (double) (A * 100) / 255;
        notPid = false;
    }
    else if (pid == ENGINE_OIL_TEMP)
    {
        PidVector[ENGINE_OIL_TEMP] = A - 40;
        notPid = false;
    }
    else if (pid == FUEL_INJECTION_TIMMING)
    {
        PidVector[FUEL_INJECTION_TIMMING] = (double) (((A * 256) + B) - 26.880) / 128;
        notPid = false;
    }
    else if (pid == ENGINE_FUEL_RATE)
    {
        PidVector[ENGINE_FUEL_RATE] = (double) ((A * 256) + B) * 0.05;
        notPid = false;
    }
    else if (pid == DRIVERS_DEMAND_PERCENT_TORQUE)
    {
        PidVector[DRIVERS_DEMAND_PERCENT_TORQUE] = A - 125;
        notPid = false;
    }
    else if (pid == ACTUAL_ENGINE_PERCENT_TORQUE)
    {
        PidVector[ACTUAL_ENGINE_PERCENT_TORQUE] = A - 125;
        notPid = false;
    }
    else if (pid == ENGINE_REFERENCE_TORQUE)
    {
        PidVector[ENGINE_REFERENCE_TORQUE] = (A * 256) + B;
        notPid = false;
    }
    else if (notPid)
    {
//        text << "Not supported PID\n";
        qDebug() <<"Not supported PID";
    }

    if (count == numPIDs)
    {
        // TODO meter esto en una funcion y que solo entre cuando se haya completado un ciclo de PIDs

        text << timeStamp.toString("hh:mm:ss.zzz;");
        for (int i = 1; i < PidVector.size(); i++)
        {
            if (ISNT_AV_PID_COMMAND(i) && ISNT_USELESS_PID(i))
            {
                text << QString::number(PidVector.at(i)) << ";";
            }
        }
        text << "\n";
        count = 0;
    }
}

void obd::readData()
{
    /* Esta es la funcion a usar para recibir las tramas UART
    Siempre recibe tramas de 7 bytes */

    if ((serial->bytesAvailable()) >= 7)
    {
        QByteArray data;
        data = serial->read(7);
        parseOBD(data);

        qDebug() << data.toHex();
    }
}

void obd::enableAsincronousReceive()
{
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}
void obd::disableAsincronousReceive()
{
    disconnect(serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void obd::run()
{
    QMutex mutex;

//    mutex.lock();
//    QByteArray avPids = avaliablePids();

    qDebug() << this->avPids;

//    emit avPidsSignal(avPids);
//    mutex.unlock();

    enableAsincronousReceive();
    qDebug() << "detntro, con ID: " << QThread::currentThreadId();
    while (1)
    {
        qDebug() << "Thread loop";

        emit SendPidsHS(); // Send Pids request

        mutex.lock();
        if (this->stop)
        {
            disableAsincronousReceive();
            mutex.unlock();
            break;
        }
        mutex.unlock();
        obd::msleep(1000);

    }
}
