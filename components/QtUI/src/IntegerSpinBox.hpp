#pragma once

#include <QAbstractSpinBox>
#include <QLineEdit>
#include <QIntValidator>
#include <QLocale>
#include <type_traits>
#include <limits>
namespace QtExt
{
    namespace details {
        template<typename T>
        auto qt_string_to_T(const QLocale& local, const QString& value, bool * ok = nullptr) {
            if constexpr (std::is_same_v<T,long>) {
                return local.toLong(value,ok);
            }
            else if constexpr (std::is_same_v<T,unsigned long>){
                return local.toULong(value,ok);
            }
            else if constexpr (std::is_same_v<T,long long>) {
                return local.toLongLong(value,ok);
            }
            else if constexpr (std::is_same_v<T,unsigned long long>){
                return local.toULongLong(value,ok);
            }
            else if constexpr (std::is_same_v<T,int>) {
                return local.toInt(value,ok);
            }
            else if constexpr (std::is_same_v<T,unsigned int>){
                return local.toUInt(value,ok);
            }
            else if constexpr (std::is_same_v<T,short>) {
                return local.toShort(value,ok);
            }
            else if constexpr (std::is_same_v<T,unsigned short>){
                return local.toUShort(value,ok);
            }
            else {
                static_assert(!std::is_same_v<T,T>, "Type is unsupported! __FUNCSIG__ ");
                return 0;
            }
        }
    }
    template<typename T>
    requires (std::is_integral_v<T>)
    class QIntergerValidator : public QValidator
    {
    private:
        
        T m_min{std::numeric_limits<T>::min()};
        T m_max{std::numeric_limits<T>::max()};
    public:
        QIntergerValidator(T min, T max, QObject* parent) : QValidator(parent), m_min(min), m_max(max) { }
        void fixup(QString &input) const override {
            bool ok;
            auto entered = details::qt_string_to_T<T>(locale(),input,&ok);
            if (ok)
                input = locale().toString(entered);
        }
        QValidator::State validate(QString &input, int &pos) const override {
            if (input.isEmpty())
                return Intermediate;

            const bool startsWithMinus(*input.begin() == '-');
            if (m_min >= 0 && startsWithMinus) {
                return Invalid;
            }

            const bool startsWithPlus(*input.begin() == '+');
            if (m_max < 0 && startsWithPlus) {
                return Invalid;
            }

            if (input.size() == 1 && (startsWithPlus || startsWithMinus)) {
                return Intermediate;
            }

            bool ok;
            auto entered = details::qt_string_to_T<T>(locale(),input,&ok); 
            if (!ok) {
                return Invalid;
            }

            if (entered >= m_min && entered <= m_max) {
                return Acceptable;
            }

            if (entered >= 0) {
                // the -entered < b condition is necessary to allow people to type
                // the minus last (e.g. for right-to-left languages)
                // The buffLength > tLength condition validates values consisting
                // of a number of digits equal to or less than the max value as intermediate.

                auto buffLength = static_cast<T>(input.size());
                if (startsWithPlus)
                    buffLength--;
                const auto tLength = m_max != 0 ? static_cast<T>(std::log10(qAbs(m_min))) + 1 : 1;

                return (entered > m_max && -entered < m_min && buffLength > tLength) ? Invalid : Intermediate;
            } else {
                return (entered < m_min) ? Invalid : Intermediate;
            }
        }
    };

    Q_WIDGETS_EXPORT class LongLongSpinBox : public QAbstractSpinBox
    {
        using T = long long;
        using Qt_Type = qlonglong;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(qlonglong minimum MEMBER m_min)
        Q_PROPERTY(qlonglong maximum MEMBER m_max)
        Q_PROPERTY(qlonglong value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(qlonglong val);
    public:
        LongLongSpinBox(QWidget *parent = nullptr);
        ~LongLongSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };

    Q_WIDGETS_EXPORT class ULongLongSpinBox : public QAbstractSpinBox
    {
        using T = unsigned long long;
        using Qt_Type = qulonglong;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(qulonglong minimum MEMBER m_min)
        Q_PROPERTY(qulonglong maximum MEMBER m_max)
        Q_PROPERTY(qulonglong value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(qulonglong val);
    public:
        ULongLongSpinBox(QWidget *parent = nullptr);
        ~ULongLongSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };

    Q_WIDGETS_EXPORT class LongSpinBox : public QAbstractSpinBox
    {
        using T = long;
        using Qt_Type = long;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(long minimum MEMBER m_min)
        Q_PROPERTY(long maximum MEMBER m_max)
        Q_PROPERTY(long value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(long val);
    public:
        LongSpinBox(QWidget *parent = nullptr);
        ~LongSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };

    Q_WIDGETS_EXPORT class ULongSpinBox : public QAbstractSpinBox
    {
        using T = unsigned long;
        using Qt_Type = ulong;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(ulong minimum MEMBER m_min)
        Q_PROPERTY(ulong maximum MEMBER m_max)
        Q_PROPERTY(ulong value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(ulong val);
    public:
        ULongSpinBox(QWidget *parent = nullptr);
        ~ULongSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };

    Q_WIDGETS_EXPORT class ShortSpinBox : public QAbstractSpinBox
    {
        using T = short;
        using Qt_Type = short;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(short minimum MEMBER m_min)
        Q_PROPERTY(short maximum MEMBER m_max)
        Q_PROPERTY(short value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(short val);
    public:
        ShortSpinBox(QWidget *parent = nullptr);
        ~ShortSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };

    Q_WIDGETS_EXPORT class UShortSpinBox : public QAbstractSpinBox
    {
        using T = unsigned short;
        using Qt_Type = ushort;
        Qt_Type m_value {0};
        Qt_Type m_min {std::numeric_limits<T>::min()};
        Qt_Type m_max {std::numeric_limits<T>::max()};
        Q_OBJECT
        Q_PROPERTY(ushort minimum MEMBER m_min)
        Q_PROPERTY(ushort maximum MEMBER m_max)
        Q_PROPERTY(ushort value MEMBER m_value NOTIFY valueChanged USER true)
    public Q_SLOTS:
        void onEdit();
    Q_SIGNALS:
        void valueChanged(ushort val);
    public:
        UShortSpinBox(QWidget *parent = nullptr);
        ~UShortSpinBox() override;
        QValidator::State validate(QString &text, int &pos) const override;
        void setRange(Qt_Type min, Qt_Type max);
        void stepBy(int steps) override;
        Qt_Type valueFromText(const QString &text) const;
        QString textFromValue(Qt_Type val) const;
        QAbstractSpinBox::StepEnabled stepEnabled() const override;
    };
}
