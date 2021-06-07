#pragma once

#include <stack>
#include <type_traits>

#include <cstdio> // std::puts

#include <QtWidgets>

#include <MyCEL/stdext/std_extensions.h>
#include <SerAr/Core/NamedValue.h>
#include <SerAr/Core/NamedEnumVariant.hpp>
#include <SerAr/Core/InputArchive.h>

#include <fmt/core.h>

namespace SerAr
{
    class QtUI_Archive;

    namespace details {
        template<typename T>
        concept SizeableContainer = stdext::is_container_v<T> && requires(T t) {
            typename T::value_type;
            {t.resize(1)};
            {t.resize(1)};
        };

        template<typename T>
        static constexpr bool is_sizeable_container_v = false;

        template<SizeableContainer T>
        static constexpr bool is_sizeable_container_v<T> = true;
    }
    using namespace Archives;

    enum class QtUI_ItemType : int { NamedItem = 1, StructItem = 2, ValueItem = 3, ContainerItem = 4, SizeableContainerItem = 5, EigenMatrixItem = 6};

    template<typename T>
    class QtUI_ContainerItem : public QStandardItem
    {
        static_assert(stdext::is_container_v<std::remove_cvref_t<T>>);
        static_assert(!details::is_sizeable_container_v<std::remove_cvref_t<T>>);
    public:
        NamedValue<T> named_value;

        QtUI_ContainerItem(NamedValue<T> nv) : QStandardItem(QString::fromStdString(nv.name)), named_value(nv)
        {
        }

        int type() const override
        {
            return (QStandardItem::UserType +  static_cast<int>(QtUI_ItemType::ContainerItem));
        }
    };
    template<typename T>
    class QtUI_SizeableContainerItem : public QStandardItem
    {
        static_assert(stdext::is_container_v<std::remove_cvref_t<T>>);
        static_assert(details::is_sizeable_container_v<std::remove_cvref_t<T>>);
    public:
        NamedValue<T> named_value;

        QtUI_SizeableContainerItem(NamedValue<T> nv) : QStandardItem(QString::fromStdString(nv.name)), named_value(nv)
        {
        }

