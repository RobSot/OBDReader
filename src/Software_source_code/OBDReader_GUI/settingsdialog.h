#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QSerialPort>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class SettingsDialog;
}

class QIntValidator;

QT_END_NAMESPACE

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    struct Settings {
        QString name;
        bool fastSample;
        bool slowSample;
//        qint32 baudRate;
//        QString stringBaudRate;
//        QSerialPort::DataBits dataBits;
//        QString stringDataBits;
//        QSerialPort::Parity parity;
//        QString stringParity;
//        QSerialPort::StopBits stopBits;
//        QString stringStopBits;
//        QSerialPort::FlowControl flowControl;
//        QString stringFlowControl;
//        bool localEchoEnabled;
    };

    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    Settings settings() const;

private slots:
    void showPortInfo(int idx);
    void apply();

private:
    void fillPortsInfo();
    void updateSettings();

private:
    Ui::SettingsDialog *ui;
    Settings currentSettings;
};

#endif // SETTINGSDIALOG_H
