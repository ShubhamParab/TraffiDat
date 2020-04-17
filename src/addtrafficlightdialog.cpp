#include "addtrafficlightdialog.h"
#include "ui_addtrafficlightdialog.h"

AddTrafficLightDialog::AddTrafficLightDialog(Database *db, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTrafficLightDialog),
    db(db)
{
    juncts = QStringList();
    rds = QStringList();
    selectRds = QStringList();
    ui->setupUi(this);
    connect(ui->pushButton_add, SIGNAL(clicked()), this, SLOT(onAdd()));
    connect(ui->comboBox_junction, SIGNAL(currentTextChanged(QString)), this, SLOT(onCurrentTextChanged(QString)));
}

void AddTrafficLightDialog::build(QList<junction> js, QList<road> rs)
{
    cleanup();
    for(junction i: js)
        juncts.append(repr_junction(i));
    for(road i : rs)
        rds.append(repr_road(i));
    junctsModel = new QStringListModel(juncts, this);
    rdsModel = new QStringListModel(rds, this);
    selectRdsModel = new QStringListModel(selectRds, this);
    ui->comboBox_junction->setModel(junctsModel);
    ui->comboBox_road->setModel(rdsModel);
    ui->listView->setModel(selectRdsModel);
}

traffic_light AddTrafficLightDialog::getData()
{
    traffic_light out;
    out.junction_id = ui->comboBox_junction->currentText().split(',')[0].toInt();
    out.road_ids = Database::idsToString(selectRds);

    return out;
}

void AddTrafficLightDialog::cleanup()
{
    juncts.clear();
    rds.clear();
    selectRds.clear();
}

AddTrafficLightDialog::~AddTrafficLightDialog()
{
    if(ui != nullptr)
        delete ui;
//    if(junctsModel != nullptr)
//        delete junctsModel;
//    if(rdsModel != nullptr)
//        delete rdsModel;
//    if(selectRdsModel != nullptr)
//        delete selectRdsModel;
}

void AddTrafficLightDialog::onAdd()
{
    QString select = ui->comboBox_road->currentText();
    rds.removeAll(select);
    selectRds.append(select);

    rdsModel->setStringList(rds);
    selectRdsModel->setStringList(selectRds);
}

void AddTrafficLightDialog::onCurrentTextChanged(QString value)
{
    qint64 junction_id = value.split(',')[0].toInt();
    junction j = db->junctionById(junction_id);
    rds.clear();
    for(road i : db->roadsByJunction(j)){
        rds.append(repr_road(i));
    }
    rdsModel = new QStringListModel(rds, this);
    ui->comboBox_road->setModel(rdsModel);
}
