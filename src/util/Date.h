#pragma once

#include <string>
#include <ostream>

namespace util {

// Собственный класс даты: предсказуемая сериализация в ISO 8601,
// без C++20 chrono-зависимостей, которые в MSVC 2019 поддержаны частично.
class Date {
public:
    Date(int year, int month, int day);

    static Date today();
    static Date fromIso(const std::string& iso);   // "2026-06-03"
    std::string toIso() const;

    int year()  const { return year_; }
    int month() const { return month_; }
    int day()   const { return day_; }

    // Число полных месяцев от *this до other (может быть отрицательным).
    int monthsBetween(const Date& other) const;
    // Число дней от *this до other (может быть отрицательным).
    int daysBetween(const Date& other) const;

    bool operator==(const Date& o) const;
    bool operator!=(const Date& o) const { return !(*this == o); }
    bool operator<(const Date& o) const;
    bool operator<=(const Date& o) const { return *this < o || *this == o; }
    bool operator>(const Date& o) const { return o < *this; }
    bool operator>=(const Date& o) const { return !(*this < o); }

    friend std::ostream& operator<<(std::ostream& os, const Date& d);

    static bool isLeapYear(int year);
    static int  daysInMonth(int year, int month);

private:
    int year_;
    int month_;
    int day_;

    // Порядковый номер дня (григорианский), для разности дат.
    long long toSerial() const;
};

}  // namespace util
