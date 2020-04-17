#ifndef ROAD2TRAFFLWIZARDPAGE_H
#define ROAD2TRAFFLWIZARDPAGE_H

#include <QWizardPage>
#include <QMessageBox>

#include "road2traffltablemodel.h"
#include "trafflitemdelegate.h"
#include "mapmodel.h"

// qwt
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>

// qwtplot3d
#include "visualgraphdialog.h"

using namespace TraffiDat;
namespace Ui {
class Road2TraffLWizardPage;
}

class Road2TraffLWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit Road2TraffLWizardPage(MapModel* model, QWidget *parent = nullptr);
    void buildMeanMutRds();
    void buildPs();
    void buildTlRecommendations();
    void build();
    ~Road2TraffLWizardPage();

    QList<road> getInvalidRoads();

    bool validatePage() override;

    QVector<double> getMutTlPlotDat(QList<mut_traffic_light> mutTls, tlPlot tp);
    QVector<QwtText> getMutTlPlotX(QList<mut_traffic_light> mutTls);

signals:
    void wizInfo(QString msg, qint64 msgType = INFO);

public slots:
    void onShowGraph(qint64 id);

private:
    Ui::Road2TraffLWizardPage *ui;
    MapModel* model;
    QMap<qint64, traffic_light*> final;
    QMap<qint64, traffic_light*> prev;

    qint64 tl_count = 0;
    qint64 tl_update = 0;
};

#endif // ROAD2TRAFFLWIZARDPAGE_H
