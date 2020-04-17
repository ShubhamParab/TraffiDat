#include "analyticswizard.h"

AnalyticsWizard::AnalyticsWizard(MapModel* model, QWidget *parent) :
    QWizard(parent),
    model(model)
{
    stats = new StatsWizardPage(model, this);
    connect(stats, SIGNAL(wizInfo(QString, qint64)), parent, SLOT(addLog(QString, qint64)));
    addPage(stats);
    rd2TraffL = new Road2TraffLWizardPage(model, this);
    connect(rd2TraffL, SIGNAL(wizInfo(QString, qint64)), parent, SLOT(addLog(QString, qint64)));
    addPage(rd2TraffL);
    traffLTimer = new TraffLTimerWizardPage(model, this);
    connect(traffLTimer, SIGNAL(wizInfo(QString, qint64)), parent, SLOT(addLog(QString, qint64)));
    addPage(traffLTimer);

    setMinimumSize(1024, 840);
}

AnalyticsWizard::~AnalyticsWizard()
{

}
