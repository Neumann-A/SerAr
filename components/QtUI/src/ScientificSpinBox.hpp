#pragma once

#include <QDoubleSpinBox>

//#include "ScientificSpinBox.moc"

namespace QtExt
{
    class ScientificSpinBox : public QDoubleSpinBox
    {
        Q_OBJECT
    public:
        ScientificSpinBox(QWidget *parent = nullptr);
        ~ScientificSpinBox() override = default;
        QValidator::State validate(QString &text, int &pos) const override;

    };
}
