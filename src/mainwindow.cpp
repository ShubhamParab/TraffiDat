#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    addLog("Welcome to TraffiDat Software. \nThe software provides simulation & analytics tools for traffic managment.", INFO);

    // connect
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(onNewFile()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(onOpenFile()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(onSaveFile()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onExitFile()));
    connect(ui->actionTrafficLight, SIGNAL(triggered()), this, SLOT(onTrafficLightAddEdit()));
    connect(ui->actionRoad, SIGNAL(triggered()), this, SLOT(onRoadAddEdit()));
    connect(ui->actionJunction, SIGNAL(triggered()), this, SLOT(onJunctionAddEdit()));
    connect(ui->actionEditRoad, SIGNAL(triggered()), this, SLOT(onEditRoadEdit()));
    connect(ui->actionRemoveJunction, SIGNAL(triggered()), this, SLOT(onJunctionAddRemove()));
    connect(ui->actionRoadView, SIGNAL(triggered()), this, SLOT(onRoadView()));
    connect(ui->actionJunctionView, SIGNAL(triggered()), this, SLOT(onJunctionView()));
    connect(ui->actionStartPlay, SIGNAL(triggered()), this, SLOT(onStartPlay()));
    connect(ui->actionPausePlay, SIGNAL(triggered()), this, SLOT(onPausePlay()));
    connect(ui->actionStopPlay, SIGNAL(triggered()), this, SLOT(onStopPlay()));
    connect(ui->actionGotoPlay, SIGNAL(triggered()), this, SLOT(onGotoPlay()));
    connect(ui->actionRoadVisual, SIGNAL(triggered()), this, SLOT(onRoadVisual()));
    connect(ui->actionTrafficLightVisual, SIGNAL(triggered()), this, SLOT(onTrafficLightVisual()));
    connect(ui->actionRoadPumpTest, SIGNAL(triggered()), this, SLOT(onRoadPumpTestClicked()));
    connect(ui->actionTrafficLightPumpTest, SIGNAL(triggered()), this, SLOT(onTrafficLightPumpTestClicked()));
    connect(ui->actionRoadDumpTest, SIGNAL(triggered()), this, SLOT(onRoadDumpTestClicked()));
    connect(ui->actionTrafficLightDumpTest, SIGNAL(triggered()), this, SLOT(onTrafficLightDumpTestClicked()));

    // actions
    ui->treeView_struct->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeView_struct->addAction(ui->actionRoadView);
    ui->treeView_struct->addAction(ui->actionJunctionView);

//    videoWidget = new QVideoWidget;
//    players = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
//    players->setPlaylist(new QMediaPlaylist(this));
//    connect(players, SIGNAL(durationChanged(qint64)), this, SLOT(onVideoChange(qint64)));

    listRoad = new QListWidget;
    listRoad->setContextMenuPolicy(Qt::ActionsContextMenu);
    listRoad->addAction(ui->actionGotoPlay);

    // creating verticalLayout_3
//    ui->verticalLayout_3->addWidget(new QLabel(tr("CCTV: ")));
//    ui->verticalLayout_3->addWidget(videoWidget);
    ui->verticalLayout_3->addWidget(new QLabel(tr("Roads: ")));
    ui->verticalLayout_3->addWidget(listRoad);
    ui->verticalLayout_3->setStretch(1, 3);
    ui->verticalLayout_3->setStretch(3, 4);

    ui->splitter->setStretchFactor(0, 22);
    ui->splitter->setStretchFactor(1, 11);

    ui->splitter_2->setStretchFactor(0, 3);
    ui->splitter_2->setStretchFactor(1, 1);

    ui->splitter_3->setStretchFactor(0, 1);
    ui->splitter_3->setStretchFactor(1, 3);

    dumpTimer = new QTimer(this);
    connect(dumpTimer, SIGNAL(timeout()), this, SLOT(onDump()));

    setWindowTitle("TraffiDat");
}

