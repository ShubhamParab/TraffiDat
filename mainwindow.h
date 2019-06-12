#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QInputDialog>
#include <QPoint>
#include <QDateTime>
#include <QListWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include <QDebug>
#include <QPointer>
#include <algorithm>
#include <QMessageBox>

#include "model.h"
#include "database.h"
#include "addroaddialog.h"
#include "addjunctiondialog.h"
#include "addtrafficlightdialog.h"
#include "dumptestroaddialog.h"
#include "dumptesttrafficlightdialog.h"
#include "structtreemodel.h"
#include "roadwidget.h"
#include "mapmodel.h"
#include "mapgraphicsscene.h"
#include "constants.h"

#include "detection.h"
#include "analyticswizard.h"

// qwt
#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_legend.h>
#include <qwt_symbol.h>

// qwtplot3d
#include "visualgraphdialog.h"

#include "sessionreport.h"

using namespace TraffiDat;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void build(QString filename);
    void roadToWdgt(QList<road> rds, QString junct = QString("JUNCTION NOT SET"));

    QVector<double> getMutRdPlotDat(QList<mut_road> mutRds, rPlot rp);
    QVector<QwtText> getMutRdPlotX(QList<mut_road> mutRds);

    QVector<double> getMutTlPlotDat(QList<mut_traffic_light> mutTls, tlPlot tp);
    QVector<QwtText> getMutTlPlotX(QList<mut_traffic_light> mutTls);

    ~MainWindow();

    void keyReleaseEvent(QKeyEvent *ev) override;

public slots:
    // File
    void onNewFile();
    void onOpenFile();
    void onSaveFile();
    void onExitFile();
    // Edit
    // -Add
    void onTrafficLightAddEdit();
    void onRoadAddEdit();
    void onJunctionAddEdit();
    void onJunctionAddRemove();
    //
    void onEditRoadEdit();
    //
    // View
    void onRoadView();
    void onJunctionView();
    // Play
    void onStartPlay();
    void onPausePlay();
    void onStopPlay();
    void onGotoPlay();
    // Test
    // -Pump
    void onRoadPumpTestClicked();
    void onTrafficLightPumpTestClicked();
    // -Dump
    void onRoadDumpTestClicked();
    void onTrafficLightDumpTestClicked();
    // Visualization
    void onRoadVisual();
    void onTrafficLightVisual();
    // Custom
    void addLog(QString msg, qint64 styleType = 1);   // add log messages
//    void onVideoChange(qint64 duration);
    void finished(qint64 road_id = -1);
    void updateStats();
    void onDump();

private:
    Ui::MainWindow *ui;
    Database *db = nullptr;
    AddRoadDialog *addRoadDialog = nullptr;
    AddJunctionDialog *addJunctionDialog = nullptr;
    AddTrafficLightDialog *addTrafficLightDialog = nullptr;
    DumpTestRoadDialog *dumpTestRoadDialog = nullptr;
    DumpTestTrafficLightDialog *dumpTestTrafficLightDialog = nullptr;
    StructTreeModel *structTreeModel = nullptr;

//    QVideoWidget *videoWidget = nullptr;
    QListWidget *listRoad = nullptr;

    MapModel *mapModel = nullptr;
    MapGraphicsScene *mapScene = nullptr;

    QString fileName;
    qint64 seekTime;
//    QMediaPlayer *players = nullptr;
//    QMediaPlayer *currPlayer = nullptr;
    QMap<qint64, RoadWidget*> roadWdgts;
    QMap<qint64, QListWidgetItem*> rdWdgtItms;

    int playType = STOP_PLAY;
    bool allowStart = true;

    Detection *detect = nullptr;
    QTimer *dumpTimer = nullptr;

    QPointer<AnalyticsWizard> analyt;
    QPointer<QwtPlot> plot = nullptr;
};

#endif // MAINWINDOW_H
