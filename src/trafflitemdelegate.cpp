#include "trafflitemdelegate.h"


TraffLItemDelegate::TraffLItemDelegate(QMap<qint64, mut_traffic_light*> mutTls, QMap<qint64, QList<traffic_light>*>& rd2analy_tls, QObject *parent):
    QStyledItemDelegate (parent),
    rd2analy_tls(&rd2analy_tls),
    mutTls(mutTls)
{
    this->mutTls.insert(this->mutTls.keys().last()+1, new mut_traffic_light(-1));

    thisButton = new QPushButton;
    thisButton->setText("View In Graph");
    connect(this, SIGNAL(showGraph(qint64)), parent, SLOT(onShowGraph(qint64)));
    connect(thisButton, SIGNAL(clicked()), this, SLOT(onClick()));
}

QWidget *TraffLItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 7){
        QComboBox* editor = new QComboBox(parent);
        for(mut_traffic_light* i : mutTls.values()){
            if(i->traffic_light_id == -1){
                editor->addItem(QString("CLEAR"));
                continue;
            }
            editor->addItem(QString("TRAFFIC ID :%1").arg(i->traffic_light_id));
        }
        return editor;
    }else if(index.column() == 8){
        QPushButton* thisButton = new QPushButton;
        thisButton->setText("View In Graph");
        connect(thisButton, SIGNAL(clicked()), this, SLOT(onClick()));
        thisButton->click();
        return thisButton;
    }
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TraffLItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if(index.column() == 7){
        QString val = index.model()->data(index, Qt::EditRole).toString();
        QComboBox* wdgt = static_cast<QComboBox*>(editor);
        int i = wdgt->findData(val);
        if(i == -1)
            wdgt->setCurrentIndex(0);
        else
            wdgt->setCurrentIndex(i);
    }else if(index.column() == 8){

    }else{
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void TraffLItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    editor->setGeometry(option.rect);
}

void TraffLItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if(index.column() == 7){
        QComboBox* wdgt = static_cast<QComboBox*>(editor);
        model->setData(index, wdgt->currentText());
        QString selectedStr = wdgt->currentText();
        if(!selectedStr.contains("CLEAR")){
            qint64 selectedId = selectedStr.split(":").at(1).toInt();
            qDebug() << "selected ID: " << selectedId;
            traffic_light selectedTl = traffic_light(selectedId);
            if(rd2analy_tls->values().at(index.row())->contains(selectedTl)){
                rd2analy_tls->values().at(index.row())->removeAll(selectedTl);
                qDebug() << "Removing " << selectedId << " updated tls: " << repr_traffLs(*rd2analy_tls->values().at(index.row())) << " at index: " << index.row();
            }else{
                rd2analy_tls->values().at(index.row())->append(selectedTl);
                qDebug() << "Adding " << selectedId << " updated tls : " << repr_traffLs(*rd2analy_tls->values().at(index.row())) << " at index: " <<index.row();
            }
        }else{
            qDebug() << "clearing all TLs";
            rd2analy_tls->values().at(index.row())->clear();
        }
    }else if(index.column() == 8){
    }else{
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

TraffLItemDelegate::~TraffLItemDelegate()
{

}

void TraffLItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if(index.column() == 5){
        qreal density = index.model()->data(index, Qt::DisplayRole).toFloat();
        density *= 100;
        QProgressBar renderer;
        renderer.resize(option.rect.size());
        renderer.setMinimum(0);
        renderer.setMaximum(100);
        renderer.setValue(density);

        painter->save();
        painter->translate(option.rect.topLeft());
        renderer.render(painter);
        painter->restore();
    }else if(index.column() == 8){
        connect(thisButton, SIGNAL(clicked()), this, SLOT(onClick()));
        thisButton->resize(option.rect.size());

        painter->save();
        painter->translate(option.rect.topLeft());
        thisButton->render(painter);
        painter->restore();
    }else{
        QStyledItemDelegate::paint(painter, option, index);
    }
}

void TraffLItemDelegate::onClick()
{
    qDebug() << "SHOWING TL: " << mutTls.first()->traffic_light_id;
    emit showGraph(mutTls.first()->traffic_light_id);
}
