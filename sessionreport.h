#ifndef SESSIONREPORT_H
#define SESSIONREPORT_H

#include <QObject>
#include <QAbstractTableModel>

#include <KDReportsReport>
#include <KDReportsTextElement>
#include <KDReportsHLineElement>
#include <KDReportsAutoTableElement>
#include <KDReportsPreviewDialog>
#include <QtPrintSupport/QPrintDialog>

#include "mapmodel.h"
#include "roaddetailtablemodel.h"

class SessionReport : public QObject
{
    Q_OBJECT
public:
    explicit SessionReport(MapModel* model, QObject *parent = nullptr);
    void build();

signals:

public slots:
    void show();

private:
    MapModel* model;
    KDReports::Report report;
};

#endif // SESSIONREPORT_H
