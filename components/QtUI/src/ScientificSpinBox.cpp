#include "ScientificSpinBox.hpp"

#include <limits>
#include <charconv>

#include <QDoubleValidator>
#include <iostream>
namespace QtExt
{
    using namespace Qt;
    ScientificSpinBox::ScientificSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
    {
        this->setDecimals(15);
        this->setMaximum(std::numeric_limits<double>::max());
        this->setMinimum(std::numeric_limits<double>::min());
    };

    QValidator::State ScientificSpinBox::validate(QString &text, int &pos) const {
        QDoubleValidator state{this->minimum(),this->maximum(),this->decimals()};
        return state.validate(text,pos);
    };
}

#include "moc_ScientificSpinBox.cpp"
