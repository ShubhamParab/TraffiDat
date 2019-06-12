#include "dumptesttrafficlightdialog.h"
#include "ui_dumptesttrafficlightdialog.h"

DumpTestTrafficLightDialog::DumpTestTrafficLightDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DumpTestTrafficLightDialog)
{
    ui->setupUi(this);
}

void DumpTestTrafficLightDialog::build(QList<traffic_light> tls)
{
    cleanup();
    for(traffic_light i : tls){
        tlsStr.append(repr_traffiL(i));
    }
    ui->comboBox_traffic_light->setModel(new QStringListModel(tlsStr));
}

mut_traffic_light DumpTestTrafficLightDialog::getData()
{
    mut_traffic_light out(-1);
    out.traffic_light_id = ui->comboBox_traffic_light->currentText().split(',')[0].toInt();
    out.green_timer = ui->lineEdit_green_timer->text().toInt();
    out.use_count = ui->lineEdit_use_count->text().toInt();
    out.timestamp = ui->dateTimeEdit_timestamp->dateTime().toString(Qt::ISODate);

    return out;
}

void DumpTestTrafficLightDialog::cleanup()
{
    tlsStr.clear();
    ui->lineEdit_green_timer->clear();
    ui->lineEdit_use_count->clear();
    ui->dateTimeEdit_timestamp->setDateTime(QDateTime::currentDateTime());
}

DumpTestTrafficLightDialog::~DumpTestTrafficLightDialog()
{
    delete ui;
}