void MainWindow::build(QString fileName)
{
    this->fileName = fileName;
    db = new Database(fileName, this);
    structTreeModel = new StructTreeModel(db, this);
    ui->treeView_struct->setModel(structTreeModel);

//    QListWidgetItem *item = new QListWidgetItem(listRoad);
//    itemWidget =  new RoadWidget(this, db->getAvailRoads().last(), "TEST 1");
//    item->setSizeHint(itemWidget->sizeHint());
//    listRoad->setItemWidget(item, itemWidget);

    addRoadDialog = new AddRoadDialog(this);
    addJunctionDialog = new AddJunctionDialog(this);
    addTrafficLightDialog = new AddTrafficLightDialog(db, this);
    dumpTestRoadDialog = new DumpTestRoadDialog(this);
    dumpTestTrafficLightDialog = new DumpTestTrafficLightDialog(this);
}

void MainWindow::roadToWdgt(QList<road> rds, QString junct)
{
    roadWdgts.clear();
    rdWdgtItms.clear();
    listRoad->clear();
    for(road i : rds){
        // video
        // players->playlist()->addMedia(QUrl::fromLocalFile(i.video_addr.replace('\\\\', '\\')));

        // widgets
        RoadWidget *rdWidget = new RoadWidget(this, i, junct);
        roadWdgts.insert(i.road_id, rdWidget);
        QListWidgetItem *rdWdgtItm = new QListWidgetItem;
        rdWdgtItms.insert(i.road_id, rdWdgtItm);
    }
    for(qint64 i : roadWdgts.keys()){
        listRoad->addItem(rdWdgtItms.value(i));
        rdWdgtItms.value(i)->setSizeHint(roadWdgts.value(i)->sizeHint());
        listRoad->setItemWidget(rdWdgtItms.value(i), roadWdgts.value(i));
    }
//    players->setMedia(QUrl::fromLocalFile(roadWdgts.first()->getRoad().video_addr.replace('\\', '\\\\')));
//    players->setMuted(true);
    //    players->setVideoOutput(videoWidget);
}

QVector<double> MainWindow::getMutRdPlotDat(QList<mut_road> mutRds, rPlot rp)
{
    QVector<double> out = QVector<double>(mutRds.size());
    for(int i = mutRds.size()-1; i >= 0; --i){
        switch (rp){
        case RPLOT_ID:
            out.append(mutRds[i].id);
            break;
        case RPLOT_VEHICLE_COUNT:
            out.append(mutRds[i].vehicle_count);
            break;
        case RPLOT_VEHICLE_SPEED:
            out.append(mutRds[i].vehicle_speed);
            break;
        case RPLOT_CONGESTION_COUNT:
            out.append(mutRds[i].congestion_count);
            break;
        }
    }
    return out;
}

QVector<QwtText> MainWindow::getMutRdPlotX(QList<mut_road> mutRds)
{
    QVector<QwtText> out;
    for(int i = mutRds.size()-1; i >= 0; --i){
        out.append(mutRds[i].timestamp);
    }
    return out;
}

QVector<double> MainWindow::getMutTlPlotDat(QList<mut_traffic_light> mutTls, tlPlot tp)
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

QVector<QwtText> MainWindow::getMutTlPlotX(QList<mut_traffic_light> mutTls)
{
    QVector<QwtText> out;
    for(int i = mutTls.size()-1; i >= 0; --i){
        out.append(mutTls[i].timestamp);
    }
    return out;
}

MainWindow::~MainWindow()
{
    qDeleteAll(roadWdgts);
    qDeleteAll(rdWdgtItms);
    if(ui != nullptr)
        delete ui;
    if(db != nullptr)
        delete db;
    if(addRoadDialog != nullptr)
        delete addRoadDialog;
    if(addJunctionDialog != nullptr)
        delete addJunctionDialog;
    if(addTrafficLightDialog != nullptr)
        delete addTrafficLightDialog;
    if(dumpTestRoadDialog != nullptr)
        delete dumpTestRoadDialog;
    if(dumpTestTrafficLightDialog != nullptr)
        delete dumpTestTrafficLightDialog;
    if(structTreeModel != nullptr)
        delete structTreeModel;
//    if(videoWidget != nullptr)
//        delete videoWidget;
//    if(players != nullptr)
//        delete players;
    if(listRoad != nullptr)
        delete listRoad;
//    if(currPlayer != nullptr)
//        delete currPlayer;
    if(dumpTimer != nullptr)
        delete dumpTimer;
    if(detect != nullptr)
        delete detect;
    if(mapScene != nullptr)
        delete mapScene;
    if(mapModel != nullptr)
        delete mapModel;

//    if(graph != nullptr)
//        delete graph;
    if(plot != nullptr)
        delete plot;
}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Plus){
        ui->graphicsView_map->scale(2, 2);
    }else if(ev->key() == Qt::Key_Minus){
        ui->graphicsView_map->scale(0.5, 0.5);
    }
}

