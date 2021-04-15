#include "QtUI_Archive.hpp"

#include <QtWidgets>
#include <memory>
namespace SerAr {

    QtUI_Archive::QtUI_Archive(QWidget *parent, QLayout * layout) : InputArchive<QtUI_Archive>(this) {
            //Layout
            //parent->setLayout(Layout);
            //Layout->setHorizontalSpacing(1);
            //Layout->setVerticalSpacing(1);

            treeview = new QTreeView(parent);
            layout->addWidget(treeview);
            treeview->setWordWrap(true);
            treeview->resize(parent->size());

            //QPushButton *button5 = new QPushButton("test");
            //layout->addWidget(button5,1,0);

            //Model
            model = new QStandardItemModel(treeview);
            treeview->setModel(model);
            model->setColumnCount(2);
            model->setHorizontalHeaderLabels({"Name","Value"});


            //Setup display delegates
            auto delegate = static_cast<QStyledItemDelegate*>(treeview->itemDelegate());
            auto factory = std::make_unique<QItemEditorFactory>();
            QItemEditorCreatorBase *creator = new QStandardItemEditorCreator<QtExt::ScientificSpinBox>();
            factory->registerEditor(QVariant::Double, creator);
            delegate->setItemEditorFactory(factory.release());

            QObject::connect(model, &QStandardItemModel::itemChanged, parent, [=](QStandardItem* item){ std::puts(item->text().toStdString().c_str()); });
        }
    namespace QtUI {
    /*
        QWidget* QtUI_DataItemDelegate::createEditor(QWidget *parent,
                                            const QStyleOptionViewItem & option ,
                                            const QModelIndex & index ) const
        {
            if(!index.data().canConvert<QtUI_DataItem>())
                return QStyledItemDelegate::createEditor(parent,option,index);

            QtUI_DataItem item = qvariant_cast<QtUI_DataItem>(index.data());
            switch(item.WidgetType) {
                case QtUI_WidgetDataType::ComboBox:
                    QComboBox *editor = new QComboBox(parent);
                    editor->setFrame(false);
                    // TODO: Add ComboBox choices; 
                    return editor;
                    break;
                case QtUI_WidgetDataType::SpinBox:
                    QSpinBox *editor = new QSpinBox(parent);
                    editor->setFrame(false);
                    return editor;
                    break;
                case QtUI_WidgetDataType::DoubleSpinBox:
                    QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
                    editor->setFrame(false);
                    return editor;
                    break;
                default:
                    return  QStyledItemDelegate::createEditor(parent,option,index);
            }
        }

        void QtUI_DataItemDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const
        {
            if(!index.data().canConvert<QtUI_DataItem>())
                return QStyledItemDelegate::setEditorData(editor,index);

            QtUI_DataItem item = qvariant_cast<QtUI_DataItem>(index.data());
            switch(item.WidgetType) {
                case QtUI_WidgetDataType::ComboBox:
                    // To do figure out what to do
                    break;
                case QtUI_WidgetDataType::SpinBox:
                    auto value = index.model()->data(index, Qt::EditRole).toLongLong();
                    QSpinBox *spinBox = static_cast<QSpinBox*>(editor);
                    spinBox->setValue(value);
                    break;
                case QtUI_WidgetDataType::DoubleSpinBox:
                    auto value = index.model()->data(index, Qt::EditRole).toDouble();
                    QDoubleSpinBox *doublespinBox = static_cast<QDoubleSpinBox*>(editor);
                    doublespinBox->setValue(value);
                    break;
                default:
                    return  QStyledItemDelegate::setEditorData(editor,index);
            }
        }
    */
    }
}
