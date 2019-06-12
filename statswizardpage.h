#ifndef STATSWIZARDPAGE_H
#define STATSWIZARDPAGE_H

#include <QWizardPage>

#include "mapmodel.h"
#include "mutroadwidget.h"

using namespace TraffiDat;
namespace Ui {
class StatsWizardPage;
}

class StatsWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    explicit StatsWizardPage(MapModel* model, QWidget *parent = nullptr);
    void build();
    ~StatsWizardPage();

signals:
    void wizInfo(QString msg, qint64 msgType = INFO);

public slots:
    void onShowWidget(QWidget* widget);

private:
    Ui::StatsWizardPage *ui;
    MapModel* model;

    QWidget* parent = nullptr;
};

#endif // STATSWIZARDPAGE_H
