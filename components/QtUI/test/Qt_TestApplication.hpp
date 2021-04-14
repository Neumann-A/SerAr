#pragma once

#include "SerAr/QtUI/ScientificSpinBox.hpp"

#include <QtWidgets>

#include <cstdio>

class Qt_TestApplication : public QApplication {
private:
    QWidget window;
public:
    Qt_TestApplication(int argc, char *argv[]);
    ~Qt_TestApplication();
    void SetupUI();
};
