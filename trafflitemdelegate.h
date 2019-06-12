#ifndef TRAFFLITEMDELEGATE_H
#define TRAFFLITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QComboBox>
#include <QProgressBar>
#include <QApplication>
#include <QPainter>
#include <QPushButton>
#include <QObject>

#include "model.h"

using namespace TraffiDat;
class TraffLItemDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    TraffLItemDelegate(QMap<qint64, mut_traffic_light*> mutTls, QMap<qint64, QList<traffic_light>*>& rd2analy_tls, QObject* parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

    ~TraffLItemDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

signals:
    void showGraph(qint64 id);

public slots:
    void onClick();

private:
    QMap<qint64, QList<traffic_light>*>* rd2analy_tls;
    QMap<qint64, mut_traffic_light*> mutTls;

    QPushButton* thisButton = nullptr;
};

#endif // TRAFFLITEMDELEGATE_H
