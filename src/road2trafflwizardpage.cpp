#include "road2trafflwizardpage.h"
#include "ui_road2trafflwizardpage.h"

Road2TraffLWizardPage::Road2TraffLWizardPage(MapModel* model, QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::Road2TraffLWizardPage),
    model(model)
{
    ui->setupUi(this);
    setTitle("Recommended Traffic Lights");
    setSubTitle("Choose any traffic light as per preference (IN RECOMMENDATION): ");

    setCommitPage(true);

    buildMeanMutRds();
    buildPs();
    buildTlRecommendations();
    build();
}

void Road2TraffLWizardPage::buildMeanMutRds()
{
    for(mut_road *i : model->getMutRds().values()){
        for(mut_road j : model->getDatabase()->getAllMutRoadsById(i->road_id, model->getDumpCount())){
            i->vehicle_count += j.vehicle_count;
            i->vehicle_speed += j.vehicle_speed;
        }
        i->vehicle_count /= model->getDumpCount();
        i->vehicle_speed /= model->getDumpCount();
    }
}

void Road2TraffLWizardPage::buildPs()
{
    for(qint64 i : model->getTrafficBreakProbabs().keys()){
        *model->getTrafficBreakProbabById(i) = getTrafficBreakdownProbability(getTrafficDensity(model->getAllRds().value(i), *model->getMutRdById(i)), model->getDatabase()->getMeanMutRdById(i).vehicle_count);
    }
}

void Road2TraffLWizardPage::buildTlRecommendations()
{
    GraphColouring gc(model->getUpstreamRds(), model->getUpstreamRdAngles(), model->getDownstreamRds(), model->getDownstreamRdAngles(), model->getTrafficBreakProbabs());

    for(int i = 1; i <= gc.getRoutes().size(); i++){
        if(gc.graphColoring(i)){
            qDebug() << "NUMBER OF GRAPH COLORS: " << i;
            int* color = gc.getColor();
            QList<QPair<qint64, qint64>> routes = gc.getRoutes();

            QMap<qint64, mut_traffic_light*> color2trffL;
            tl_count = model->getMutTraffL().values().size();
            tl_update = i - tl_count;
            qDebug() << "tl_count: " << tl_count << " tl_update: " << tl_update;
            if(tl_update > 0){
                for(qint64 id : model->getDatabase()->getUnallocTraffL(tl_update)){
                    qDebug() << "NEW TL: " << id;
                    model->getMutTraffL().insert(id, new mut_traffic_light(id));
                }
            }
            qDebug() << "i : " << i;
            qDebug() << "mutTraffL: " << model->getMutTraffL().values().size();
            for(int j = 1; j <= i; j++){
                qDebug() << "ATTACHING COLOR: " << j << " TO " << model->getMutTraffL().values().at(j-1)->traffic_light_id;
                color2trffL.insert(j, model->getMutTraffL().values().at(j-1));
            }

            qDebug() << "COLORS:";
            for(int i = 0; i < routes.size(); i++){
                qDebug() << "ROUTE: " << i << "FROM " << routes[i].first << " TO " << routes[i].second << " COLOR: " << color[i];
                model->getAnalyTlByRoadId(routes[i].first)->clear();
                model->getAnalyTlByRoadId(routes[i].second)->clear();
            }

            for(int i = 0; i < routes.size(); i++){
                traffic_light tl = traffic_light(color2trffL.value(color[i])->traffic_light_id, model->getJunction().junction_id);
                qint64 upRdId = routes.at(i).first;
                if(!model->getAnalyTlByRoadId(upRdId)->contains(tl)){
                    model->getAnalyTlByRoadId(upRdId)->append(tl);
                }
                qint64 downRdId = routes.at(i).second;
                if(!model->getAnalyTlByRoadId(downRdId)->contains(tl)){
                    model->getAnalyTlByRoadId(downRdId)->append(tl);
                }
            }

            break;
        }
    }
}

void Road2TraffLWizardPage::build()
{
    ui->lineEdit_val_road_count->setText(QString::number(model->getAllRds().size()));
    ui->lineEdit_val_tl_count->setText(QString::number(tl_count));
    ui->lineEdit_tl_update->setText(QString::number(tl_update));

    ui->tableView->setModel(new Road2TraffLTableModel(model, this));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//    qint64 height = ui->tableView->horizontalHeader()->height();
//    height += ui->tableView->rowHeight(0)*model->getAllRds().size();
//    ui->tableView->setMaximumHeight(height);

    ui->tableView->setItemDelegate(new TraffLItemDelegate(model->getMutTraffL(), model->getAnalyTls(), this));
}

Road2TraffLWizardPage::~Road2TraffLWizardPage()
{
    delete ui;
}

QList<road> Road2TraffLWizardPage::getInvalidRoads()
{
    QList<road> out;
    for(road i : model->getAllRds().values()){
        if(model->getAnalyTlByRoadId(i.road_id)->isEmpty()){
            out.append(i);
        }
    }
    return out;
}

