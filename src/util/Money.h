#pragma once

#include <cstdint>
#include <string>
#include <ostream>

namespace util {

// Деньги хранятся в копейках (int64_t), не во float/double:
// детерминированные накопительные расчёты амортизации без ошибок округления.
class Money {
public:
    Money() : kopecks_(0) {}
    explicit Money(std::int64_t kopecks) : kopecks_(kopecks) {}

    static Money fromRubles(double rubles);   // конвертация на границе ввода

    std::int64_t kopecks() const { return kopecks_; }
    double       rubles()  const { return static_cast<double>(kopecks_) / 100.0; }
    std::string  format()  const;              // "12 500,00 ₽"

    Money operator+(Money o) const { return Money(kopecks_ + o.kopecks_); }
    Money operator-(Money o) const { return Money(kopecks_ - o.kopecks_); }
    Money operator*(double k) const;           // для долей износа
    Money& operator+=(Money o) { kopecks_ += o.kopecks_; return *this; }

    bool operator==(Money o) const { return kopecks_ == o.kopecks_; }
    bool operator!=(Money o) const { return kopecks_ != o.kopecks_; }
    bool operator<(Money o)  const { return kopecks_ < o.kopecks_; }
    bool operator>(Money o)  const { return kopecks_ > o.kopecks_; }
    bool operator<=(Money o) const { return kopecks_ <= o.kopecks_; }
    bool operator>=(Money o) const { return kopecks_ >= o.kopecks_; }

    friend std::ostream& operator<<(std::ostream& os, Money m);

private:
    std::int64_t kopecks_;
};

}  // namespace util
