#include "IntegerSpinBox.hpp"


namespace QtExt {
    //LongLong
    void LongLongSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    LongLongSpinBox::LongLongSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &LongLongSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &LongLongSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    LongLongSpinBox::~LongLongSpinBox() = default;
    QValidator::State LongLongSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void LongLongSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void LongLongSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value; 
        Q_EMIT valueChanged(new_value);
    }
    LongLongSpinBox::Qt_Type LongLongSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString LongLongSpinBox::textFromValue(LongLongSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled LongLongSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

    //Unsigned LongLong
    void ULongLongSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    ULongLongSpinBox::ULongLongSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &ULongLongSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &ULongLongSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    ULongLongSpinBox::~ULongLongSpinBox() = default;
    QValidator::State ULongLongSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void ULongLongSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void ULongLongSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value;
        Q_EMIT valueChanged(new_value);
    }
    ULongLongSpinBox::Qt_Type ULongLongSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString ULongLongSpinBox::textFromValue(ULongLongSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled ULongLongSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

    //Long
    void LongSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    LongSpinBox::LongSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &LongSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &LongSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    LongSpinBox::~LongSpinBox() = default;
    QValidator::State LongSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void LongSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void LongSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value; 
        Q_EMIT valueChanged(new_value);
    }
    LongSpinBox::Qt_Type LongSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString LongSpinBox::textFromValue(LongSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled LongSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

    //Unsigned Long
    void ULongSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    ULongSpinBox::ULongSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &ULongSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &ULongSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    ULongSpinBox::~ULongSpinBox() = default;
    QValidator::State ULongSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void ULongSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void ULongSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value;
        Q_EMIT valueChanged(new_value);
    }
    ULongSpinBox::Qt_Type ULongSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString ULongSpinBox::textFromValue(ULongSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled ULongSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

    //Short
    void ShortSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    ShortSpinBox::ShortSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &ShortSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &ShortSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    ShortSpinBox::~ShortSpinBox() = default;
    QValidator::State ShortSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void ShortSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void ShortSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value; 
        Q_EMIT valueChanged(new_value);
    }
    ShortSpinBox::Qt_Type ShortSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString ShortSpinBox::textFromValue(ShortSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled ShortSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

    //Unsigned Short
    void UShortSpinBox::onEdit()
    {
        QString input = lineEdit()->text();
        int pos = 0;
        switch(validate(const_cast<QString&>(input), pos)) {
        case QValidator::Acceptable:
            m_value = valueFromText(input);
            break;
        case QValidator::Intermediate:
            break;
        case QValidator::Invalid:
        default:
                lineEdit()->setText(textFromValue(m_value));
        }
    }
    UShortSpinBox::UShortSpinBox(QWidget *parent) : QAbstractSpinBox(parent) {
        // Needed to update the display widget after editing?
        QObject::connect(lineEdit(), &QLineEdit::textEdited, this, &UShortSpinBox::onEdit);
        // This is only needed to update the widget for editing to show the correct value?
        QObject::connect(this, &UShortSpinBox::valueChanged, this, 
            [this](){ this->lineEdit()->setText(this->textFromValue(this->m_value));});
        setAccelerated(true);
    };
    UShortSpinBox::~UShortSpinBox() = default;
    QValidator::State UShortSpinBox::validate(QString &text, int &pos) const {
        QIntergerValidator<T> state{this->m_min,this->m_max,nullptr};
        return state.validate(text,pos);
    };
    void UShortSpinBox::setRange(Qt_Type min, Qt_Type max) {
        m_min = min;
        m_max = max;
    }
    void UShortSpinBox::stepBy(int steps) {
        auto new_value = m_value;
        if (steps < 0 && new_value + steps > new_value) {
            new_value = std::numeric_limits<T>::min();
        }
        else if (steps > 0 && new_value + steps < new_value) {
            new_value = std::numeric_limits<T>::max();
        }
        else {
            new_value += steps;
        }
        m_value = new_value;
        Q_EMIT valueChanged(new_value);
    }
    UShortSpinBox::Qt_Type UShortSpinBox::valueFromText(const QString &text) const {
        return details::qt_string_to_T<T>(locale(),text,nullptr);
    }

    QString UShortSpinBox::textFromValue(UShortSpinBox::Qt_Type val) const {
        return QString::number(val);
    }
    QAbstractSpinBox::StepEnabled UShortSpinBox::stepEnabled() const {
        return StepUpEnabled | StepDownEnabled;
    }

}

#include "moc_IntegerSpinBox.cpp"
