#include "addjunctiondialog.h"
#include "ui_addjunctiondialog.h"

AddJunctionDialog::AddJunctionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddJunctionDialog)
{
    roadStr = QStringList();
    up_roads = QStringList();
    down_roads = QStringList();
    ui->setupUi(this);
    connect(ui->pushButton_up, SIGNAL(clicked()), this, SLOT(onUpClicked()));
    connect(ui->pushButton_down, SIGNAL(clicked()), this, SLOT(onDownClicked()));
}

void AddJunctionDialog::build(QList<road> roads)
{
    cleanup();
    for(road i : roads){
        roadStr.append(repr_road(i));
    }
    roadModel = new QStringListModel(roadStr, this);
    upRoadModel = new QStringListModel(up_roads, this);
    downRoadModel = new QStringListModel(down_roads, this);
    ui->comboBox_up_roads->setModel(roadModel);
    ui->comboBox_down_roads->setModel(roadModel);
    ui->listView_up->setModel(upRoadModel);
    ui->listView_down->setModel(downRoadModel);
}

junction AddJunctionDialog::getData()
{
    junction j(-1);
    j.name = ui->lineEdit_name->text();
    j.area = ui->lineEdit_area->text().toFloat();
    j.up_road_ids = Database::idsToString(up_roads);
    j.up_road_angles = Database::anglesToString(up_roads);
    j.down_road_ids = Database::idsToString(down_roads);
    j.down_road_angles = Database::anglesToString(down_roads);

    cleanup();
    return j;
}

void AddJunctionDialog::cleanup()
{
    roadStr.clear();
    up_roads.clear();
    down_roads.clear();
    ui->lineEdit_name->clear();
    ui->lineEdit_area->clear();
    ui->lineEdit_up_angle->clear();
    ui->lineEdit_down_angle->clear();
}

AddJunctionDialog::~AddJunctionDialog()
{
    if (ui != nullptr)
        delete ui;
    if (roadModel != nullptr)
        delete roadModel;
    if (upRoadModel != nullptr)
        delete upRoadModel;
    if (downRoadModel != nullptr)
        delete downRoadModel;
}

void AddJunctionDialog::onUpClicked()
{
    QString select = ui->comboBox_up_roads->currentText();
    roadStr.removeAll(select);
    select.append(tr("|%1 degrees").arg(ui->lineEdit_up_angle->text()));
    up_roads.append(select);
    roadModel->setStringList(roadStr);
    upRoadModel->setStringList(up_roads);
}

void AddJunctionDialog::onDownClicked()
{
    QString select = ui->comboBox_down_roads->currentText();
    roadStr.removeAll(select);
    select.append(tr("|%1 degrees").arg(ui->lineEdit_down_angle->text()));
    down_roads.append(select);
    roadModel->setStringList(roadStr);
    downRoadModel->setStringList(down_roads);
}