        int type() const override
        {
            return (QStandardItem::UserType +  static_cast<int>(QtUI_ItemType::SizeableContainerItem));
        }
    };
    template<typename T>
    class QtUI_SizeableContainerItemSize : public QStandardItem
    {
        QtUI_Archive& archive;
        QtUI_SizeableContainerItem<T> * container_item;
        mutable std::size_t container_size;
    public:
        QtUI_SizeableContainerItemSize(QtUI_Archive& ar, QtUI_SizeableContainerItem<T> * item) :  QStandardItem(), archive(ar), container_item(item), container_size(container_item->named_value.val.size())
        {
            QStandardItem::setData(QVariant::fromValue(container_size),Qt::EditRole);
        }
        int type() const override
        {
            return (QStandardItem::UserType +  static_cast<int>(QtUI_ItemType::SizeableContainerItem));
        }
        // QVariant data(int role = Qt::UserRole + 1) const override {
        //     if(role == Qt::DisplayRole || role == Qt::EditRole) {
        //         container_size = container_item->named_value.val.size();
        //         return QVariant::fromValue(container_size);
        //     }
        //     return QStandardItem::data(role);
        // }
        void setData(const QVariant &value, int role = Qt::UserRole + 1) override {
            if(role == Qt::DisplayRole || role == Qt::EditRole) {
                const auto new_size = value.toULongLong();
                if(new_size == container_size)
                    return;
                bool new_smaller = new_size < container_size;
                auto difference = new_smaller ? container_size - new_size : new_size - container_size;
                if(new_smaller) {
                    //WARNING HACK: Even though the container supports size_t here Qt does not support that!
                    container_item->removeRows((int)(container_item->rowCount() - difference),(int)(difference));
                    container_item->named_value.val.resize(new_size);
                } else {
                    
                    container_item->removeRows(0,container_item->rowCount());
                    container_item->named_value.val.resize(new_size);
                    archive.itemstack.push(container_item);
                    std::size_t counter = 0;
                    for(auto& elem : container_item->named_value.val) {
                         archive(NamedValue(fmt::format("[{}]",counter++),elem));
                    }
                    archive.itemstack.pop();
                }
            }
            container_size = container_item->named_value.val.size();
            QStandardItem::setData(value, role);
        }
    };

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
            return (QStandardItem::UserType + static_cast<int>(QtUI_ItemType::StructItem));
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
                QStandardItem::setData(QString::fromStdString(named_value.val),Qt::EditRole);
            else {
                QStandardItem::setData(QVariant::fromValue(named_value.val),Qt::EditRole);
            }
        }

        int type() const override
        {
            return (QStandardItem::UserType + static_cast<int>(QtUI_ItemType::ValueItem) );
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

//     class Qt_EigenMatrixData {
//         Qt_EigenMatrixData_Abstract * data = nullptr;
//     public:
//         enum class DataType { type_double, type_float, type_unsigned_integer, type_signed_integer};
//         Qt_EigenMatrixData() {};
//         ~Qt_EigenMatrixData() {};
//     };
//     class Qt_EigenMatrixData_Abstract {
//     protected:
//         Qt_EigenMatrixData_Abstract() {};
//         virtual ~Qt_EigenMatrixData_Abstract();
//         virtual int getRows() = 0;
//         virtual int getCols() = 0;
//     };
//     template<typename T> requires (stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
//     class Qt_EigenMatrixData_Impl : public Qt_EigenMatrixData_Abstract {
//         Qt_EigenMatrixData_Impl() {
//         }
//     };
// #ifdef EIGEN_CORE_H

//     template<typename T> requires (stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
//     class QtUI_EigenMatrixItem : public QStandardItem
//     {
//     public:
//         NamedValue<T> named_value;

//         QtUI_EigenMatrixItem(NamedValue<T>& nv) : QStandardItem(), named_value(nv)
//         {
//             if constexpr (stdext::is_string_v<std::remove_cvref_t<T>>)
//                 QStandardItem::setData(QString::fromStdString(named_value.val),Qt::EditRole);
//             else {
//                 QStandardItem::setData(QVariant::fromValue(named_value.val),Qt::EditRole);
//             }
//         }
//         int type() const override
//         {
//             return (QStandardItem::UserType + static_cast<int>(QtUI_ItemType::EigenMatrixItem) );
//         }
//         void setData(const QVariant &value, int role = Qt::UserRole + 1) override {
//             if constexpr (stdext::is_string_v<std::remove_cvref_t<T>>)
//                 named_value.val = value.value<QString>().toStdString();
//             else {
//                 named_value.val = value.value<std::remove_cvref_t<T>>();
//             }
//             QStandardItem::setData(value, role);
//         }
//     };
// #endif
    class QtUI_Archive : public InputArchive<QtUI_Archive> 
    {
        using ThisClass = QtUI_Archive;

        template<typename T> friend class QtUI_SizeableContainerItemSize;
    private:
        QTreeView* treeview;
        QStandardItemModel* model;
        std::stack<QStandardItem *> itemstack;
    
        void appendItemToStack(QStandardItem * item) {
            if(itemstack.empty()) {
                model->appendRow(item);
            } else {
                auto val = itemstack.top();
                val->appendRow(item);
            }
        };
        void appendItemToStack(QList<QStandardItem *>& items) {
            if(itemstack.empty()) {
                model->appendRow(items);
            } else {
                auto val = itemstack.top();
                val->appendRow(items);
            }
        };

        void resizeNameColumn() {
            if(itemstack.empty()) {
                treeview->resizeColumnToContents(0);
            }
        }
    public:
        QtUI_Archive(QWidget *parent, QLayout * layout); 

        template<typename T> 
        requires (std::is_class_v<std::remove_cvref_t<T>> 
        && !stdext::is_string_v<std::remove_cvref_t<T>> 
        && !stdext::is_container_v<std::remove_cvref_t<T>>
        && !stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
        ThisClass& load(NamedValue<T>& nv) {
            auto newval = new QtUI_StructItem<T>(nv);
            newval->setEditable(false);
            appendItemToStack(newval);
            itemstack.push(newval);
            this->operator()(nv.val);
            itemstack.pop();
            resizeNameColumn();
            return *this;
        }

        template<typename T> 
        requires (stdext::is_string_v<std::remove_cvref_t<T>>)
        ThisClass& load(NamedValue<T>& nv) {
            auto name = std::make_unique<QStandardItem>(QString::fromStdString(nv.name));
            name->setEditable(false);
            auto value = std::make_unique<QtUI_ValueItem<T>>(nv);
            QList<QStandardItem *> items;
            items.push_back(name.release());
            items.push_back(value.release());
            appendItemToStack(items);
            return *this;
        }

        template<typename T> 
        requires (std::is_arithmetic_v<std::remove_cvref_t<T>> || std::is_same_v<bool,std::remove_cvref_t<T>>)
        ThisClass& load(NamedValue<T>& nv) {
            auto name = std::make_unique<QStandardItem>(QString::fromStdString(nv.name));
            name->setEditable(false);
            auto value = std::make_unique<QtUI_ValueItem<T>>(nv);
            QList<QStandardItem *> items;
            items.push_back(name.release());
            items.push_back(value.release());
            appendItemToStack(items);
            return *this;
        }

        template<typename T>
        requires (stdext::is_container_v<std::remove_cvref_t<T>> && 
                    details::is_sizeable_container_v<std::remove_cvref_t<T>> &&
                    !stdext::is_string_v<std::remove_cvref_t<T>>)
        ThisClass& load(NamedValue<T> &nv) {
            auto name = std::make_unique<QtUI_SizeableContainerItem<T>>(nv);
            name->setEditable(false);
            auto value = std::make_unique<QtUI_SizeableContainerItemSize<T>>(*this,name.get());
            QList<QStandardItem *> items;
            items.push_back(name.release());
            items.push_back(value.release());
            appendItemToStack(items);
            itemstack.push(items[0]);
            std::size_t counter = 0;
            for(auto& elem : nv.val) {
                this->operator()(NamedValue(fmt::format("[{}]",counter++),elem));
            }
            itemstack.pop();
            return *this;
        }

        template<typename T>
        requires (stdext::is_container_v<std::remove_cvref_t<T>> && 
                    !details::is_sizeable_container_v<std::remove_cvref_t<T>> &&
                    !stdext::is_string_v<std::remove_cvref_t<T>>)
        ThisClass& load(NamedValue<T> &nv) {
            auto name = std::make_unique<QtUI_ContainerItem<T>>(nv);
            name->setEditable(false);
            QList<QStandardItem *> items;
            items.push_back(name.release());
            auto value = std::make_unique<QStandardItem>(QString::fromStdString(fmt::format("[size: {}]",nv.val.size())));
            items.push_back(value.release());
            appendItemToStack(items);
            itemstack.push(items[0]);
            std::size_t counter = 0;
            for(auto& elem : nv.val) {
                this->operator()(NamedValue(fmt::format("[{}]",counter++),elem));
            }
            itemstack.pop();
            return *this;
        }

#ifdef EIGEN_CORE_H
        // class QtUI_Archive_EigenDataType {
        // };
        template<typename T> requires(stdext::is_eigen_type_v<std::remove_cvref_t<T>>)
        inline ThisClass& load(NamedValue<T>& nv) {
            auto name = std::make_unique<QtUI_StructItem<T>>(nv);
            name->setEditable(false);
            QList<QStandardItem *> items;
            items.push_back(name.release());
            auto value = std::make_unique<QStandardItem>(QString::fromStdString(fmt::format("[size: {} x {}]",nv.val.rows(),nv.val.cols())));
            value->setEditable(false);
            items.push_back(value.release());
            appendItemToStack(items);
            itemstack.push(items[0]);
            for(std::size_t row = 0; row < nv.val.rows(); row++) {
                for(std::size_t col = 0; col < nv.val.cols(); col++) {
                this->operator()(NamedValue(fmt::format("[{},{}]",row,col),nv.val(row,col)));
            } }
            itemstack.pop();
            return *this;
        }
#endif

    };

    #define QTUI_ARCHIVE_LOAD(type) extern template QtUI_Archive& QtUI_Archive::load<type&> ( NamedValue<type&> & );
    QTUI_ARCHIVE_LOAD(bool)
    QTUI_ARCHIVE_LOAD(short)
    QTUI_ARCHIVE_LOAD(unsigned short)
    QTUI_ARCHIVE_LOAD(int)
    QTUI_ARCHIVE_LOAD(unsigned int)
    QTUI_ARCHIVE_LOAD(long)
    QTUI_ARCHIVE_LOAD(unsigned long)
    QTUI_ARCHIVE_LOAD(long long)
    QTUI_ARCHIVE_LOAD(unsigned long long)
    QTUI_ARCHIVE_LOAD(double)
    QTUI_ARCHIVE_LOAD(float)
    QTUI_ARCHIVE_LOAD(std::string)
    QTUI_ARCHIVE_LOAD(std::vector<short>)
    QTUI_ARCHIVE_LOAD(std::vector<unsigned short>)
    QTUI_ARCHIVE_LOAD(std::vector<int>)
    QTUI_ARCHIVE_LOAD(std::vector<unsigned int>)
    QTUI_ARCHIVE_LOAD(std::vector<long>)
    QTUI_ARCHIVE_LOAD(std::vector<unsigned long>)
    QTUI_ARCHIVE_LOAD(std::vector<long long>)
    QTUI_ARCHIVE_LOAD(std::vector<unsigned long long>)
    QTUI_ARCHIVE_LOAD(std::vector<double>)
    QTUI_ARCHIVE_LOAD(std::vector<float>)
    QTUI_ARCHIVE_LOAD(std::vector<std::string>)
    #undef QTUI_ARCHIVE_LOAD
}

// #ifdef EIGEN_CORE_H
// Q_DECLARE_METATYPE(SerAr::Qt_EigenMatrix);
// #endif
