#ifndef ADDJUNCTIONDIALOG_H
#define ADDJUNCTIONDIALOG_H

#include <QDialog>
#include <QList>
#include <QStringListModel>
#include <QDebug>

#include "model.h"
#include "database.h"

using namespace TraffiDat;
namespace Ui {
class AddJunctionDialog;
}

class AddJunctionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddJunctionDialog(QWidget *parent = nullptr);
    void build(QList<road> roads);
    junction getData();
    void cleanup();
    ~AddJunctionDialog();

public slots:
    void onUpClicked();
    void onDownClicked();

private:
    Ui::AddJunctionDialog *ui;
    QStringListModel *roadModel = nullptr;
    QStringListModel *upRoadModel = nullptr;
    QStringListModel *downRoadModel = nullptr;
    QStringList up_roads;
    QStringList down_roads;
    QStringList roadStr;
};

#endif // ADDJUNCTIONDIALOG_H
