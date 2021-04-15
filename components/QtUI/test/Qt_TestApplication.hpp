#pragma once


#include <QtWidgets>

#include <cstdio>

struct serar_struct_1_numbers {
    bool mybool { false};
    double mydouble {5.35116151};
    float  myfloat {-3425.14141};
    unsigned int myuint {9};
    int myint {-3};
    unsigned long long myull {3252562652562};
    long long myll {-325498748};
};

class Qt_TestApplication : public QApplication {
private:
    QWidget window;
    serar_struct_1_numbers test_struct1;
public:
    Qt_TestApplication(int argc, char *argv[]);
    ~Qt_TestApplication();
    void SetupUI();
};
