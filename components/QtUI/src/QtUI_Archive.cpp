#include "QtUI_Archive.hpp"
#include "QtUI_Archive_p.hpp"
#include "ScientificSpinBox.hpp"
#include "IntegerSpinBox.hpp"
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
        treeview->header()->setSectionResizeMode(0,QHeaderView::ResizeToContents);

        //Setup display delegates
        auto delegate = static_cast<QStyledItemDelegate*>(treeview->itemDelegate());
        auto factory = std::make_unique<QItemEditorFactory>();
        QItemEditorCreatorBase *creator = nullptr;
        creator = new QStandardItemEditorCreator<QtExt::ScientificSpinBox>();
        factory->registerEditor(QVariant::Double, creator);
        creator = new QStandardItemEditorCreator<QtExt::LongLongSpinBox>();
        factory->registerEditor(QVariant::LongLong, creator);
        creator = new QStandardItemEditorCreator<QtExt::ULongLongSpinBox>();
        factory->registerEditor(QVariant::ULongLong, creator);
        creator = new QStandardItemEditorCreator<QtExt::LongSpinBox>();
        factory->registerEditor(QMetaType::Long, creator);
        creator = new QStandardItemEditorCreator<QtExt::ULongSpinBox>();
        factory->registerEditor(QMetaType::ULong, creator);
        creator = new QStandardItemEditorCreator<QtExt::ShortSpinBox>();
        factory->registerEditor(QMetaType::Short, creator);
        creator = new QStandardItemEditorCreator<QtExt::UShortSpinBox>();
        factory->registerEditor(QMetaType::UShort, creator);
        creator = new QStandardItemEditorCreator<QtExt::UShortSpinBox>();
        factory->registerEditor(QMetaType::UShort, creator);

        delegate->setItemEditorFactory(factory.release());

        QObject::connect(model, &QStandardItemModel::itemChanged, parent, [=](QStandardItem* item){ std::puts(item->text().toStdString().c_str()); });
        //QObject::connect(treeview, &QTreeView::expanded, parent, [=](const QModelIndex &index){ treeview->resizeColumnToContents(0); });
        //QObject::connect(treeview, &QTreeView::collapsed, parent, [=](const QModelIndex &index){ treeview->resizeColumnToContents(0); });
    }

    #define QTUI_ARCHIVE_LOAD(type) template QtUI_Archive& QtUI_Archive::load< type &>(NamedValue< type &> &);
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

