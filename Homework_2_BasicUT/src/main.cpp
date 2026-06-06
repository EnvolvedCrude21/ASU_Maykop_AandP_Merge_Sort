#include <iostream>
#include <string>

#include "Date.h"
#include "WeekNumberCalculator.h"

int main()
{
    std::cout << "Введите дату (гггг.мм.дд): ";

    std::string input;
    if (!std::getline(std::cin, input) || input.empty())
    {
        std::cerr << "Ошибка: дата не введена.\n";
        return 1;
    }

    try
    {
        const NWeek::Date date = NWeek::Date::Parse(input);
        const NWeek::WeekNumberCalculator calculator;

        std::cout << "Номер недели: " << calculator.GetIsoWeekNumber(date) << '\n';
        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Ошибка: " << exception.what() << '\n';
        return 1;
    }
}