void MainWindow::onNewFile()
{
    fileName = QFileDialog::getSaveFileName(this, tr("Create new File..."), QDir::currentPath(), tr("Database Files (*.db)"));
    if(fileName != ""){
        build(fileName);
    }
}

void MainWindow::onOpenFile()
{
    fileName = QFileDialog::getOpenFileName(this, tr("Open new File..."), QDir::currentPath(), tr("Database Files (*.db)"));
    if(fileName != ""){
        build(fileName);
    }
}

void MainWindow::onSaveFile()
{
    if(db != nullptr){
        db->saveAll();
    }
}

void MainWindow::onExitFile()
{
    QCoreApplication::quit();
}

void MainWindow::onTrafficLightAddEdit()
{
    if(db != nullptr){
        addTrafficLightDialog->build(db->getAvailJuncts(), db->getAvailRoads());
        if(addTrafficLightDialog->exec() == QDialog::Accepted){
            if(!db->addTrafficLight(addTrafficLightDialog->getData()))
                qDebug() << "Failed adding new traffic light";
            else {
                db->saveAll();
            }
        }
    }
}

void MainWindow::onRoadAddEdit()
{
    if(db != nullptr){
        if(addRoadDialog->exec() == QDialog::Accepted){
            if(!db->addRoad(addRoadDialog->getData()))
                qDebug() << "Failed adding new road";
            else {
                db->saveAll();
            }
        }
    }
}

void MainWindow::onJunctionAddEdit()
{
    if(db != nullptr){
        addJunctionDialog->build(db->getAvailRoads());
        if(addJunctionDialog->exec() == QDialog::Accepted){
            if(!db->addJunction(addJunctionDialog->getData()))
                qDebug() << "Failed adding new junction";
            else {
                db->saveAll();
            }
        }
    }
}

void MainWindow::onJunctionAddRemove()
{
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == JUNCTION_ITEM){
            junction j = iItem->getJunct();

            db->removeJunction(j);
        }
    }
}

void MainWindow::onEditRoadEdit()
{
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == UPSTREAM_ROAD_ITEM || iItem->getItemType() == DOWNSTREAM_ROAD_ITEM){
            road select = iItem->getRoad();
            addRoadDialog->setData(select);
            if(addRoadDialog->exec() == QDialog::Accepted){
                road update = addRoadDialog->getData();
                update.road_id = select.road_id;
                db->updateRoad(update);
            }
        }
    }
}

void MainWindow::onRoadView()
{
//    rds.clear();
//    mutRds.clear();
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == UPSTREAM_ROAD_ITEM || iItem->getItemType() == DOWNSTREAM_ROAD_ITEM){
            addLog(repr_road(iItem->getRoad()));
            roadToWdgt(QList<road>({iItem->getRoad()}), iItem->getJunct().name);
            mapModel = new MapModel(ROAD_MAP_TYPE, db);
            connect(mapModel, SIGNAL(mapInfo(QString,qint64)), this, SLOT(addLog(QString, qint64)));
            mapModel->build(iItem->getJunct(), iItem->getRoad());
            connect(mapModel->getTimer(), SIGNAL(timeout()), this, SLOT(updateStats()));

//            rds.insert(iItem->getRoad().road_id, iItem->getRoad());
//            mutRds.insert(iItem->getRoad().road_id, new mut_road(iItem->getRoad().road_id));

            mapScene = new MapGraphicsScene(mapModel);
            connect(mapModel->getTimer(), SIGNAL(timeout()), mapScene, SLOT(updateTime()));

            ui->graphicsView_map->setScene(mapScene);
            ui->graphicsView_map->fitInView(mapScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
            ui->graphicsView_map->setDragMode(QGraphicsView::ScrollHandDrag);
        }
    }
}

