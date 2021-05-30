#pragma once

#include <cstdio>

#include <string>
#include <string_view>

#include <array>
#include <vector>

#include <type_traits>

#include <QtWidgets>
#include <Eigen/Core>

struct serar_struct_1_numbers {
    bool mybool { false};
    double mydouble {5.35116151};
    float  myfloat {-3425.14141f};
    unsigned short myushort {3};
    short myshort {-4};
    unsigned int myuint {9};
    int myint {-3};
    unsigned long myul {92525562};
    long myl {-325748};
    unsigned long long myull {3252562652562};
    long long myll {-325498748};
};

struct serar_struct_1_strings {
    unsigned char       myuchar {'a'};
    signed char         myschar{'b'};
    char                mychar{'c'};
    std::string         mystring {"World"};
};

struct serar_struct_1_containers_simple {
    std::array<int,5> myarray1 { 5 ,3,-12,-520,4548};
    std::array<std::string,3> myarray2 { "Hello", "World", "!"};
    std::vector<long> myvector1 { 15815, 5151 ,36121 ,-561156, -541, -51102051 };
    std::vector<std::string> myvector2 { "Bla", "Blub", "!" };
};

struct serar_test_eigen3 {
    Eigen::Matrix<double,2,3> my_double_matrix {Eigen::Matrix<double,2,3>::Constant(6.25)};
};

struct serar_test_struct {
    serar_struct_1_numbers           numbers;
    serar_struct_1_strings           strings;
    serar_struct_1_containers_simple containers;
    serar_test_eigen3                eigen3;
};






class Qt_TestApplication : public QApplication {
private:
    QWidget window;
    serar_test_struct test_struct1;
public:
    Qt_TestApplication(int argc, char *argv[]);
    ~Qt_TestApplication() override;
    void SetupUI();
};
