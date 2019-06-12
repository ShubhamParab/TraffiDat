#ifndef ADDROADDIALOG_H
#define ADDROADDIALOG_H

#include <QDialog>
#include <QFileDialog>

#include "model.h"


using namespace TraffiDat;
namespace Ui {
class AddRoadDialog;
}

class AddRoadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddRoadDialog(QWidget *parent = nullptr);
    void setData(road value);
    road getData();
    void cleanup();
    ~AddRoadDialog();

public slots:
    void onVideoAddrClicked();

private:
    Ui::AddRoadDialog *ui;
};

#endif // ADDROADDIALOG_H
