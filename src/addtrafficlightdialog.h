#ifndef ADDTRAFFICLIGHTDIALOG_H
#define ADDTRAFFICLIGHTDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QStringListModel>
#include <QPointer>

#include "model.h"
#include "database.h"

using namespace TraffiDat;
namespace Ui {
class AddTrafficLightDialog;
}

class AddTrafficLightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTrafficLightDialog(Database *db, QWidget *parent = nullptr);
    void build(QList<junction> js, QList<road> rs);
    traffic_light getData();
    void cleanup();
    ~AddTrafficLightDialog();

public slots:
    void onAdd();
    void onCurrentTextChanged(QString value);

private:
    Ui::AddTrafficLightDialog *ui;
    QStringList juncts;
    QStringList rds;
    QStringList selectRds;
    QStringListModel *junctsModel = nullptr;
    QStringListModel *rdsModel = nullptr;
    QStringListModel *selectRdsModel = nullptr;

    QPointer<Database> db;

};

#endif // ADDTRAFFICLIGHTDIALOG_H
