#include "sessionreport.h"

SessionReport::SessionReport(MapModel *model, QObject *parent):
    QObject (parent),
    model(model)
{
    build();
}

void SessionReport::build()
{
    QDateTime dt = QDateTime::currentDateTime();
    report.setDocumentName(QString("JUNCTION: %1(%2)").arg(model->getJunction().name).arg(dt.toString(Qt::ISODate)));
    report.setWatermarkText(QString("TRAFFIDAT"), 45);

    KDReports::TextElement headText = KDReports::TextElement(QString("Session Report: " ));
    headText.setBold(true);
    headText.setPointSize(32);
    report.addElement(headText, Qt::AlignCenter);

    KDReports::TextElement dateText = KDReports::TextElement(QString("Date: %1").arg(dt.toString(Qt::ISODate)));
    dateText.setPointSize(18);
    report.addElement(dateText, Qt::AlignRight);

    KDReports::HLineElement lineElement = KDReports::HLineElement();
    lineElement.setThickness(5);
    report.addElement(lineElement);

    QList<qint64> upIds = model->getDatabase()->stringToIds(model->getJunction().up_road_ids);
    QList<qint64> downIds = model->getDatabase()->stringToIds(model->getJunction().down_road_ids);

    KDReports::TextElement junctionDetailsText = KDReports::TextElement(QString("JUNCTION NAME: %1\nAREA: %2\nNUMBER OF UPSTREAM ROADS: %3\nNUMBER OF DOWNSTREAM ROADS: %4").arg(model->getJunction().name).arg(model->getJunction().area).arg(upIds.size()).arg(downIds.size()));
    junctionDetailsText.setPointSize(16);
    report.addElement(junctionDetailsText);

    KDReports::HLineElement dottedLineElement = KDReports::HLineElement();
    dottedLineElement.setThickness(3);
    dottedLineElement.setColor(QColor(Qt::lightGray));
    report.addElement(dottedLineElement);

    QMap<qint64, traffic_light*> oldTls = model->getTraffLsByRd2TraffL(model->getTls());
    QMap<qint64, traffic_light*> newTls = model->getTraffLsByRd2TraffL(model->getAnalyTls());
    QMap<qint64, TlCategory> catTls;

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

    qDebug() << "ALL TL: " << catTls.values().size() << " UNAVAILABLE TL: " << catTls.keys(TlCategory::UNAVAILABLE_TL).size() << " NEW TL: " << catTls.keys(TlCategory::NEW_TL).size();
    qint64 allTl = catTls.values().size();
    qint64 deltaTl = allTl - catTls.keys(TlCategory::UNAVAILABLE_TL).size() + catTls.keys(TlCategory::NEW_TL).size();
    qDebug() << "deltaTl: " << deltaTl;
    KDReports::TextElement traffLText =  KDReports::TextElement(QString("ORIGNAL TRAFFIC LIGHTS COUNT: %1\t\t\t\t\tUPDATED TRAFFIC LIGHTS COUNT: %2").arg(allTl).arg(deltaTl));
    traffLText.setTextColor(QColor(Qt::black));
    report.addElement(traffLText);
    KDReports::TextElement deltaTlElement = KDReports::TextElement();
    if(deltaTl < allTl){
        qreal perc = ((float)(allTl - deltaTl)/allTl)*100;
        deltaTlElement.setText(QString("Number of traffic lights reduced by %1 %").arg(perc));
        deltaTlElement.setTextColor(QColor(Qt::green));
    }else if(deltaTl == allTl){
        deltaTlElement.setText(QString("Number of traffic lights remain same"));
        deltaTlElement.setTextColor(QColor(Qt::darkYellow));
    }else{
        qreal perc = ((float)(deltaTl - allTl)/allTl)*100;
        deltaTlElement.setText(QString("Number of traffic lights increased by %1 %").arg(perc));
        deltaTlElement.setTextColor(QColor(Qt::red));
    }
    report.addElement(deltaTlElement);

    qint64 originalCycleTime = 0;
    for(mut_traffic_light* i : model->getPreMutTraffL().values()){
        originalCycleTime += i->green_timer;
    }
    qint64 updatedCycleTime = 0;
    for(qint64* i : model->getTimerTls().values()){
        updatedCycleTime += *i;
    }
    KDReports::TextElement cycleTimeText = KDReports::TextElement(QString("ORIGINAL CYCLE TIME: %1\t\t\t\t\t\tUPDATED CYCLE TIME: %2").arg(originalCycleTime).arg(updatedCycleTime));
    cycleTimeText.setTextColor(QColor(Qt::black));
    report.addElement(cycleTimeText);
    KDReports::TextElement deltaCycleTimeElement = KDReports::TextElement();
    if(originalCycleTime > updatedCycleTime){
        qint64 deltaCycleTime = originalCycleTime - updatedCycleTime;
        deltaCycleTimeElement.setText(QString("Cycle time reduced by %1 sec").arg(deltaCycleTime));
        deltaCycleTimeElement.setTextColor(QColor(Qt::red));
    }else if(originalCycleTime == updatedCycleTime){
        deltaCycleTimeElement.setText(QString("Cycle time remain same"));
        deltaCycleTimeElement.setTextColor(QColor(Qt::darkYellow));
    }else{
        qint64 deltaCycleTime = updatedCycleTime - originalCycleTime;
        deltaCycleTimeElement.setText(QString("Cycle time increased by %1 sec").arg(deltaCycleTime));
        deltaCycleTimeElement.setTextColor(QColor(Qt::green));
    }
    report.addElement(deltaCycleTimeElement);
    report.addElement(KDReports::HLineElement());

    report.addElement(KDReports::TextElement(QString("ROAD DETAILS: ")));

    KDReports::AutoTableElement roadDetailsTable = KDReports::AutoTableElement(new RoadDetailTableModel(model));
    report.addElement(roadDetailsTable, Qt::AlignCenter);

    report.addElement(KDReports::TextElement(QString("CHANGE DETAILS: ")));
    report.addElement(KDReports::AutoTableElement(new Road2TraffLChangeTableModel(model)), Qt::AlignCenter);
    report.addElement(KDReports::AutoTableElement(new TraffL2TimerChangeTableModel(model)), Qt::AlignCenter);
}

void SessionReport::show()
{
    KDReports::PreviewDialog preview(&report);
    preview.resize(640, 860);
    preview.exec();
}
