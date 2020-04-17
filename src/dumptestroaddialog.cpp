#include "dumptestroaddialog.h"
#include "ui_dumptestroaddialog.h"

DumpTestRoadDialog::DumpTestRoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DumpTestRoadDialog)
{
    ui->setupUi(this);
}

void DumpTestRoadDialog::build(QList<road> rds)
{
    cleanup();
    for(road i : rds){
        this->rds.append(repr_road(i));
    }
    ui->comboBox_road->setModel(new QStringListModel(this->rds));
}

mut_road DumpTestRoadDialog::getData()
{
    mut_road out(-1);
    out.road_id = ui->comboBox_road->currentText().split(',')[0].toInt();
    out.vehicle_count = ui->lineEdit_vehicle_count->text().toInt();
    out.vehicle_speed = ui->lineEdit_vehicle_speed->text().toFloat();
    out.congestion_count = ui->lineEdit_cong_count->text().toInt();
    out.timestamp = ui->dateTimeEdit->dateTime().toString(Qt::ISODate);
    return out;
}

void DumpTestRoadDialog::cleanup()
{
    rds.clear();
    ui->lineEdit_vehicle_count->clear();
    ui->lineEdit_vehicle_speed->clear();
    ui->lineEdit_cong_count->clear();
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

DumpTestRoadDialog::~DumpTestRoadDialog()
{
    delete ui;
}
