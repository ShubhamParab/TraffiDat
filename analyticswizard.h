#ifndef ANALYTICSWIZARD_H
#define ANALYTICSWIZARD_H

#include <QWizard>
#include <QPointer>

#include "mapmodel.h"
#include "statswizardpage.h"
#include "road2trafflwizardpage.h"
#include "traffltimerwizardpage.h"

using namespace TraffiDat;

class AnalyticsWizard : public QWizard
{
    Q_OBJECT

public:
    explicit AnalyticsWizard(MapModel* model, QWidget *parent = nullptr);
    ~AnalyticsWizard();

    QPointer<MapModel> model;

    QPointer<StatsWizardPage> stats;
    QPointer<Road2TraffLWizardPage> rd2TraffL;
    QPointer<TraffLTimerWizardPage> traffLTimer;

};

#endif // ANALYTICSWIZARD_H
