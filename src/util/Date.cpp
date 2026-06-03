#include "util/Date.h"

#include <ctime>
#include <cstdio>
#include <stdexcept>

namespace util {

bool Date::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int Date::daysInMonth(int year, int month) {
    static const int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Date: некорректный месяц " + std::to_string(month));
    }
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

Date::Date(int year, int month, int day) : year_(year), month_(month), day_(day) {
    if (month < 1 || month > 12) {
        throw std::invalid_argument("Date: месяц вне диапазона 1..12");
    }
    if (day < 1 || day > daysInMonth(year, month)) {
        throw std::invalid_argument("Date: день вне диапазона для месяца");
    }
}

Date Date::today() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

Date Date::fromIso(const std::string& iso) {
    int y = 0, m = 0, d = 0;
    // Строгий формат YYYY-MM-DD; %n проверяет, что строка израсходована целиком.
    int consumed = 0;
    if (std::sscanf(iso.c_str(), "%d-%d-%d%n", &y, &m, &d, &consumed) != 3 ||
        consumed != static_cast<int>(iso.size())) {
        throw std::invalid_argument("Date: ожидался формат YYYY-MM-DD, получено '" + iso + "'");
    }
    return Date(y, m, d);  // конструктор довалидирует диапазоны
}

std::string Date::toIso() const {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year_, month_, day_);
    return std::string(buf);
}

// Алгоритм days_from_civil (Howard Hinnant): дни от 1970-01-01.
long long Date::toSerial() const {
    long long y = year_;
    const long long m = month_;
    const long long d = day_;
    y -= m <= 2;
    const long long era = (y >= 0 ? y : y - 399) / 400;
    const long long yoe = y - era * 400;                                  // [0, 399]
    const long long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1; // [0, 365]
    const long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;          // [0, 146096]
    return era * 146097 + doe - 719468;
}

int Date::daysBetween(const Date& other) const {
    return static_cast<int>(other.toSerial() - toSerial());
}

int Date::monthsBetween(const Date& other) const {
    int months = (other.year_ - year_) * 12 + (other.month_ - month_);
    // Месяц считается завершённым только если день достигнут.
    if (other.day_ < day_) {
        --months;
    }
    return months;
}

bool Date::operator==(const Date& o) const {
    return year_ == o.year_ && month_ == o.month_ && day_ == o.day_;
}

bool Date::operator<(const Date& o) const {
    if (year_ != o.year_)   return year_ < o.year_;
    if (month_ != o.month_) return month_ < o.month_;
    return day_ < o.day_;
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
    return os << d.toIso();
}

}  // namespace util
