#include "Date.h"

#include <sstream>
#include <stdexcept>

namespace NWeek
{

Date::Date(int year, int month, int day)
    : m_year(year)
    , m_month(month)
    , m_day(day)
{
    Validate(year, month, day);
}

Date Date::Parse(const std::string& value)
{
    std::istringstream input(value);
    int year = 0;
    int month = 0;
    int day = 0;
    char firstDot = '\0';
    char secondDot = '\0';
    char extra = '\0';

    if (!(input >> year >> firstDot >> month >> secondDot >> day) ||
        firstDot != '.' ||
        secondDot != '.')
    {
        throw std::invalid_argument("Ожидается формат даты гггг.мм.дд.");
    }

    if (input >> extra)
    {
        throw std::invalid_argument("Дата содержит лишние символы.");
    }

    return Date(year, month, day);
}

bool Date::IsLeapYear(int year)
{
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

int Date::GetYear() const
{
    return m_year;
}

int Date::GetMonth() const
{
    return m_month;
}

int Date::GetDay() const
{
    return m_day;
}

int Date::GetDayOfYear() const
{
    int dayOfYear = 0;
    for (int month = 1; month < m_month; ++month)
    {
        dayOfYear += DaysInMonth(m_year, month);
    }

    return dayOfYear + m_day;
}

int Date::GetIsoDayOfWeek() const
{
    static const int monthOffsets[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

    int year = m_year;
    if (m_month < 3)
    {
        --year;
    }

    const int weekday = (year + year / 4 - year / 100 + year / 400 +
        monthOffsets[m_month - 1] + m_day) % 7;

    return weekday == 0 ? 7 : weekday;
}

int Date::DaysInMonth(int year, int month)
{
    static const int daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && IsLeapYear(year))
    {
        return 29;
    }

    return daysPerMonth[month - 1];
}

void Date::Validate(int year, int month, int day)
{
    if (year < 1)
    {
        throw std::invalid_argument("Год должен быть положительным.");
    }

    if (month < 1 || month > 12)
    {
        throw std::invalid_argument("Месяц должен быть в диапазоне от 1 до 12.");
    }

    const int maxDay = DaysInMonth(year, month);
    if (day < 1 || day > maxDay)
    {
        throw std::invalid_argument("Некорректный день для указанного месяца.");
    }
}

} // namespace NWeek