void MainWindow::onJunctionView()
{
//    rds.clear();
//    mutRds.clear();
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == JUNCTION_ITEM){
            addLog(repr_junction(iItem->getJunct()));
            roadToWdgt(db->roadsByJunction(iItem->getJunct()), iItem->getJunct().name);
            mapModel = new MapModel(JUNCTION_MAP_TYPE, db);
            connect(mapModel, SIGNAL(mapInfo(QString,qint64)), this, SLOT(addLog(QString, qint64)));
            mapModel->build(iItem->getJunct(), iItem->getRoad());
            connect(mapModel->getTimer(), SIGNAL(timeout()), this, SLOT(updateStats()));

//            for(road i : mapModel->getUpstreamRds()){
//                mutRds.insert(i.road_id, new mut_road(i.road_id));
//                rds.insert(i.road_id, i);
//            }
//            for(road i : mapModel->getDownstreamRds()){
//                mutRds.insert(i.road_id, new mut_road(i.road_id));
//                rds.insert(i.road_id, i);
//            }

            mapScene = new MapGraphicsScene(mapModel);
            connect(mapModel->getTimer(), SIGNAL(timeout()), mapScene, SLOT(updateTime()));

            ui->graphicsView_map->setScene(mapScene);
            ui->graphicsView_map->fitInView(mapScene->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);
            ui->graphicsView_map->setDragMode(QGraphicsView::ScrollHandDrag);
        }
    }
}

void MainWindow::onStartPlay()
{
    if(db != nullptr && mapModel != nullptr){
        if(mapModel->getUnallocRoads().isEmpty()){
            db->buildMeanValues();
    //        players->setPosition(0);
    //        players->play();
            playType = START_PLAY;

            if(detect == nullptr){
                if(mapModel->getMutRds().size()>0){
                        for(mut_road *i : mapModel->getMutRds()){
                            i->vehicle_count = 0;
                            i->vehicle_speed = -1;
                        }
                        detect = new Detection(mapModel->getAllRds(), &mapModel->getMutRds(), &mapModel->getSpeedRds(), &mapModel->getCongRds(), this);
                        connect(detect, SIGNAL(detectInfo(QString,qint64)), this, SLOT(addLog(QString, qint64)));
                        connect(detect, SIGNAL(finish(qint64)), this, SLOT(finished(qint64)));
                        detect->start();
                }
            }
            ui->actionStartPlay->setEnabled(false);
            ui->actionPausePlay->setEnabled(true);
            ui->actionStopPlay->setEnabled(true);
            mapModel->startTimer();
            dumpTimer->start(60*1000);
        }else{
            QList<road> unallocRds = mapModel->getUnallocRoads();
            QStringList rdNmList;
            for(road i : unallocRds){
                rdNmList.append(i.name);
            }
            QMessageBox::warning(this, tr("WARNING: "), tr("Please assign traffic lights to every road: \nFollowing are the unassigned roads: \n%1").arg(rdNmList.join(',')));
            addLog(QString("WARNING: Please assign traffic lights to every road: \nFollowing are the unassigned roads: \n%1").arg(rdNmList.join(',')), WARNING);
        }
    }
}

void MainWindow::onPausePlay()
{
    if(db != nullptr){
        if(playType == START_PLAY){
//            players->pause();
            playType = PAUSE_PLAY;
            ui->actionPausePlay->setText("Resume");
        }else if(playType == PAUSE_PLAY){
//            players->play();
            playType = START_PLAY;
            ui->actionPausePlay->setText("Pause");
        }

        if(mapModel->getMutRds().size()>0){
            if(detect != nullptr){
                detect->pause();
            }
        }
    }
}

void MainWindow::onStopPlay()
{
    finished(0);
}

