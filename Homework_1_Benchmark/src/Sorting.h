#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace NBenchmark
{

template<typename T>
void BubbleSortClassic(std::vector<T>& values)
{
    if (values.size() < 2)
    {
        return;
    }

    const std::size_t size = values.size();
    for (std::size_t i = 0; i + 1 < size; ++i)
    {
        for (std::size_t j = 0; j + 1 < size; ++j)
        {
            if (values[j] > values[j + 1])
            {
                std::swap(values[j], values[j + 1]);
            }
        }
    }
}

template<typename T>
void BubbleSortOptimized(std::vector<T>& values)
{
    if (values.size() < 2)
    {
        return;
    }

    std::size_t activeSize = values.size();
    while (activeSize > 1)
    {
        std::size_t lastSwapIndex = 0;

        for (std::size_t i = 1; i < activeSize; ++i)
        {
            if (values[i - 1] > values[i])
            {
                std::swap(values[i - 1], values[i]);
                lastSwapIndex = i;
            }
        }

        if (lastSwapIndex == 0)
        {
            break;
        }

        activeSize = lastSwapIndex;
    }
}

template<typename T>
void InsertionSort(std::vector<T>& values)
{
    for (std::size_t i = 1; i < values.size(); ++i)
    {
        T current = values[i];
        std::size_t position = i;

        while (position > 0 && values[position - 1] > current)
        {
            values[position] = values[position - 1];
            --position;
        }

        values[position] = current;
    }
}

template<typename T>
void StdSort(std::vector<T>& values)
{
    std::sort(values.begin(), values.end());
}

template<typename T>
bool IsSorted(const std::vector<T>& values)
{
    return std::is_sorted(values.begin(), values.end());
}

} // namespace NBenchmark
