#pragma once

#include <stack>
#include <type_traits>

#include <cstdio> // std::puts

#include <QtWidgets>

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

    template<typname T>
    class QtUI_StructItem : public QStandardItem
    {
    public:
        NamedValue<T> named_value;

        QtUI_StructItem(NamedValue<T> nv, QWidget *parent = nullptr) : QStandardItem(this), named_value(nv)
        {
        }

        int type() const 
        {
            return (QStandardItem::UserType + std::type_index(typeid(T)));
        }

        QStandardItem *QStandardItem::clone() const
        {
            return new QtUI_StructItem<T>(named_value,this->parent());
        }
    };

    template<typname T>
    class QtUI_ValueItem : public QStandardItem
    {
    public:
        NamedValue<T>* named_value;

        QtUI_ValueItem(NamedValue<T>* nv, QWidget *parent = nullptr) : QStandardItem(this), named_value(nv)
        {
            this->setData(nv->val,Qt::EditRole);
        }

        int type() const 
        {
            return (QStandardItem::UserType + std::type_index(typeid(T)));
        }
    };

    template<typname T>
    class QtUI_NamedItem : public QStandardItem
    {
    public:
        NamedValue<T> named_value;

        QtUI_NamedItem(NamedValue<T> nv, QWidget *parent = nullptr) : QStandardItem(this), named_value(nv)
        {
            auto name = new QStandardItem(nv.name);
            name->setEditable(false);
            this->setChild(0,0,name);

            auto value = new QtUI_ValueItem(&nv,this);
            this->setChild(0,1,value);
        }

        int type() const 
        {
            return (QStandardItem::UserType + std::type_index(typeid(T)));
        }
    };

    class QtUI_Archive : InputArchive<QtUI_Archive> 
    {
    private:
        QWidget* TreeWidget;
        QStandardItemModel model;
        std::stack<QStandarItem *> itemstack;
    public:
        QtUI_Archive(QWidget *parent) : InputArchive<QtUI_Archive>(this) {
            //Setup header
            auto *headermodel = new QStandardItemModel();
            auto *nameheader = new QStandardItem("Name"); 
            auto *valueheader = new QStandardItem("Value"); 
            model->setColumnCount(2);
            model->setHorizontalHeaderItem(0,nameheader);
            model->setHorizontalHeaderItem(1,valueheader);
            QHeaderView *headerview = new QHeaderView(Qt::Orientation::Horizontal);
            headerview->setModel(headermodel);

            TreeWidget = new QTreeView(headerview); 
            TreeWidget->setModel(model);

            //Setup display delegates
            auto delegate = static_cast<QStyledItemDelegate*>(TreeWidget->itemDelegate());
            auto factory = new QItemEditorFactory();
            QItemEditorCreatorBase *creator = new QStandardItemEditorCreator<QtExt::ScientificSpinBox>();
            factory->registerEditor(QVariant::Double, creator);
            delegate->setItemEditorFactory(factory);

            QObject::connect(model, &QStandardItemModel::itemChanged, this, [=](QStandardItem* item){ std::puts(item->text().toStdString().c_str()); });
        }

        template<typename T> 
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>>)
        QStandardItem * create_QtUI_Item(NamedValue<T>& val) {
            return new QtUI_NamedItem(nv);
        }
        template<typename T> 
        requires (std::is_class_v<std::remove_cvref_t<T>>)
        QStandardItem * create_QtUI_Item(NamedValue<T>& nv) {
            return new QtUI_StructItem(nv);
        }

        template<typename T> 
        requires (std::is_class_v<std::remove_cvref_t<T>> || std::is_arithmetic_v<std::remove_cvref_t<T>>)
        void load(NamedValue<T> nv) {
            auto newval = create_QtUI_Item(nv);
            if(itemstack.empty()) {
                model.appendRow(newval);
            } else {
                auto val = itemstack.top();
                val.appendRow(newval);
            }
            itemstack.push(newval);
            this->operator()(nv.val);
            itemstack.pop();
        }

    };
}