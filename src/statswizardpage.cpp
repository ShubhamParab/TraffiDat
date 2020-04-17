#include "statswizardpage.h"
#include "ui_statswizardpage.h"

StatsWizardPage::StatsWizardPage(MapModel* model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::StatsWizardPage),
    model(model),
    parent(parent)
{
    ui->setupUi(this);
    setTitle("Statistics");
    setSubTitle("Recent collected data: ");
    build();

}

void StatsWizardPage::build()
{
    // building road stats
    for(road i : model->getAllRds().values()){
        MutRoadWidget* eachWdgt = new MutRoadWidget(this);
        qDebug() << "TRAFFIC LIGHT OF ROAD: " << i.road_id << " is " << model->getTraffLByRoadId(i.road_id);
        eachWdgt->setData(i, model->getDatabase()->getAllMutRoadsById(i.road_id, model->getDumpCount()), *model->getMutTraffLById(model->getTraffLByRoadId(i.road_id)));

        QListWidgetItem *each = new QListWidgetItem();
        each->setSizeHint(eachWdgt->sizeHint());

        ui->listWidget_list_road->addItem(each);
        ui->listWidget_list_road->setItemWidget(each, eachWdgt);
    }
    // building traffL stats
    for(mut_traffic_light* i : model->getMutTraffL().values()){
        qDebug() << "TL ID: " << i->traffic_light_id << " USE COUNT: " << i->use_count;
    }
}

StatsWizardPage::~StatsWizardPage()
{
    delete ui;
}

void StatsWizardPage::onShowWidget(QWidget *widget)
{
    widget->show();
    QWidget* parentWdgt = static_cast<QWidget*>(parent);
    parentWdgt->stackUnder(widget);
}
