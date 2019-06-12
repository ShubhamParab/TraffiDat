#ifndef ROADWIDGET_H
#define ROADWIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPointer>

#include "model.h"
#include "Section.h"
#include "constants.h"

using namespace TraffiDat;
namespace Ui {
class RoadWidget;
}

class RoadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RoadWidget(QWidget *parent = nullptr, struct road r = road(), QString junctName = QString("DEFAULT JUNCTION"));
    road getRoad(){return r;}
    QString getJunction(){return junctName;}
    ~RoadWidget();

public slots:
    void setStats(qint64 count, qint64 speed, qint64 signal);

private:
    Ui::RoadWidget *ui;
    Section *sectionInfo = nullptr;
    road r;
    QString junctName;
};

#endif // ROADWIDGET_H
