#include <SerAr/QtUI/QtUI_Archive.hpp>

#include "Qt_TestApplication.hpp"
#include <QtWidgets>
#include <memory>

// https://doc.qt.io/qt-5/qstackedlayout.html
// https://doc.qt.io/qt-5/qformlayout.html
// https://doc.qt.io/qt-5/qgridlayout.html
// https://doc.qt.io/qt-5/qtreeview.html

// https://doc.qt.io/qt-5/qtwidgets-itemviews-simplewidgetmapper-example.html

//https://doc.qt.io/qt-5/qitemeditorfactory.html#registerEditor
//https://doc.qt.io/qt-5/qstandarditemeditorcreator.html


// const QItemEditorFactory *QItemEditorFactory::defaultFactory()
// void QItemEditorFactory::registerEditor(int userType, QItemEditorCreatorBase *creator)
// setItemEditorFactory()
int main(int argc, char *argv[])
{
    Qt_TestApplication app(argc,argv);

    return app.exec();
}
