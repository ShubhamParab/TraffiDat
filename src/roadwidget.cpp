#include "roadwidget.h"
#include "ui_roadwidget.h"

RoadWidget::RoadWidget(QWidget *parent, road r, QString junctName) :
    QWidget(parent),
    ui(new Ui::RoadWidget),
    r(r),
    junctName(junctName)
{
    ui->setupUi(this);
    ui->lineEdit_road->setText(r.name);
    ui->lineEdit_junction->setText(junctName);
    sectionInfo = new Section("Details: ", 300);
    QVBoxLayout *sectionLayout = new QVBoxLayout;
    sectionLayout->addWidget(new QLabel(QString("Length: %1km").arg(r.length)));
    sectionLayout->addWidget(new QLabel(QString("Number of lanes: %1").arg(r.lanes)));
    sectionLayout->addWidget(new QLabel(QString("Damage Percentage: %1").arg(r.perc_damage)));
    sectionInfo->setContentLayout(*sectionLayout);
    ui->gridLayout->replaceWidget(ui->widget_section, sectionInfo);
}

RoadWidget::~RoadWidget()
{
    delete ui;
}

void RoadWidget::setStats(qint64 count, qint64 speed, qint64 signal)
{
    ui->lineEdit_veh_count->setText(QString("%1 cars/min").arg(QString::number(count)));
    ui->lineEdit_veh_speed->setText(QString("%1 km/hr").arg(QString::number(speed)));
    switch (signal) {
    case RED_SIGNAL:
        ui->lineEdit_taff_sign->setStyleSheet("background-color: red;");
        ui->lineEdit_taff_sign->setText("RED SIGNAL");
        break;
    case YELLOW_SIGNAL:
        ui->lineEdit_taff_sign->setStyleSheet("background-color: yellow;");
        ui->lineEdit_taff_sign->setText("YELLOW SIGNAL");
        break;
    case GREEN_SIGNAL:
        ui->lineEdit_taff_sign->setStyleSheet("background-color: green;");
        ui->lineEdit_taff_sign->setText("GREEN SIGNAL");
        break;
    default:
        ui->lineEdit_taff_sign->setStyleSheet("background-color: gray;");
        ui->lineEdit_taff_sign->setText("NO SIGNAL");
        break;
    }
}
