#include "visualgraphdialog.h"
#include "ui_visualgraphdialog.h"

using namespace TraffiDat;

VisualGraphDialog::VisualGraphDialog(QMap<QString, QVector<double>> data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VisualGraphDialog),
    data(data)
{
    ui->setupUi(this);

    build();

    connect(ui->comboBox_x_axis, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdate()));
    connect(ui->comboBox_y_axis, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdate()));
    connect(ui->comboBox_z_axis, SIGNAL(currentIndexChanged(int)), this, SLOT(onUpdate()));

}

void VisualGraphDialog::buildGraph(qint64 x_select, qint64 y_select, qint64 z_select)
{

    surface = new Q3DSurface();
    container = QWidget::createWindowContainer(surface);
    ui->fill->addWidget(container);

    QVector<double> x_series = data.values().at(x_select);
    QVector<double> y_series = data.values().at(y_select);
    QVector<double> z_series = data.values().at(z_select);

    QValue3DAxis *x_axis = new QValue3DAxis;
    QValue3DAxis *y_axis = new QValue3DAxis;
    QValue3DAxis *z_axis = new QValue3DAxis;
    x_axis->setTitle(data.keys().at(x_select));
    x_axis->setTitleVisible(true);
    x_axis->setMax(getMax(x_series));
    y_axis->setTitle(data.keys().at(y_select));
    y_axis->setTitleVisible(true);
    y_axis->setMax(getMax(y_series));
    z_axis->setTitle(data.keys().at(z_select));
    z_axis->setTitleVisible(true);
    z_axis->setMax(getMax(z_series));

    surface->setAxisX(x_axis);
    surface->setAxisY(y_axis);
    surface->setAxisZ(z_axis);

    qDebug() << data.keys().at(x_select) << ", " << data.keys().at(y_select) << ", " << data.keys().at(z_select);

    QSurfaceDataArray* dataArray = new QSurfaceDataArray;
    dataArray->clear();
    dataArray->reserve(data.values().first().size());

    for(int j = 0; j < 2; j++){
        QSurfaceDataRow* row = new QSurfaceDataRow;
        for(int i = 0; i < data.first().size(); i++){
            float x = (float)x_series.at(i)+j;
            float y = (float)y_series.at(i)+j;
            float z = (float)z_series.at(i)+j;
            row->append(QSurfaceDataItem(QVector3D(x, y, z)));
        }
        *dataArray << row;
    }

    QSurfaceDataProxy* dataProxy = new QSurfaceDataProxy;
    dataProxy->resetArray(dataArray);

    QSurface3DSeries* series = new QSurface3DSeries;
    series->setDataProxy(dataProxy);
    series->setColorStyle(Q3DTheme::ColorStyleObjectGradient);

    surface->addSeries(series);

}

void VisualGraphDialog::build()
{
    ui->comboBox_x_axis->addItems(data.keys());
    ui->comboBox_y_axis->addItems(data.keys());
    ui->comboBox_z_axis->addItems(data.keys());

    ui->comboBox_x_axis->setCurrentIndex(0);
    ui->comboBox_y_axis->setCurrentIndex(1);
    ui->comboBox_z_axis->setCurrentIndex(2);

    buildGraph(0, 1, 2);
}

float VisualGraphDialog::getMax(QVector<double> data)
{
    float out = 0;
    for(double i : data){
        if(out < i)
            out = i;
    }
    return out;
}

VisualGraphDialog::~VisualGraphDialog()
{
    delete ui;
}

void VisualGraphDialog::onUpdate()
{
    ui->fill->removeWidget(container);
    buildGraph(ui->comboBox_x_axis->currentIndex(), ui->comboBox_y_axis->currentIndex(), ui->comboBox_z_axis->currentIndex());
}

void VisualGraphDialog::addWidget(QWidget *widgt)
{
    widgt->show();
}
