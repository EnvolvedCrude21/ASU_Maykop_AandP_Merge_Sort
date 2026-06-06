#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Date.h"
#include "WeekNumberCalculator.h"

namespace
{

class CTestRunner
{
public:
    using TTestFunction = std::function<void()>;

    void AddTest(const std::string& name, TTestFunction test)
    {
        m_tests.push_back({name, std::move(test)});
    }

    int Run()
    {
        std::cout << "Запуск " << m_tests.size() << " тестов...\n";

        int passed = 0;
        int failed = 0;

        for (const auto& test : m_tests)
        {
            try
            {
                test.function();
                ++passed;
                std::cout << "[PASS] " << test.name << '\n';
            }
            catch (const std::exception& exception)
            {
                ++failed;
                std::cout << "[FAIL] " << test.name << ": " << exception.what() << '\n';
            }
        }

        std::cout << "Итог: " << passed << " passed, " << failed << " failed\n";
        return failed == 0 ? 0 : 1;
    }

private:
    struct STestCase
    {
        std::string name;
        TTestFunction function;
    };

    std::vector<STestCase> m_tests;
};

void ExpectEqual(int expected, int actual, const std::string& message)
{
    if (expected != actual)
    {
        std::ostringstream error;
        error << message << ". expected = " << expected << ", actual = " << actual;
        throw std::runtime_error(error.str());
    }
}

void ExpectThrowsInvalidArgument(const std::function<void()>& action, const std::string& message)
{
    try
    {
        action();
    }
    catch (const std::invalid_argument&)
    {
        return;
    }

    throw std::runtime_error(message);
}

void TestParseValidDate()
{
    const NWeek::Date date = NWeek::Date::Parse("2026.06.06");

    ExpectEqual(2026, date.GetYear(), "Неверный год");
    ExpectEqual(6, date.GetMonth(), "Неверный месяц");
    ExpectEqual(6, date.GetDay(), "Неверный день");
}

void TestInvalidFormat()
{
    ExpectThrowsInvalidArgument(
        []()
        {
            NWeek::Date::Parse("2026-06-06");
        },
        "Дата в неверном формате должна выбрасывать исключение");
}

void TestInvalidMonth()
{
    ExpectThrowsInvalidArgument(
        []()
        {
            NWeek::Date::Parse("2026.13.06");
        },
        "Некорректный месяц должен выбрасывать исключение");
}

void TestInvalidDay()
{
    ExpectThrowsInvalidArgument(
        []()
        {
            NWeek::Date::Parse("2023.02.29");
        },
        "Некорректный день должен выбрасывать исключение");
}

void TestWeekOneAtStartOf2024()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2024.01.01");

    ExpectEqual(1, calculator.GetIsoWeekNumber(date), "Для 2024.01.01 ожидается 1 неделя");
}

void TestWeekFiftyThreeAtEndOf2020()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2020.12.31");

    ExpectEqual(53, calculator.GetIsoWeekNumber(date), "Для 2020.12.31 ожидается 53 неделя");
}

void TestWeekFiftyThreeFor2021FirstDay()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2021.01.01");

    ExpectEqual(53, calculator.GetIsoWeekNumber(date), "Для 2021.01.01 ожидается 53 неделя");
}

void TestWeekFiftyTwoFor2023FirstSunday()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2023.01.01");

    ExpectEqual(52, calculator.GetIsoWeekNumber(date), "Для 2023.01.01 ожидается 52 неделя");
}

void TestWeekOneFor2023FirstMonday()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2023.01.02");

    ExpectEqual(1, calculator.GetIsoWeekNumber(date), "Для 2023.01.02 ожидается 1 неделя");
}

void TestWeekOneFor2024LastDay()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2024.12.31");

    ExpectEqual(1, calculator.GetIsoWeekNumber(date), "Для 2024.12.31 ожидается 1 неделя");
}

void TestWeekTwentyThreeForSpecificDate()
{
    const NWeek::WeekNumberCalculator calculator;
    const NWeek::Date date = NWeek::Date::Parse("2026.06.06");

    ExpectEqual(23, calculator.GetIsoWeekNumber(date), "Для 2026.06.06 ожидается 23 неделя");
}

} // namespace

int main()
{
    CTestRunner runner;

    runner.AddTest("ParseValidDate", TestParseValidDate);
    runner.AddTest("InvalidFormat", TestInvalidFormat);
    runner.AddTest("InvalidMonth", TestInvalidMonth);
    runner.AddTest("InvalidDay", TestInvalidDay);
    runner.AddTest("WeekOneAtStartOf2024", TestWeekOneAtStartOf2024);
    runner.AddTest("WeekFiftyThreeAtEndOf2020", TestWeekFiftyThreeAtEndOf2020);
    runner.AddTest("WeekFiftyThreeFor2021FirstDay", TestWeekFiftyThreeFor2021FirstDay);
    runner.AddTest("WeekFiftyTwoFor2023FirstSunday", TestWeekFiftyTwoFor2023FirstSunday);
    runner.AddTest("WeekOneFor2023FirstMonday", TestWeekOneFor2023FirstMonday);
    runner.AddTest("WeekOneFor2024LastDay", TestWeekOneFor2024LastDay);
    runner.AddTest("WeekTwentyThreeForSpecificDate", TestWeekTwentyThreeForSpecificDate);

    return runner.Run();
}
