#include "Qt_TestApplication.hpp"

#include <SerAr/Core/NamedValue.h>
#include <SerAr/QtUI/QtUI_Archive.hpp>

#include <SerAr/JSON/JSON_Archives.hpp>



template<SerAr::IsArchive Archive>
void serialize(serar_struct_1_numbers& val, Archive& ar) {
    ar(Archives::createNamedValue("mybool",val.mybool));
    ar(Archives::createNamedValue("mydouble",val.mydouble));
    ar(Archives::createNamedValue("myvector",val.myfloat));
    ar(Archives::createNamedValue("myuint",val.myuint));
    ar(Archives::createNamedValue("myint",val.myint));
    ar(Archives::createNamedValue("myull",val.myull));
    ar(Archives::createNamedValue("myll",val.myll));
};

Qt_TestApplication::~Qt_TestApplication() = default;
Qt_TestApplication::Qt_TestApplication(int argc, char *argv[]) : QApplication(argc,argv), window() {
    window.resize(480,360);
    auto WindowLayout = new QVBoxLayout();
    window.setLayout(WindowLayout);

    SerAr::QtUI_Archive ar(&window, WindowLayout);
    //QWidget * ViewWidget = new QWidget(&window);
    //WindowLayout->addWidget(ViewWidget);

    auto ButtonLayout = new QHBoxLayout();
    WindowLayout->addLayout(ButtonLayout);
    auto Status = new QStatusBar(&window);
    WindowLayout->addWidget(Status);

    QPushButton* SaveButton = new QPushButton("Save as JSON", &window);
    ButtonLayout->addWidget(SaveButton);
    SaveButton->setFixedSize(120,50);
    ButtonLayout->insertStretch(-1,1);


    ar(Archives::createNamedValue("Number_Struct",test_struct1));

    QObject::connect(SaveButton, &QPushButton::clicked, this, [this](){ 
        SerAr::JSON_OutputArchive json_archive({}, "QtUI_test.json");
        json_archive(Archives::createNamedValue("Number_Struct",this->test_struct1));
    });

    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "Top-level widget"));
}