void MainWindow::onGotoPlay()
{
    if(db != nullptr){
        QModelIndex i_list = listRoad->currentIndex();
        if(i_list.isValid()){
//            seekTime = players->position();
            RoadWidget *iItem = static_cast<RoadWidget*>(listRoad->itemWidget(static_cast<QListWidgetItem*>(i_list.internalPointer())));
//            players->setMedia(QUrl::fromLocalFile(iItem->getRoad().video_addr.replace('\\', '\\\\')));

//            players->play();
            if(detect != nullptr){
                detect->scrollTo(iItem->getRoad().road_id);
            }
            playType = START_PLAY;
        }
    }
}
void MainWindow::onRoadPumpTestClicked()
{
    if(db != nullptr){
        bool ok;
        int id = QInputDialog::getInt(this, tr("Pump Road Test: "), tr("Enter id: "), -1, -1, 100, 1, &ok);
        if(ok){
            mut_road rslt = db->onPumpRoad(id);
            addLog(repr_mut_road(rslt));
            addLog(QString("________________________________________"));
        }

    }
}

void MainWindow::onTrafficLightPumpTestClicked()
{
    if(db != nullptr){
        bool ok;
        int id = QInputDialog::getInt(this, tr("Pump Traffic Light: "), tr("Enter id: "), -1, -1, 100, 1, &ok);
        if(ok){
            mut_traffic_light rslt = db->onPumpTrafficLight(id);
            addLog(repr_mut_traffic_light(id));
            addLog(QString("_________________________________________"));
        }
    }
}

void MainWindow::onRoadDumpTestClicked()
{
    if(db != nullptr){
        dumpTestRoadDialog->build(db->getAvailRoads());
        if(dumpTestRoadDialog->exec() == QDialog::Accepted){
            db->onDumpRoad(QList<mut_road>({dumpTestRoadDialog->getData()}));
        }
    }
}

void MainWindow::onTrafficLightDumpTestClicked()
{
    if(db != nullptr){
        dumpTestTrafficLightDialog->build(db->getAvailTraffL());
        if(dumpTestTrafficLightDialog->exec() == QDialog::Accepted){
            db->onDumpTrafficLight(QList<mut_traffic_light>({dumpTestTrafficLightDialog->getData()}));
        }
    }
}

void MainWindow::onRoadVisual()
{
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == UPSTREAM_ROAD_ITEM || iItem->getItemType() == DOWNSTREAM_ROAD_ITEM){
            road thisRoad = iItem->getRoad();
            QList<mut_road> datRoad = db->getAllMutRoadsById(thisRoad.road_id);
            qDebug() << "Road: " << thisRoad.road_id << " Sample Size: " << datRoad.size();

            plot = new QwtPlot(QwtText(QString("Stats: Road: %1(%2)").arg(thisRoad.name).arg(thisRoad.road_id)));

            QVector<double> ids = getMutRdPlotDat(datRoad, RPLOT_ID);
            QVector<double> count = getMutRdPlotDat(datRoad, RPLOT_VEHICLE_COUNT);
            QVector<double> speed = getMutRdPlotDat(datRoad, RPLOT_VEHICLE_SPEED);
            QVector<double> congestion = getMutRdPlotDat(datRoad, RPLOT_CONGESTION_COUNT);
            qDebug() << ids.size() << ", " << count.size() << ", " << speed.size() << ", " << congestion.size();

            QwtPlotCurve *curveCount = new QwtPlotCurve(QString("vehicle count"));
            curveCount->setSamples(ids, count);
            curveCount->setTitle("Vehicle Count");
            curveCount->setPen(QColor(Qt::blue));
            curveCount->setSymbol(new QwtSymbol(QwtSymbol::Cross));
            curveCount->attach(plot);
            plot->insertLegend(new QwtLegend());

            QwtPlotCurve *curveSpeed = new QwtPlotCurve(QString("vehicle speed"));
            curveSpeed->setSamples(ids, speed);
            curveSpeed->setTitle("Vehicle speed");
            curveSpeed->setPen(QColor(Qt::green));
            curveSpeed->setSymbol(new QwtSymbol(QwtSymbol::Star1));
            curveSpeed->attach(plot);
            plot->insertLegend(new QwtLegend());

            QwtPlotCurve *curveCongestion = new QwtPlotCurve(QString("Congestion Count"));
            curveCongestion->setSamples(ids, congestion);
            curveCongestion->setTitle("Congestion Count");
            curveCongestion->setPen(QColor(Qt::red));
            curveCongestion->setSymbol(new QwtSymbol(QwtSymbol::VLine));
            curveCongestion->attach(plot);
            plot->insertLegend(new QwtLegend());

            plot->setAxisTitle(QwtPlot::xBottom, QString("Samples"));
            plot->setAxisScale(QwtPlot::xBottom, 0, ids.size());
            plot->setAxisScale(QwtPlot::yLeft, qMin(getMin(count), qMin(getMin(speed), getMin(congestion))), qMax(getMax(count), qMax(getMax(speed), getMax(congestion))));

            plot->resize(600, 400);
            plot->show();

            QMap<QString, QVector<double>> data;
            data.insert("IDS", ids);
            data.insert("VEHICLE COUNT", count);
            data.insert("VEHICLE SPEED", speed);
            data.insert("VEHICLE CONGESTION", congestion);

            VisualGraphDialog *graph = new VisualGraphDialog(data, this);
            graph->show();
        }
    }
}

