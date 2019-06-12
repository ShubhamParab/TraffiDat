#ifndef MUTROADWIDGET_H
#define MUTROADWIDGET_H

#include <QWidget>
#include <QList>
#include <QTableView>

#include "model.h"
#include "mutroadtablemodel.h"

// qwt
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>

// qwtplot3d
#include "visualgraphdialog.h"

using namespace TraffiDat;
namespace Ui {
class MutRoadWidget;
}

class MutRoadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MutRoadWidget(QWidget *parent = nullptr);
    void setData(road rd, QList<mut_road> mutRd, mut_traffic_light tl);
    road getRd(){return rd;}
    QList<mut_road> getMutRd(){return mutRd;}
    ~MutRoadWidget();

    QVector<double> getMutRdPlotDat(QList<mut_road> mutRds, rPlot rp);
    QVector<QwtText> getMutRdPlotX(QList<mut_road> mutRds);

signals:
    void showWidget(QWidget* widget);

public slots:
    void onViewGraph();

private:
    Ui::MutRoadWidget *ui;
    road rd;
    QList<mut_road> mutRd;
    mut_traffic_light tl;

    QWidget* parent = nullptr;
//    QPointer<QwtPlot> plot = nullptr;

};

#endif // MUTROADWIDGET_H
