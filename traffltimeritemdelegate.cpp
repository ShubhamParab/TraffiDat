#include "traffltimeritemdelegate.h"


TraffLTimerItemDelegate::TraffLTimerItemDelegate(QMap<qint64, qint64*> &timerTls, QMap<qint64, TlCategory> catTls, QObject *parent):
    QStyledItemDelegate (parent),
    catTls(catTls),
    timerTls(&timerTls)
{

}

QWidget *TraffLTimerItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 5){
        QSpinBox* wdgt = new QSpinBox(parent);
        wdgt->setSingleStep(10);
        if(catTls.values().at(index.row()) == UNAVAILABLE_TL){
            wdgt->setDisabled(true);
        }
        return wdgt;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TraffLTimerItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 5){
        qint64 value = index.model()->data(index, Qt::DisplayRole).toInt();
        QSpinBox* wdgt = static_cast<QSpinBox*>(editor);
        wdgt->setValue(value);
    }else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void TraffLTimerItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void TraffLTimerItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 5){
        QSpinBox* wdgt = static_cast<QSpinBox*>(editor);
        model->setData(index, wdgt->value());
        qint64 value = wdgt->value();
        *timerTls->values().at(index.row()) = value;
    }else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void TraffLTimerItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 3){
        qint64 use_count = index.model()->data(index, Qt::DisplayRole).toInt();
        QLabel label;
        label.setText(QString("%1").arg(use_count));
        label.setAlignment(Qt::AlignCenter);
        if(use_count < 0){
            label.setStyleSheet(QString("color : red; font-size: 22px;"));
        }else{
            label.setStyleSheet(QString("color : green; font-size: 22px;"));
        }

        painter->save();
        painter->translate(option.rect.topLeft());
        label.render(painter);
        painter->restore();
    }else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

TraffLTimerItemDelegate::~TraffLTimerItemDelegate()
{

}
