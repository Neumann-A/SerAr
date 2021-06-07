#include "QtUI_Archive.hpp"
#include "QtUI_Archive_p.hpp"

namespace SerAr {
        QWidget* QtUI_ArchiveItemDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem & option ,
                                            const QModelIndex & index ) const
        {
            return  QStyledItemDelegate::createEditor(parent,option,index);
        }

        void QtUI_ArchiveItemDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const
        {
            return  QStyledItemDelegate::setEditorData(editor,index);
        }
        void QtUI_ArchiveItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                        const QModelIndex &index) const
        {
            return QStyledItemDelegate::setModelData(editor,model,index);
        }
}

#include "moc_QtUI_Archive_p.cpp"