void MainWindow::onTrafficLightVisual()
{
    QModelIndex i = ui->treeView_struct->currentIndex();
    if(i.isValid()){
        StructTreeItem *iItem = static_cast<StructTreeItem*>(i.internalPointer());
        if(iItem->getItemType() == TRAFFIC_LIGHT_ITEM){
            traffic_light thisTl = iItem->getTraffL();
            QList<mut_traffic_light> datTl = db->getAllMutTraffLsById(thisTl.traffic_light_id);
            qDebug() << "TL: " << thisTl.traffic_light_id << " Sample Size: " << datTl.size();

            QPointer<QwtPlot> plot = new QwtPlot(QwtText(QString("Stats: Traffic Light: %1").arg(thisTl.traffic_light_id)));

            qDebug() << "Drawing curve";
            QwtPlotCurve *curve = new QwtPlotCurve(QString("green timer"));
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
            plot->setAxisScale(QwtPlot::xBottom, xmin, xmax);

//            for(int i = 0; i < ids.size(); i++){
//                qDebug() << ids.at(i) << ", " << green_timer.at(i);
//            }

            curve->setSamples(ids, green_timer);
            curve->setTitle(QString("green timer"));
            curve->setPen(QColor(Qt::green));
            curve->setSymbol(new QwtSymbol(QwtSymbol::Cross));
            curve->attach(plot);
            plot->insertLegend(new QwtLegend());

            QwtPlotCurve *curveUseCount = new QwtPlotCurve(QString("use count"));
            QVector<double> use_count = getMutTlPlotDat(datTl, TLPLOT_USE_COUNT);

            double ymin1 = getMin(use_count);
            double ymax1 = getMax(use_count);

            plot->setAxisScale(QwtPlot::yLeft, qMin(ymin, ymin1), qMax(ymax, ymax1));
            curveUseCount->setSamples(ids, use_count);
            curveUseCount->setTitle(QString("use count"));
            curveUseCount->setPen(QColor(Qt::blue));
            curveUseCount->setSymbol(new QwtSymbol(QwtSymbol::Star1));
            curveUseCount->attach(plot);
            plot->insertLegend(new QwtLegend());

            plot->setAxisTitle(QwtPlot::xBottom, QString("Samples"));

            plot->resize(600, 400);
            plot->show();

            QMap<QString, QVector<double>> data;
            data.insert("IDS", ids);
            data.insert("USE COUNT", use_count);
            data.insert("GREEN TIMER", green_timer);

            VisualGraphDialog *graph = new VisualGraphDialog(data, this);
            graph->show();
        }
    }
}

