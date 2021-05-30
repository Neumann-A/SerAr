#pragma once

#include <QWidget>
#include <QStyledItemDelegate>

namespace SerAr {
        //https://doc.qt.io/qt-5/qstyleditemdelegate.html
    class QtUI_ArchiveItemDelegate : public QStyledItemDelegate {
        Q_OBJECT
    public:
        QtUI_ArchiveItemDelegate(QObject *parent = nullptr);

        QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const override;

        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model,
                        const QModelIndex &index) const override;

        // void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        //                         const QModelIndex &index) const override;
    };
}
