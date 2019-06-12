#include "addroaddialog.h"
#include "ui_addroaddialog.h"

AddRoadDialog::AddRoadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddRoadDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton_video_addr, SIGNAL(clicked()), this, SLOT(onVideoAddrClicked()));
}

void AddRoadDialog::setData(road value)
{
    ui->lineEdit_name->setText(value.name);
    ui->lineEdit_length->setText(QString::number(value.length));
    ui->lineEdit_lanes->setText(QString::number(value.lanes));
    ui->lineEdit_perc_damage->setText(QString::number(value.perc_damage));
    ui->lineEdit_video_addr->setText(value.video_addr);
}

road AddRoadDialog::getData()
{
    road out;
    out.name = ui->lineEdit_name->text();
    out.length = ui->lineEdit_length->text().toFloat();
    out.lanes = ui->lineEdit_lanes->text().toInt();
    out.perc_damage = ui->lineEdit_perc_damage->text().toFloat();
    out.video_addr = ui->lineEdit_video_addr->text();

    cleanup();
    return out;
}

void AddRoadDialog::cleanup()
{
    ui->lineEdit_name->clear();
    ui->lineEdit_length->clear();
    ui->lineEdit_lanes->clear();
    ui->lineEdit_perc_damage->clear();
    ui->lineEdit_video_addr->clear();
}

AddRoadDialog::~AddRoadDialog()
{
    if(ui != nullptr)
        delete ui;
}

void AddRoadDialog::onVideoAddrClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select Video file"), QDir::currentPath(), tr("Video Files (*.mp4)"));
    if(fileName != ""){
        ui->lineEdit_video_addr->setText(fileName);
    }
}