void MainWindow::addLog(QString msg, qint64 styleType)
{
    QString font_color = "black";
    if(styleType == INFO)
        font_color = "green";
    else if(styleType == VERBOSE)
        font_color = "gray";
    else if(styleType == WARNING)
        font_color = "greenyellow";
    else if(styleType == ERROR)
        font_color = "red";
    qDebug() << msg << "\n";
    ui->textEdit_log->append(QString("\n<h4 style='color:%1;'>%2</h2>").arg(font_color).arg(msg));
}

//void MainWindow::onVideoChange(qint64 duration)
//{
//    if(duration != 0){
//        qint64 totalTime = duration;
//        players->setPosition(seekTime%totalTime);
//    }
//}

void MainWindow::finished(qint64 road_id)
{
    Q_UNUSED(road_id);
    if(dumpTimer != nullptr){
        if(mapModel != nullptr && mapScene != nullptr){
            dumpTimer->stop();
            mapModel->stopTimer();
    //        players->stop();
            playType = STOP_PLAY;
            if(mapModel->getMutRds().size()>0){
                for(mut_traffic_light* i : mapModel->getMutTraffL().values()){
                    qDebug() << "TL ID: " << i->traffic_light_id << " USE COUNT: " << i->use_count;
                }
                if(detect != nullptr){
                    detect ->stop();
                    detect = nullptr;
                    emit addLog(QString("%1 data dumps successfully.").arg(mapModel->getDumpCount()));
                    if(mapModel->getDumpCount() > 0 && mapModel->getMapType() == JUNCTION_MAP_TYPE){
                        // analytics here
                        addLog(QString("Starting analytics..."), INFO);
                        analyt = new AnalyticsWizard(mapModel, this);
                        analyt->setModal(false);
                        analyt->setWindowModality(Qt::NonModal);

                        if(analyt->exec() == QWizard::Accepted){
                            addLog("Finished analytics");
                            QMessageBox::StandardButton ok = QMessageBox::question(this, tr("Generate Report..."), tr("Do you want to generate report for this session?"));
                            if(ok == QMessageBox::StandardButton::Yes){
                                qDebug() << "Generating Report....";
                                SessionReport* report = new SessionReport(mapModel, this);
                                report->show();
                            }
                        }
                    }
                }
            }
            ui->actionStartPlay->setEnabled(true);
            ui->actionPausePlay->setEnabled(false);
            ui->actionStopPlay->setEnabled(false);
            mapModel->stopTimer();
            mapScene->setSecTime(0);
        }
    }
}

void MainWindow::updateStats()
{
    if(mapModel != nullptr && mapScene != nullptr){
        if(mapModel->getMutRds().size()>0){
            for(mut_road *i : mapModel->getMutRds()){
                roadWdgts.value(i->road_id)->setStats(i->vehicle_count, *mapModel->getSpeedRdById(i->road_id), *mapModel->getTraffLById(i->road_id));
                mapModel->setTrafficBreakProbabById(i->road_id, float(getTrafficBreakdownProbability(getTrafficDensity(mapModel->getAllRds().value(i->road_id), *i), db->getMeanMutRdById(i->road_id).vehicle_count)));
                mapModel->updateTrafficLights();
                mapScene->update();
            }
        }
    }
}

void MainWindow::onDump()
{
    if(mapModel != nullptr && db != nullptr){
        if(mapModel->getMutRds().size() > 0){
            QList<mut_road> mutOuts;
            for(mut_road *i : mapModel->getMutRds()){
                setMutRoadTimestamp(i);
                i->vehicle_speed = (int)*mapModel->getSpeedRdById(i->road_id);
                mutOuts.append(*i);
                setMutRoadVehCount(i, 0);
                setMutRoadVehSpeed(i, 0);
                setMutRoadCongCount(i, 0);
            }
            db->onDumpRoad(mutOuts);
        }
        if(mapModel->getMutTraffL().size() > 0){
            QList<mut_traffic_light> mutTl;
            for(mut_traffic_light *i : mapModel->getMutTraffL()){
                setMutTraffLTimestamp(i);
                mutTl.append(*i);
                setMutTraffLUseCount(i, 0);
            }
            db->onDumpTrafficLight(mutTl);
        }
        mapModel->incrDumpCount();
    }
}
