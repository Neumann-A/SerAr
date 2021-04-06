#pragma once

#include "SerAr/QtUI/ScientificSpinBox.hpp"

#include <QtWidgets>

#include <cstdio>

class Qt_TestApplication : public QApplication {
private:
    QWidget window;
public:
    Qt_TestApplication(int argc, char *argv[]) : QApplication(argc,argv), window() {

    auto LeftLabel = std::make_unique<QLabel>(QApplication::translate("leftlabel", "Left-Side"));

    auto Layout = std::make_unique<QHBoxLayout>();

    auto *model = new QStandardItemModel();
    auto *item1 = new QStandardItem("item1"); 
    auto *item2 = new QStandardItem("item2"); 
    auto *item3 = new QStandardItem("item3"); 
    auto *item4 = new QStandardItem("item4"); 

    auto *subitem1 = new QStandardItem("subitem1"); 
    auto *subitem2 = new QStandardItem("subitem2"); 

    auto *subsubitem1 = new QStandardItem("subsubitem1"); 
    auto *subsubitemcolumn0 = new QStandardItem("subsubitemcolumn0"); 
    auto *subsubitemcolumn1 = new QStandardItem("subsubitemcolumn1"); 
    subsubitem1 ->setEditable(false);
    subsubitemcolumn0->setEditable(false);
    subsubitem1->setChild(0,0,subsubitemcolumn0);
    subsubitem1->setChild(0,1,subsubitemcolumn1);

    auto *subsubitem1column0 = new QStandardItem("subsubitem1column0"); 
    auto *subsubitem1column1 = new QStandardItem("subsubitem1column1"); 
    auto *subsubitem2column0 = new QStandardItem("subsubitem2column0"); 
    auto *subsubitem2column1 = new QStandardItem("subsubitem2column1"); 
    auto *subsubitem3column0 = new QStandardItem("subsubitem3column0"); 
    auto *subsubitem3column1 = new QStandardItem("subsubitem3column1"); 
    subsubitem1->setChild(1,0,subsubitem1column0);
    subsubitem1->setChild(1,1,subsubitem1column1);
    subsubitem1column1->setData(15,Qt::EditRole);
    subsubitem1->setChild(2,0,subsubitem2column0);
    subsubitem1->setChild(2,1,subsubitem2column1);
    subsubitem2column1->setData(15.1258,Qt::EditRole);
    subsubitem1->setChild(3,0,subsubitem3column0);
    subsubitem1->setChild(3,1,subsubitem3column1);
    subsubitem3column1->setData(false,Qt::EditRole);

    subitem1->appendRow(subsubitem1);
    item1->appendRow(subitem1);
    item1->appendRow(subitem2);

    model->appendRow(item1);
    model->appendRow(item2);
    model->appendRow(item3);
    model->appendRow(item4);



    QObject::connect(model, &QStandardItemModel::itemChanged, this, [=](QStandardItem* item){ std::puts(item->text().toStdString().c_str()); });

    QComboBox *cb = new QComboBox;


    auto *headermodel = new QStandardItemModel();
    auto *header1 = new QStandardItem("Element"); 
    auto *header2 = new QStandardItem("Value"); 
    QList<QStandardItem*> list;
    list.push_back(header1);
    list.push_back(header2);
    model->setColumnCount(2);
    model->setHorizontalHeaderItem(0,header1);
    model->setHorizontalHeaderItem(1,header2);

    QHeaderView *headerview = new QHeaderView(Qt::Orientation::Horizontal);
    headerview->setModel(headermodel);

    QTreeView *tree = new QTreeView(headerview);
    tree->setModel(model);
    auto delegate = static_cast<QStyledItemDelegate*>(tree->itemDelegate());
    auto factory = new QItemEditorFactory();
    QItemEditorCreatorBase *creator = new QStandardItemEditorCreator<QtExt::ScientificSpinBox>();
    factory->registerEditor(QVariant::Double, creator);
    delegate->setItemEditorFactory(factory);
    Layout->addWidget(tree);
    Layout->addWidget(LeftLabel.release());

    window.setLayout(Layout.release());
    window.resize(320,240);
    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "Top-level widget"));



    };

    void SetupUI()
    {


    }
};
