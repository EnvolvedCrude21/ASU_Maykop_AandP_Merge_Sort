#include <iostream>
#include <vector>
#include <algorithm>
#include <future>
#include <chrono>
#include <random>
#include <thread>

template <typename T>
class ParallelMergeSorter {
private:
    size_t threshold_;
    int max_depth_;

    std::vector<T> buffer_;

    int calculate_max_depth() const {
        unsigned int num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2;
        int depth = 0;
        while ((1 << depth) < num_threads) {
            depth++;
        }
        return depth;

    }
    void merge(std::vector<T>& arr, size_t left, size_t mid, size_t right) {
        std::copy(arr.begin()+left, arr.begin()+right+1, buffer_.begin()+left);
        size_t i = left;
        size_t j = mid+1;
        size_t k = left;
        while (i<=mid && j<=right) {
            if (buffer_[i]<=buffer_[j]) arr[k++] = buffer_[i++];
            else arr[k++] = buffer_[j++];
        }
        while (i<=mid)arr[k++]=buffer_[i++];


    }
    void sort_impl(std::vector<T>& arr, size_t left, size_t right, int depth) {
        if (left>=right)return;
        if (right-left<=threshold_) {
            std::sort(arr.begin()+left, arr.begin()+right+1);
            return;
        }
        size_t mid = left + (right - left) / 2;

        if (depth > 0) {
            auto left_future = std::async(std::launch::async, &ParallelMergeSorter::sort_impl,
                                          this, std::ref(arr), left, mid, depth - 1);
            sort_impl(arr, mid + 1, right, depth - 1);
            left_future.get();
        }else {
            sort_impl(arr, left, mid, 0);
            sort_impl(arr, mid + 1, right, 0);
        }

        if (arr[mid] <= arr[mid + 1]) return;

        merge(arr, left, mid, right);
    }

public:
    explicit ParallelMergeSorter(size_t threshold = 16384)
        : threshold_(threshold) {
        max_depth_ = calculate_max_depth();
    }
    void sort(std::vector<T>& arr) {
        if (arr.size() < 2) return;
        if (buffer_.size() < arr.size()) {
            buffer_.resize(arr.size());
        }
        sort_impl(arr, 0, arr.size() - 1, max_depth_);
    }
};

int main() {
    const size_t SIZE = 10000000;

    // 1. Лучший случай (Best Case): Массив УЖЕ отсортирован
    std::vector<int> best_case(SIZE);
    for (size_t i = 0; i < SIZE; ++i) best_case[i] = i;

    // 2. Худший случай (Worst Case): Массив отсортирован в ОБРАТНОМ порядке
    std::vector<int> worst_case(SIZE);
    for (size_t i = 0; i < SIZE; ++i) worst_case[i] = SIZE - i;

    // 3. Средний случай (Average Case): Случайные числа
    std::vector<int> avg_case(SIZE);
    std::mt19937 gen(42);
    std::uniform_int_distribution<> distrib(1, 50000000);
    for (size_t i = 0; i < SIZE; ++i) avg_case[i] = distrib(gen);

    auto run_test = [](const std::string& name, std::vector<int> data) {
        std::vector<int> data_sync = data;
        std::vector<int> data_async = data;

        auto start_sync = std::chrono::high_resolution_clock::now();
        std::sort(data_sync.begin(), data_sync.end());
        auto end_sync = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_sync = end_sync - start_sync;

        // 2. Тест класса
        ParallelMergeSorter<int> sorter;
        auto start_async = std::chrono::high_resolution_clock::now();
        sorter.sort(data_async);
        auto end_async = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> time_async = end_async - start_async;

        // Проверка корректности
        bool ok1 = std::is_sorted(data_sync.begin(), data_sync.end());
        bool ok2 = std::is_sorted(data_async.begin(), data_async.end());

        // Вывод результатов
        std::cout << "--- " << name << " ---\n";
        std::cout << "std::sort: " << time_sync.count() << " ms " << (ok1 ? "[OK]" : "[FAIL]") << "\n";
        std::cout << "Our sort:  " << time_async.count() << " ms " << (ok2 ? "[OK]" : "[FAIL]") << "\n\n";
    };


    std::cout << "Тестирование на массивах из " << SIZE << " элементов.\n";
    std::cout << "(Многопоточность не включится, т.к. SIZE < threshold_ 16384)\n\n";

    run_test("Best Case (Already Sorted)", best_case);
    run_test("Worst Case (Reverse Sorted)", worst_case);
    run_test("Average Case (Random)", avg_case);

    return 0;
}
//Компиляция: g++ -O3 -std=c++20 -I./stdexec/include -pthread sort.cpp -o merge_sort
// ./merge_sort
// ./visualizer