bool Road2TraffLWizardPage::validatePage()
{
    if(getInvalidRoads().isEmpty()){
        qDebug() << "Submitting new modified traffic lights";

        prev = model->getTraffLsByRd2TraffL(model->getTls());
        final = model->getTraffLsByRd2TraffL(model->getAnalyTls());
        qDebug() << "NO OF TLs: " << final.size();
        for(traffic_light *i : final.values()){
            emit wizInfo(QString("ORIGNAL: TRAFFIC LIGHT: %1 'S prev roads: %2").arg(prev.value(i->traffic_light_id)->traffic_light_id).arg(prev.value(i->traffic_light_id)->road_ids));
            emit wizInfo(QString("UPDATE: TRAFFIC LIGHT: %1 's new roads: %2").arg(i->traffic_light_id).arg(i->road_ids));
            emit wizInfo(QString("______________________________________________"));
        }

        // Modifying traffic lights

        return true;
    }else{
        QStringList rdNms;
        for(road i : getInvalidRoads()){
            rdNms.append(i.name);
        }
        QMessageBox::warning(this, "WARNING: ", QString("Please assign atleast 1 traffic light to each road\nFollowing are the unassigned roads:\n").arg(rdNms.join(',')));
        return false;
    }
}

QVector<double> Road2TraffLWizardPage::getMutTlPlotDat(QList<mut_traffic_light> mutTls, tlPlot tp)
{
    QVector<double> out = QVector<double>(mutTls.size());
    for(int i = mutTls.size()-1; i >= 0; --i){
        switch (tp){
        case TLPLOT_ID:
            out.append(mutTls[i].id);
            break;
        case TLPLOT_USE_COUNT:
            out.append(mutTls[i].use_count);
            break;
        case TLPLOT_GREEN_TIMER:
            out.append(mutTls[i].green_timer);
            break;
        }
    }
    return out;
}

QVector<QwtText> Road2TraffLWizardPage::getMutTlPlotX(QList<mut_traffic_light> mutTls)
{
    QVector<QwtText> out;
    for(int i = mutTls.size()-1; i >= 0; --i){
        out.append(mutTls[i].timestamp);
    }
    return out;
}

void Road2TraffLWizardPage::onShowGraph(qint64 id)
{
    QList<mut_traffic_light> datTl = model->getDatabase()->getAllMutTraffLsById(id, model->getDumpCount());

//    QPointer<QwtPlot> plot = new QwtPlot(QwtText(QString("Stats: Traffic Light: %1").arg(id)));

    qDebug() << "Drawing curve";
//    QwtPlotCurve *curve = new QwtPlotCurve(QString("green timer"));
    qDebug() << "collecting ids";
    QVector<double> ids = getMutTlPlotDat(datTl, TLPLOT_ID);
    qDebug() << "IDS SAMPLE SIZE: " << ids.size();
    QVector<double> green_timer = getMutTlPlotDat(datTl, TLPLOT_GREEN_TIMER);
    qDebug() << "GREEN TIMER: SAMPLE SIZE: " << green_timer.size();

    double ymin = getMin(green_timer);
    double ymax = getMax(green_timer);

    double xmin = getMin(ids);
    double xmax = getMax(ids);

    qDebug() << xmin << ", " << xmax;
    qDebug() << ymin << ", " << ymax;
    qDebug() << "Setting axis";
//    plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);

//            for(int i = 0; i < ids.size(); i++){
//                qDebug() << ids.at(i) << ", " << green_timer.at(i);
//            }

//    curve->setSamples(ids, green_timer);
//    curve->setTitle(QString("green timer"));
//    curve->setPen(QColor(Qt::green));
//    curve->setSymbol(new QwtSymbol(QwtSymbol::Cross));
//    curve->attach(plot);
//    plot->insertLegend(new QwtLegend());

//    QwtPlotCurve *curveUseCount = new QwtPlotCurve(QString("use count"));
    QVector<double> use_count = getMutTlPlotDat(datTl, TLPLOT_USE_COUNT);

    double ymin1 = getMin(use_count);
    double ymax1 = getMax(use_count);

//    plot->setAxisScale(QwtPlot::yLeft, qMin(ymin, ymin1), qMax(ymax, ymax1));
//    curveUseCount->setSamples(ids, use_count);
//    curveUseCount->setTitle(QString("use count"));
//    curveUseCount->setPen(QColor(Qt::blue));
//    curveUseCount->setSymbol(new QwtSymbol(QwtSymbol::Star1));
//    curveUseCount->attach(plot);
//    plot->insertLegend(new QwtLegend());

//    plot->setAxisTitle(QwtPlot::xBottom, QString("Samples"));

//    plot->resize(600, 400);
//    plot->show();

    QMap<QString, QVector<double>> data;
    data.insert("IDS", ids);
    data.insert("USE COUNT", use_count);
    data.insert("GREEN TIMER", green_timer);

    VisualGraphDialog *graph = new VisualGraphDialog(data, this);
    graph->show();
}

