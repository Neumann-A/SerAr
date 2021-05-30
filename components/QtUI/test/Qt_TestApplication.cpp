#include "Qt_TestApplication.hpp"

#include <SerAr/Core/NamedValue.h>
#include <SerAr/QtUI/QtUI_Archive.hpp>

#include <SerAr/JSON/JSON_Archives.hpp>

template<SerAr::IsArchive Archive>
void serialize(serar_struct_1_numbers& val, Archive& ar) {
    ar(Archives::createNamedValue("mybool",val.mybool));
    ar(Archives::createNamedValue("mydouble",val.mydouble));
    ar(Archives::createNamedValue("myfloat",val.myfloat));
    ar(Archives::createNamedValue("myushort",val.myushort));
    ar(Archives::createNamedValue("myshort",val.myshort));
    ar(Archives::createNamedValue("myuint",val.myuint));
    ar(Archives::createNamedValue("myint",val.myint));
    ar(Archives::createNamedValue("myul",val.myul));
    ar(Archives::createNamedValue("myl",val.myl));
    ar(Archives::createNamedValue("myull",val.myull));
    ar(Archives::createNamedValue("myll",val.myll));
};

template<SerAr::IsArchive Archive>
void serialize(serar_struct_1_strings& val, Archive& ar) {
    //ar(Archives::createNamedValue("myconstcharstart",val.mychars));
    ar(Archives::createNamedValue("myuchar",val.myuchar));
    ar(Archives::createNamedValue("myschar",val.myschar));
    ar(Archives::createNamedValue("mychar",val.mychar));
    ar(Archives::createNamedValue("mystring",val.mystring));
};

template<SerAr::IsArchive Archive>
void serialize(serar_test_struct& val, Archive& ar) {
    ar(Archives::createNamedValue("numbers",val.numbers));
    ar(Archives::createNamedValue("string",val.strings));
    ar(Archives::createNamedValue("containers",val.containers));
    ar(Archives::createNamedValue("eigen3",val.eigen3));
};

template<SerAr::IsArchive Archive>
void serialize(serar_struct_1_containers_simple& val, Archive& ar) {
    ar(Archives::createNamedValue("int_array",val.myarray1));
    ar(Archives::createNamedValue("string_array",val.myarray2));
    ar(Archives::createNamedValue("long_vector",val.myvector1));
    ar(Archives::createNamedValue("string_vector",val.myvector2));
};

template<SerAr::IsArchive Archive>
void serialize(serar_test_eigen3& val, Archive& ar) {
    ar(Archives::createNamedValue("double_matrix",val.my_double_matrix));
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


    ar(Archives::createNamedValue("Test_Struct",test_struct1));

    QObject::connect(SaveButton, &QPushButton::clicked, this, [this](){ 
        SerAr::JSON_OutputArchive json_archive({}, "QtUI_test.json");
        json_archive(Archives::createNamedValue("Test_Struct",this->test_struct1));
    });

    window.show();
    window.setWindowTitle(QApplication::translate("toplevel", "Top-level widget"));
}
