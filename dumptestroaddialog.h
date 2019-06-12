#ifndef DUMPTESTROADDIALOG_H
#define DUMPTESTROADDIALOG_H

#include <QDialog>
#include <QList>
#include <QStringListModel>

#include "model.h"

using namespace TraffiDat;
namespace Ui {
class DumpTestRoadDialog;
}

class DumpTestRoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DumpTestRoadDialog(QWidget *parent = nullptr);
    void build(QList<road> rds);
    mut_road getData();
    void cleanup();
    ~DumpTestRoadDialog();

private:
    Ui::DumpTestRoadDialog *ui;
    QStringList rds;
};

#endif // DUMPTESTROADDIALOG_H
