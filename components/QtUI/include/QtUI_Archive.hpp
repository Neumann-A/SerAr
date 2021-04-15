#pragma once

#include <stack>
#include <type_traits>

#include <cstdio> // std::puts

#include <QtWidgets>

#include <MyCEL/stdext/std_extensions.h>
#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedEnumVariant.hpp>
#include <SerAr/Core/InputArchive.h>

#include "ScientificSpinBox.hpp"

namespace SerAr
{
    using namespace Archives;
    /*
    namespace QtUI {
        enum class QtUI_WidgetDataType {LineEdit, ComboBox, SpinBox, DoubleSpinBox};
a
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

    enum class QtUI_ItemType : int { NamedItem = 1, StructItem = 2, ValueItem = 3};

    template<typename T>
    class QtUI_StructItem : public QStandardItem
    {
    public:
        NamedValue<T> named_value;

        QtUI_StructItem(NamedValue<T> nv) : QStandardItem(QString::fromStdString(nv.name)), named_value(nv)
        {
        }

        int type() const override
        {
            return (QStandardItem::UserType +  static_cast<int>(QtUI_ItemType::StructItem));
        }
    };

    template<typename T>
    class QtUI_ValueItem : public QStandardItem
    {
    public:
        NamedValue<T> named_value;

        QtUI_ValueItem(NamedValue<T>& nv) : QStandardItem(), named_value(nv)
        {
            if constexpr (stdext::is_string_v<std::remove_cvref_t<T>>)
                QStandardItem::setData(QString::fromStdString(name.val),Qt::EditRole);
            else {
            QStandardItem::setData(named_value.val,Qt::EditRole);
            }
        }

        int type() const override
        {
            return (QStandardItem::UserType +  static_cast<int>(QtUI_ItemType::ValueItem) );
        }

        void setData(const QVariant &value, int role = Qt::UserRole + 1) override {
            if constexpr (stdext::is_string_v<std::remove_cvref_t<T>>)
                named_value.val = value.value<QString>().toStdString();
            else {
                named_value.val = value.value<std::remove_cvref_t<T>>();
            }
            QStandardItem::setData(value, role);
        }
    };

    class QtUI_Archive : public InputArchive<QtUI_Archive> 
    {
    private:
        QTreeView* treeview;
        QStandardItemModel* model;
        std::stack<QStandardItem *> itemstack;
    public:
        QtUI_Archive(QWidget *parent, QLayout * layout); 

        template<typename T> 
        requires (std::is_class_v<std::remove_cvref_t<T>>)
        void load(NamedValue<T>& nv) {
            auto newval = new QtUI_StructItem<T>(nv);

            if(itemstack.empty()) {
                model->appendRow(newval);
            } else {
                auto val = itemstack.top();
                val->appendRow(newval);
            }
            itemstack.push(newval);
            this->operator()(nv.val);
            itemstack.pop();
            if(itemstack.empty()) {
                treeview->resizeColumnToContents(0);
            }
        }

        template<typename T> 
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>>)
        void load(NamedValue<T>& nv) {
            auto name = std::make_unique<QStandardItem>(QString::fromStdString(nv.name));
            name->setEditable(false);
            auto value = std::make_unique<QtUI_ValueItem<T>>(nv);
            QList<QStandardItem *> items;
            items.push_back(name.release());
            items.push_back(value.release());
            if(itemstack.empty()) {
                model->appendRow(items);
            } else {
                auto val = itemstack.top();
                val->appendRow(items);
            }
        }

        void resizeColumns();
    };
}
