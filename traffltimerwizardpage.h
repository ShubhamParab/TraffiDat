#ifndef TRAFFLTIMERWIZARDPAGE_H
#define TRAFFLTIMERWIZARDPAGE_H

#include <QWizardPage>
#include <QPointer>

#include "model.h"
#include "mapmodel.h"
#include "traffictimertablemodel.h"
#include "traffltimeritemdelegate.h"

namespace Ui {
class TraffLTimerWizardPage;
}

class TraffLTimerWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit TraffLTimerWizardPage(MapModel* model, QWidget *parent = nullptr);

    void initializePage() override;

    void buildMeanMutTls();
    void buildTlTimerRecommendations();
    void build();

    ~TraffLTimerWizardPage();

    bool validatePage() override;

signals:
    void wizInfo(QString msg, qint64 msgType = INFO);

public slots:
    void onTimerChanged();

private:
    Ui::TraffLTimerWizardPage *ui;
    MapModel* model;

    qint64 tl_count = 0;
    qint64 cycle_time = 0;
    qint64 mean_use_count = 0;

    QMap<qint64, traffic_light*> oldTls;
    QMap<qint64, traffic_light*> newTls;
    QMap<qint64, mut_traffic_light> mutTls;
    QMap<qint64, TlCategory> catTls;

    QPointer<TraffLTimerItemDelegate> timerDelegate;

};

#endif // TRAFFLTIMERWIZARDPAGE_H
