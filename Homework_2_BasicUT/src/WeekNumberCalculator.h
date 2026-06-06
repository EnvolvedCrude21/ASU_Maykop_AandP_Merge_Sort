#pragma once

#include "Date.h"

namespace NWeek
{

class WeekNumberCalculator
{
public:
    int GetIsoWeekNumber(const Date& date) const;

private:
    int GetWeeksInIsoYear(int year) const;
};

} // namespace NWeek
