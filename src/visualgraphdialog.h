#ifndef VISUALGRAPHDIALOG_H
#define VISUALGRAPHDIALOG_H

#include <QDialog>
#include <QMap>
#include <QPointer>
#include <QDebug>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include <QScreen>

#include "model.h"

namespace Ui {
class VisualGraphDialog;
}

namespace TraffiDat{
using namespace QtDataVisualization;
class VisualGraphDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VisualGraphDialog(QMap<QString, QVector<double>> data, QWidget *parent = nullptr);
    void buildGraph(qint64 x_select = 0, qint64 y_select = 1, qint64 z_select = 2);
    void build();
    float getMax(QVector<double> data);
    ~VisualGraphDialog();

public slots:
    void onUpdate();
    void addWidget(QWidget* widgt);

private:
    Ui::VisualGraphDialog *ui;
    QMap<QString, QVector<double>> data;

    Q3DSurface* surface;
    QWidget* container;
};

}
#endif // VISUALGRAPHDIALOG_H
