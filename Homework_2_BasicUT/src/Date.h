#pragma once

#include <string>

namespace NWeek
{

class Date
{
public:
    Date(int year, int month, int day);

    static Date Parse(const std::string& value);
    static bool IsLeapYear(int year);

    int GetYear() const;
    int GetMonth() const;
    int GetDay() const;
    int GetDayOfYear() const;
    int GetIsoDayOfWeek() const;

private:
    static int DaysInMonth(int year, int month);
    static void Validate(int year, int month, int day);

    int m_year = 0;
    int m_month = 0;
    int m_day = 0;
};

} // namespace NWeek
