#include "pidselectdialog.h"
#include "ui_pidselectdialog.h"

PidSelectDialog::PidSelectDialog(QWidget *parent) : QDialog(parent), ui(new Ui::PidSelectDialog)
{
    ui->setupUi(this);
}

PidSelectDialog::~PidSelectDialog()
{
    delete ui;
}

void PidSelectDialog::initPidDialog(QBitArray avPids)
{
    int pid = 1;
    avPids_pidSelect = avPids;
    ui->listWidget->clear();
    ui->Button_select->setText("Deseleccionar\ntodos");
    ui->radioButton_ratio->setChecked(true);

    for (int i = 0;i < NUM_OF_PIDS; i++)
    {
        listItem = new QListWidgetItem(queryConfigFilePidName(pid),ui->listWidget);

        if (avPids_pidSelect.at(i) && ISNT_AV_PID_COMMAND(pid) && ISNT_USELESS_PID(pid))
        {
            listItem->setCheckState(Qt::Checked);
        }
        else
        {
            listItem->setCheckState(Qt::Unchecked);
            listItem->setHidden(true);
        }

        ui->listWidget->addItem(listItem);
        pid++;
    }
}

QBitArray PidSelectDialog::avPids(void)
{
    for (int i = 0;i < NUM_OF_PIDS; i++)
    {
        if (ui->listWidget->item(i)->checkState() == Qt::Checked)
        {
            avPids_pidSelect.setBit(i);
            qDebug() << "Item " << QString::number(i+1,16) << " cheked yes";
        }
        else if (ui->listWidget->item(i)->checkState() == Qt::Unchecked)
        {
            avPids_pidSelect.clearBit(i);
            qDebug() << "Item " << QString::number(i+1,16) << " cheked No";
        }
    }
    return avPids_pidSelect;
}

bool PidSelectDialog::O2SensorsConf(void)
{
    return ui->radioButton_voltage->isChecked();
}

QString PidSelectDialog::queryConfigFilePidName(int pid)
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

void PidSelectDialog::on_Button_select_clicked()
{
    if (ui->Button_select->text() == "Deseleccionar\ntodos")
    {
        for (int i = 0;i < NUM_OF_PIDS; i++)
        {
            if (!ui->listWidget->item(i)->isHidden())
            {
                ui->listWidget->item(i)->setCheckState(Qt::Unchecked);
            }
        }
        ui->Button_select->setText("Seleccionar\ntodos");
    }
    else
    {
        for (int i = 0;i < NUM_OF_PIDS; i++)
        {
            if (!ui->listWidget->item(i)->isHidden())
            {
                ui->listWidget->item(i)->setCheckState(Qt::Checked);
            }
        }
        ui->Button_select->setText("Deseleccionar\ntodos");
    }
}
