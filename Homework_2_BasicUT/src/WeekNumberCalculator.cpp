#include "WeekNumberCalculator.h"

namespace NWeek
{

int WeekNumberCalculator::GetIsoWeekNumber(const Date& date) const
{
    const int rawWeekNumber = (date.GetDayOfYear() - date.GetIsoDayOfWeek() + 10) / 7;

    if (rawWeekNumber < 1)
    {
        return GetWeeksInIsoYear(date.GetYear() - 1);
    }

    const int weeksInCurrentYear = GetWeeksInIsoYear(date.GetYear());
    if (rawWeekNumber > weeksInCurrentYear)
    {
        return 1;
    }

    return rawWeekNumber;
}

int WeekNumberCalculator::GetWeeksInIsoYear(int year) const
{
    const Date firstDayOfYear(year, 1, 1);
    const int firstDayOfWeek = firstDayOfYear.GetIsoDayOfWeek();

    if (firstDayOfWeek == 4 || (Date::IsLeapYear(year) && firstDayOfWeek == 3))
    {
        return 53;
    }

    return 52;
}

} // namespace NWeek
