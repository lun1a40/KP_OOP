#include "util/Money.h"

#include <cmath>
#include <cstdlib>

namespace util {

Money Money::fromRubles(double rubles) {
    // Округление к ближайшей копейке.
    return Money(static_cast<std::int64_t>(std::llround(rubles * 100.0)));
}

Money Money::operator*(double k) const {
    return Money(static_cast<std::int64_t>(std::llround(static_cast<double>(kopecks_) * k)));
}

std::string Money::format() const {
    std::int64_t abs = kopecks_ < 0 ? -kopecks_ : kopecks_;
    std::int64_t rub = abs / 100;
    std::int64_t kop = abs % 100;

    // Целая часть с разбивкой по три разряда неразрывным пробелом.
    std::string digits = std::to_string(rub);
    std::string grouped;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        grouped.insert(grouped.begin(), digits[i]);
        if (++count % 3 == 0 && i != 0) {
            grouped.insert(grouped.begin(), {'\xC2', '\xA0'});  // U+00A0 (UTF-8)
        }
    }

    char kopBuf[4];
    std::snprintf(kopBuf, sizeof(kopBuf), "%02lld", static_cast<long long>(kop));

    std::string result;
    if (kopecks_ < 0) result += '-';
    result += grouped;
    result += ',';
    result += kopBuf;
    result += "\xC2\xA0\xE2\x82\xBD";  // неразрывный пробел + ₽ (U+20BD)
    return result;
}

std::ostream& operator<<(std::ostream& os, Money m) {
    return os << m.format();
}

}  // namespace util
