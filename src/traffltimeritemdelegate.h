#ifndef TRAFFLTIMERITEMDELEGATE_H
#define TRAFFLTIMERITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QLabel>

#include "mapmodel.h"

class TraffLTimerItemDelegate: public QStyledItemDelegate
{
    Q_OBJECT
public:
    TraffLTimerItemDelegate(QMap<qint64, qint64*>& timerTls, QMap<qint64, TlCategory> catTls, QObject* parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    ~TraffLTimerItemDelegate();


private:
    QMap<qint64, TlCategory> catTls;
    QMap<qint64, qint64*>* timerTls;

};

#endif // TRAFFLTIMERITEMDELEGATE_H
