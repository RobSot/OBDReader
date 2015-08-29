#ifndef PIDSELECTDIALOG_H
#define PIDSELECTDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include "defines.h"
#include "obd.h"

namespace Ui {
class PidSelectDialog;
}

class PidSelectDialog : public QDialog
{
    Q_OBJECT

public:
    void initPidDialog(QBitArray avPids);
    QBitArray avPids(void);
    bool O2SensorsConf(void);
    explicit PidSelectDialog(QWidget *parent = 0);
    ~PidSelectDialog();

private slots:
    void on_Button_select_clicked();

private:
    QString queryConfigFilePidName(int pid);
    QBitArray avPids_pidSelect;
    QListWidgetItem *listItem;
    Ui::PidSelectDialog *ui;
};

#endif // PIDSELECTDIALOG_H
