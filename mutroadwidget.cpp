#include "mutroadwidget.h"
#include "ui_mutroadwidget.h"

MutRoadWidget::MutRoadWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MutRoadWidget),
    parent(parent)
{
    ui->setupUi(this);
    connect(this, SIGNAL(showWidget(QWidget*)), parent, SLOT(onShowWidget(QWidget*)));
    connect(ui->pushButton_view_graph, SIGNAL(clicked()), this, SLOT(onViewGraph()));
}

QVector<double> MutRoadWidget::getMutRdPlotDat(QList<mut_road> mutRds, rPlot rp)
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

QVector<QwtText> MutRoadWidget::getMutRdPlotX(QList<mut_road> mutRds)
{
    QVector<QwtText> out;
    for(int i = mutRds.size()-1; i >= 0; --i){
        out.append(mutRds[i].timestamp);
    }
    return out;
}

void MutRoadWidget::setData(road rd, QList<mut_road> mutRd, mut_traffic_light tl)
{
    this->rd = rd;
    this->mutRd = mutRd;
    this->tl = tl;
    ui->lineEdit_val_id->setText(QString::number(rd.road_id));
    ui->lineEdit_val_tl_id->setText(QString("ID: %1").arg(tl.traffic_light_id));
    ui->lineEdit_val_name->setText(rd.name);
    ui->lineEdit_val_area->setText(QString("%1 x %2 = %3").arg(rd.length).arg(rd.lanes).arg(rd.length*rd.lanes));
    ui->lineEdit_val_damage->setText(QString("%1 %").arg(rd.perc_damage));

    // build table
    ui->tableView->setModel(new MutRoadTableModel(mutRd, this));
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    qint64 height = ui->tableView->horizontalHeader()->height();
    height += ui->tableView->rowHeight(0)*mutRd.size();
    ui->tableView->setMaximumHeight(height);
}

MutRoadWidget::~MutRoadWidget()
{
    delete ui;
//    if(plot != nullptr)
//        delete plot;
}

void MutRoadWidget::onViewGraph()
{
    road thisRoad = rd;
    QList<mut_road> datRoad = mutRd;
    qDebug() << "Road: " << thisRoad.road_id << " Sample Size: " << datRoad.size();

//    plot = new QwtPlot(QwtText(QString("Stats: Road: %1(%2)").arg(thisRoad.name).arg(thisRoad.road_id)));

    QVector<double> ids = getMutRdPlotDat(datRoad, RPLOT_ID);
    QVector<double> count = getMutRdPlotDat(datRoad, RPLOT_VEHICLE_COUNT);
    QVector<double> speed = getMutRdPlotDat(datRoad, RPLOT_VEHICLE_SPEED);
    QVector<double> congestion = getMutRdPlotDat(datRoad, RPLOT_CONGESTION_COUNT);
    qDebug() << ids.size() << ", " << count.size() << ", " << speed.size() << ", " << congestion.size();

//    QwtPlotCurve *curveCount = new QwtPlotCurve(QString("vehicle count"));
//    curveCount->setSamples(ids, count);
//    curveCount->setTitle("Vehicle Count");
//    curveCount->setPen(QColor(Qt::blue));
//    curveCount->setSymbol(new QwtSymbol(QwtSymbol::Cross));
//    curveCount->attach(plot);
//    plot->insertLegend(new QwtLegend());

//    QwtPlotCurve *curveSpeed = new QwtPlotCurve(QString("vehicle speed"));
//    curveSpeed->setSamples(ids, speed);
//    curveSpeed->setTitle("Vehicle speed");
//    curveSpeed->setPen(QColor(Qt::green));
//    curveSpeed->setSymbol(new QwtSymbol(QwtSymbol::Star1));
//    curveSpeed->attach(plot);
//    plot->insertLegend(new QwtLegend());

//    QwtPlotCurve *curveCongestion = new QwtPlotCurve(QString("Congestion Count"));
//    curveCongestion->setSamples(ids, congestion);
//    curveCongestion->setTitle("Congestion Count");
//    curveCongestion->setPen(QColor(Qt::red));
//    curveCongestion->setSymbol(new QwtSymbol(QwtSymbol::VLine));
//    curveCongestion->attach(plot);
//    plot->insertLegend(new QwtLegend());

//    plot->setAxisTitle(QwtPlot::xBottom, QString("Samples"));
//    plot->setAxisScale(QwtPlot::xBottom, 0, ids.size());
//    plot->setAxisScale(QwtPlot::yLeft, qMin(getMin(count), qMin(getMin(speed), getMin(congestion))), qMax(getMax(count), qMax(getMax(speed), getMax(congestion))));

//    plot->resize(600, 400);
//    plot->show();

    QMap<QString, QVector<double>> data;
    data.insert("IDS", ids);
    data.insert("VEHICLE COUNT", count);
    data.insert("VEHICLE SPEED", speed);
    data.insert("VEHICLE CONGESTION", congestion);

    VisualGraphDialog *graph = new VisualGraphDialog(data, this);
//    graph->addWidget(plot);
    graph->show();
}
