#include "traffltimerwizardpage.h"
#include "ui_traffltimerwizardpage.h"

TraffLTimerWizardPage::TraffLTimerWizardPage(MapModel* model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::TraffLTimerWizardPage),
    model(model)
{
    ui->setupUi(this);

    setCommitPage(true);
}

void TraffLTimerWizardPage::initializePage()
{
    oldTls = model->getTraffLsByRd2TraffL(model->getTls());
    newTls = model->getTraffLsByRd2TraffL(model->getAnalyTls());

    buildMeanMutTls();
    buildTlTimerRecommendations();
    build();
}

void TraffLTimerWizardPage::buildMeanMutTls()
{
    qDebug() << "OLD TRAFFIC LIGHTS: ";
    for(traffic_light* i : oldTls.values()){
        qDebug() << "TL ID: " << i->traffic_light_id;
        qDebug() << "JUNCTION ID: " << i->junction_id;
        qDebug() << "ROAD IDS: " << i->road_ids;
        qDebug() << "---------------------------------------";
    }
    qDebug() << "========================================";
    qDebug() << "NEW TRAFFIC LIGHTS: ";
    for(traffic_light* i : newTls.values()){
        qDebug() << "TL ID: " << i->traffic_light_id;
        qDebug() << "JUNCTION ID: " << i->junction_id;
        qDebug() << "ROAD IDS: " << i->road_ids;
        qDebug() << "---------------------------------------";
    }
    qDebug() << "========================================";

    // building tl category
    for(qint64 i : newTls.keys()){
        if(oldTls.value(i)->road_ids == "" && newTls.value(i)->road_ids != ""){
            catTls.insert(i, TlCategory::NEW_TL);
        }else if(oldTls.value(i)->road_ids != "" && newTls.value(i)->road_ids == ""){
            catTls.insert(i, TlCategory::UNAVAILABLE_TL);
        }else if(oldTls.value(i)->road_ids != "" && newTls.value(i)->road_ids != ""){
            catTls.insert(i, TlCategory::UPDATED_TL);
        }else if(oldTls.value(i)->road_ids == "" && newTls.value(i)->road_ids == ""){
            catTls.insert(i, TlCategory::UNUSED_TL);
        }
    }

    // calculating use count and cycle time
    for(traffic_light* i : newTls.values()){
        mut_traffic_light eachMut = mut_traffic_light(i->traffic_light_id);
        qint64 each_use_count = 0;
        qint64 each_cycle = 0;
        for(mut_traffic_light i : model->getDatabase()->getAllMutTraffLsById(i->traffic_light_id, model->getDumpCount())){
            each_use_count += i.use_count;
            each_cycle += i.green_timer;
            qDebug() << "ID: " << i.traffic_light_id << " USE_COUNT: " << each_use_count << " EACH_CYCLE: " << each_cycle << " -ID: " << i.id;
        }
        qDebug() << i->traffic_light_id << " USE_COUNT: " << each_use_count << " CYCLE: " << each_cycle;
        each_use_count /= model->getDumpCount();
        each_cycle /= model->getDumpCount();
        mean_use_count += each_use_count;
        cycle_time += each_cycle;
        model->getTimerTls().insert(i->traffic_light_id, new qint64(each_cycle));
        model->getMutTraffLById(i->traffic_light_id)->use_count = each_use_count;
        eachMut.use_count = each_use_count;
        eachMut.green_timer = each_cycle;
        mutTls.insert(i->traffic_light_id, eachMut);
    }
    mean_use_count /= newTls.size();
    tl_count = newTls.size();


}

void TraffLTimerWizardPage::buildTlTimerRecommendations()
{
    for(qint64 id : model->getTimerTls().keys()){
        qint64 each_count = mutTls.value(id).use_count - mean_use_count;
        *model->getTimerTls().value(id) += qMax((qint64)0, (qint64)each_count*mutTls.value(id).green_timer);
    }
}

void TraffLTimerWizardPage::build()
{
    ui->lineEdit_tl_count->setText(QString::number(tl_count));
    ui->lineEdit_cycle_time->setText(QString::number(cycle_time));
    ui->lineEdit_avg_use_count->setText(QString::number(mean_use_count));

    ui->tableView->setModel(new TrafficTimerTableModel(model, catTls, mutTls, model->getTimerTls(), mean_use_count, this));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    timerDelegate = new TraffLTimerItemDelegate(model->getTimerTls(), catTls, this);
    connect(timerDelegate, SIGNAL(closeEditor(QWidget*)), this, SLOT(onTimerChanged()));
    ui->tableView->setItemDelegate(timerDelegate);
}

TraffLTimerWizardPage::~TraffLTimerWizardPage()
{
    delete ui;
}

bool TraffLTimerWizardPage::validatePage()
{
    for(traffic_light* i : newTls.values()){
        if(catTls.value(i->traffic_light_id) != UNAVAILABLE_TL && catTls.value(i->traffic_light_id) != UNUSED_TL){

            mut_traffic_light eachMutTl = mutTls.value(i->traffic_light_id);
            eachMutTl.green_timer = *model->getTimerTls().value(i->traffic_light_id);
            mutTls.insert(i->traffic_light_id, eachMutTl);
            emit wizInfo(QString("UPDATE: TRAFFIC LIGHT: %1 's new green timer: %2").arg(i->traffic_light_id).arg(eachMutTl.green_timer));

            model->getDatabase()->replaceTrafficLight(*i);
            model->getDatabase()->onDumpTrafficLight(QList<mut_traffic_light>({mutTls.value(i->traffic_light_id)}));
        }else{
            model->getDatabase()->removeTrafficLight(*i);
            emit wizInfo(QString("Removing unavailable traffic light: %1").arg(i->traffic_light_id));
        }
    }

    return true;
}

void TraffLTimerWizardPage::onTimerChanged()
{
    qint64 value = 0;
    for(qint64* i : model->getTimerTls().values()){
        value += *i;
    }
    ui->lineEdit_cycle_time->setText(QString::number(value));
}
