#pragma once

#include <QtWidgets>

namespace SerAr
{
    /*
    namespace QtUI {
        enum class QtUI_WidgetDataType {LineEdit, ComboBox, SpinBox, DoubleSpinBox};

        class QtUI_DataItem : public QStandardItem {
        public:
            QtUI_DataItem(const QString &text, QtUI_WidgetDataType type) : QStandardItem(text), WidgetType(type) {};
            QtUI_WidgetDataType WidgetType {QtUI_WidgetDataType::LineEdit};
        };

        Q_DECLARE_METATYPE(QtUI_DataItem);

        class QtUI_DataItemDelegate : public QStyledItemDelegate {
            Q_OBJECT

        public:
            QtUI_DataItemDelegate(QObject *parent = nullptr);

            QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const override;

            void setEditorData(QWidget *editor, const QModelIndex &index) const override;
            void setModelData(QWidget *editor, QAbstractItemModel *model,
                            const QModelIndex &index) const override;

            // void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
            //                         const QModelIndex &index) const override;
        };
    } */
}