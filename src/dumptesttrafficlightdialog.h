#ifndef DUMPTESTTRAFFICLIGHTDIALOG_H
#define DUMPTESTTRAFFICLIGHTDIALOG_H

#include <QDialog>
#include <QStringListModel>
#include <QList>

#include "model.h"

using namespace TraffiDat;
namespace Ui {
class DumpTestTrafficLightDialog;
}

class DumpTestTrafficLightDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DumpTestTrafficLightDialog(QWidget *parent = nullptr);
    void build(QList<traffic_light> tls);
    mut_traffic_light getData();
    void cleanup();
    ~DumpTestTrafficLightDialog();

private:
    Ui::DumpTestTrafficLightDialog *ui;
    QStringList tlsStr;
};

#endif // DUMPTESTTRAFFICLIGHTDIALOG_H
