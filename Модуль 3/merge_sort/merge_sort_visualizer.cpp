#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <functional>
#include <mutex>
#include <atomic>

// ============================================
// ГЛОБАЛЬНЫЕ НАСТРОЙКИ ВИЗУАЛИЗАЦИИ
// ============================================
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int ARRAY_SIZE = 128;
const float BAR_SPACING = 2.0f;
const int VISUALIZATION_AREA_Y = 100;
const int VISUALIZATION_AREA_HEIGHT = 600;

// ============================================
// ЦВЕТА ДЛЯ СОСТОЯНИЙ
// ============================================
namespace Colors {
    const sf::Color DEFAULT(200, 200, 200);
    const sf::Color COMPARING(255, 255, 0);
    const sf::Color SWAPPING(255, 100, 100);
    const sf::Color SORTED(100, 255, 100);
    const sf::Color MERGING(100, 200, 255);
    const sf::Color THREAD_1(255, 150, 50);
    const sf::Color THREAD_2(150, 50, 255);
    const sf::Color THREAD_3(50, 255, 150);
    const sf::Color THRESHOLD(255, 200, 100);
    const sf::Color BACKGROUND(30, 30, 30);
    const sf::Color UI_TEXT(255, 255, 255);
    const sf::Color BORDER(100, 100, 100);
}

// ============================================
// СТРУКТУРА ДЛЯ СОСТОЯНИЯ ВИЗУАЛИЗАЦИИ
// ============================================
struct BarState {
    float value;
    sf::Color color;
    int thread_id;

    BarState(float v = 0.0f) : value(v), color(Colors::DEFAULT), thread_id(-1) {}
};

// ============================================
// КЛАСС ВИЗУАЛИЗАТОРА
// ============================================
class SortingVisualizer {
private:
    sf::RenderWindow window;
    sf::Font font;
    std::vector<BarState> bars;
    std::mutex mutex;
    std::atomic<bool> running{true};
    std::atomic<bool> paused{false};
    std::atomic<int> current_scenario{0};

    int comparisons = 0;
    int swaps = 0;
    std::string status_text = "Готов к запуску";

    float bar_width;
    float visualization_width;

public:
    SortingVisualizer()
        : window(sf::VideoMode(sf::Vector2u(WINDOW_WIDTH, WINDOW_HEIGHT)),
                 "Parallel Merge Sort Visualizer") {
        window.setFramerateLimit(60);

        // Загружаем шрифт (SFML 3.x использует openFromFile)
        if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc")) {
            if (!font.openFromFile("/System/Library/Fonts/SFNS.ttf")) {
                if (!font.openFromFile("/Library/Fonts/Arial.ttf")) {
                    std::cerr << "Warning: Could not load font, using default\n";
                    // Если не удалось загрузить, используем встроенный шрифт
                    font = sf::Font();
                }
            }
        }

        initializeBars();

        visualization_width = WINDOW_WIDTH - 100;
        bar_width = (visualization_width / ARRAY_SIZE) - BAR_SPACING;
    }

    void initializeBars() {
        bars.clear();
        bars.resize(ARRAY_SIZE);

        switch (current_scenario) {
            case 0: // Random
                for (int i = 0; i < ARRAY_SIZE; ++i) {
                    bars[i] = BarState(static_cast<float>(rand() % 100 + 1));
                }
                status_text = "Scenario: Random";
                break;

            case 1: // Best Case (already sorted)
                for (int i = 0; i < ARRAY_SIZE; ++i) {
                    bars[i] = BarState(static_cast<float>(i + 1));
                }
                status_text = "Scenario: Best Case (Already Sorted)";
                break;

            case 2: // Worst Case (reverse sorted)
                for (int i = 0; i < ARRAY_SIZE; ++i) {
                    bars[i] = BarState(static_cast<float>(ARRAY_SIZE - i));
                }
                status_text = "Scenario: Worst Case (Reverse Sorted)";
                break;
        }

        comparisons = 0;
        swaps = 0;
    }

    // ============================================
    // МЕТОДЫ ДЛЯ ОБНОВЛЕНИЯ ВИЗУАЛИЗАЦИИ
    // ============================================

    void setColor(int index, sf::Color color) {
        std::lock_guard<std::mutex> lock(mutex);
        if (index >= 0 && index < static_cast<int>(bars.size())) {
            bars[index].color = color;
        }
    }

    void setThreadColor(int left, int right, int thread_id) {
        std::lock_guard<std::mutex> lock(mutex);
        sf::Color color;
        switch (thread_id % 4) {
            case 0: color = Colors::THREAD_1; break;
            case 1: color = Colors::THREAD_2; break;
            case 2: color = Colors::THREAD_3; break;
            default: color = Colors::THREAD_1; break;
        }
        for (int i = left; i <= right && i < static_cast<int>(bars.size()); ++i) {
            bars[i].color = color;
            bars[i].thread_id = thread_id;
        }
    }

    void markComparing(int i, int j) {
        std::lock_guard<std::mutex> lock(mutex);
        if (i >= 0 && i < static_cast<int>(bars.size())) bars[i].color = Colors::COMPARING;
        if (j >= 0 && j < static_cast<int>(bars.size())) bars[j].color = Colors::COMPARING;
        comparisons++;
    }

    void markSwapping(int i, int j) {
        std::lock_guard<std::mutex> lock(mutex);
        if (i >= 0 && i < static_cast<int>(bars.size())) bars[i].color = Colors::SWAPPING;
        if (j >= 0 && j < static_cast<int>(bars.size())) bars[j].color = Colors::SWAPPING;
        swaps++;
    }

    void markSorted(int left, int right) {
        std::lock_guard<std::mutex> lock(mutex);
        for (int i = left; i <= right && i < static_cast<int>(bars.size()); ++i) {
            bars[i].color = Colors::SORTED;
        }
    }

    void markMerging(int left, int right) {
        std::lock_guard<std::mutex> lock(mutex);
        for (int i = left; i <= right && i < static_cast<int>(bars.size()); ++i) {
            bars[i].color = Colors::MERGING;
        }
    }

    void markThreshold(int left, int right) {
        std::lock_guard<std::mutex> lock(mutex);
        for (int i = left; i <= right && i < static_cast<int>(bars.size()); ++i) {
            bars[i].color = Colors::THRESHOLD;
        }
    }

    void swap(int i, int j) {
        std::lock_guard<std::mutex> lock(mutex);
        if (i >= 0 && i < static_cast<int>(bars.size()) &&
            j >= 0 && j < static_cast<int>(bars.size())) {
            std::swap(bars[i].value, bars[j].value);
        }
    }

    void delay(int milliseconds) {
        while (paused && running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void updateStatus(const std::string& status) {
        status_text = status;
    }

    // ============================================
    // ОТРИСОВКА
    // ============================================

    void render() {
        window.clear(Colors::BACKGROUND);

        std::lock_guard<std::mutex> lock(mutex);

        // Рисуем столбики
        float start_x = 50;
        for (size_t i = 0; i < bars.size(); ++i) {
            float bar_height = (bars[i].value / 100.0f) * VISUALIZATION_AREA_HEIGHT;
            float x = start_x + i * (bar_width + BAR_SPACING);
            float y = VISUALIZATION_AREA_Y + VISUALIZATION_AREA_HEIGHT - bar_height;

            sf::RectangleShape bar(sf::Vector2f(bar_width, bar_height));
            bar.setPosition(sf::Vector2f(x, y));  // SFML 3.x: Vector2f
            bar.setFillColor(bars[i].color);
            window.draw(bar);

            sf::RectangleShape border(sf::Vector2f(bar_width, bar_height));
            border.setPosition(sf::Vector2f(x, y));  // SFML 3.x: Vector2f
            border.setOutlineColor(Colors::BORDER);
            border.setOutlineThickness(1);
            border.setFillColor(sf::Color::Transparent);
            window.draw(border);
        }

        // UI: Заголовок
        sf::Text title(font, "Parallel Merge Sort - Real-time Visualization", 24);  // SFML 3.x: font first
        title.setFillColor(Colors::UI_TEXT);
        title.setPosition(sf::Vector2f(50, 20));  // SFML 3.x: Vector2f
        window.draw(title);

        // UI: Статус
        sf::Text status(font, status_text, 18);  // SFML 3.x: font first
        status.setFillColor(Colors::UI_TEXT);
        status.setPosition(sf::Vector2f(50, 60));  // SFML 3.x: Vector2f
        window.draw(status);

        // UI: Статистика
        sf::Text stats(font, "Comparisons: " + std::to_string(comparisons) +
                       "  |  Swaps: " + std::to_string(swaps), 16);  // SFML 3.x: font first
        stats.setFillColor(Colors::UI_TEXT);
        stats.setPosition(sf::Vector2f(WINDOW_WIDTH - 400, 30));  // SFML 3.x: Vector2f
        window.draw(stats);

        // Легенда
        drawLegend();

        // Кнопки управления

        window.display();
    }

    void drawLegend() {
    // ГОРИЗОНТАЛЬНАЯ ЛЕГЕНДА ПОД ГРАФИКОМ
    int legend_y = VISUALIZATION_AREA_Y + VISUALIZATION_AREA_HEIGHT + 20;
    int start_x = 50;
    int spacing = 150;  // Расстояние между элементами

    // Заголовок легенды
    sf::Text legend_title(font, "Legend:", 16);
    legend_title.setFillColor(Colors::UI_TEXT);
    legend_title.setPosition(sf::Vector2f(start_x, legend_y));
    window.draw(legend_title);

    // Элементы легенды (горизонтально)
    std::vector<std::pair<sf::Color, std::string>> legend_items = {
        {Colors::DEFAULT, "Default"},
        {Colors::COMPARING, "Comparing"},
        {Colors::SWAPPING, "Swapping"},
        {Colors::SORTED, "Sorted"},
        {Colors::MERGING, "Merging"},
        {Colors::THRESHOLD, "std::sort"}
    };

    int current_x = start_x + 90;  // Начинаем после заголовка

    for (size_t i = 0; i < legend_items.size(); ++i) {
        // Цветной квадратик
        sf::RectangleShape color_box(sf::Vector2f(18, 18));
        color_box.setPosition(sf::Vector2f(current_x, legend_y + 2));
        color_box.setFillColor(legend_items[i].first);
        color_box.setOutlineColor(Colors::BORDER);
        color_box.setOutlineThickness(1);
        window.draw(color_box);

        // Текст
        sf::Text label(font, legend_items[i].second, 13);
        label.setFillColor(Colors::UI_TEXT);
        label.setPosition(sf::Vector2f(current_x + 25, legend_y));
        window.draw(label);

        current_x += spacing;  // Сдвигаем следующий элемент
    }

    // ============================================
    // ИНФОРМАЦИЯ О КЛАВИШАХ УПРАВЛЕНИЯ (ЧУТЬ НИЖЕ ЛЕГЕНДЫ)
    // ============================================
    int controls_y = legend_y + 40;  // Добавлен дополнительный отступ

    sf::Text controls_title(font, "Controls:", 16);
    controls_title.setFillColor(Colors::UI_TEXT);
    controls_title.setPosition(sf::Vector2f(start_x, controls_y));
    window.draw(controls_title);

    // Список клавиш (горизонтально)
    std::vector<std::pair<std::string, std::string>> key_bindings = {
        {"SPACE", "Start/Pause"},
        {"R", "Reset"},
        {"1", "Random"},
        {"2", "Best Case"},
        {"3", "Worst Case"},
        {"ESC", "Quit"}
    };

    current_x = start_x + 100;  // Начинаем после "Controls:"
    int key_spacing = 140;  // Расстояние между клавишами

    for (size_t i = 0; i < key_bindings.size(); ++i) {
        // Клавиша в квадратных скобках
        sf::Text key_text(font, "[" + key_bindings[i].first + "]", 14);
        key_text.setFillColor(Colors::UI_TEXT);
        key_text.setPosition(sf::Vector2f(current_x, controls_y));
        window.draw(key_text);

        // Описание клавиши
        sf::Text desc_text(font, key_bindings[i].second, 14);
        desc_text.setFillColor(Colors::UI_TEXT);
        desc_text.setPosition(sf::Vector2f(current_x + 50, controls_y));
        window.draw(desc_text);

        current_x += key_spacing;  // Сдвигаем следующую клавишу
    }
}

    // ============================================
    // ГЛАВНЫЙ ЦИКЛ
    // ============================================

    void run() {
        std::thread sort_thread;
        bool sorting_active = false;

        while (window.isOpen() && running) {
            while (const auto event = window.pollEvent()) {  // SFML 3.x: optional event
                if (event->is<sf::Event::Closed>()) {  // SFML 3.x: new event API
                    running = false;
                    window.close();
                }

                if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {  // SFML 3.x: getIf
                    if (keyEvent->scancode == sf::Keyboard::Scancode::Escape) {
                        running = false;
                        window.close();
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scancode::Space) {
                        if (!sorting_active) {
                            sorting_active = true;
                            sort_thread = std::thread([this]() {
                                parallelMergeSort(0, ARRAY_SIZE - 1, 0);
                                markSorted(0, ARRAY_SIZE - 1);
                                updateStatus("Sorting Complete!");
                            });
                            sort_thread.detach();
                        } else {
                            paused = !paused;
                            updateStatus(paused ? "PAUSED" : "Running...");
                        }
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scancode::R) {
                        initializeBars();
                        sorting_active = false;
                    }

                    if (keyEvent->scancode == sf::Keyboard::Scancode::Num1) {
                        current_scenario = 0;
                        initializeBars();
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scancode::Num2) {
                        current_scenario = 1;
                        initializeBars();
                    }
                    if (keyEvent->scancode == sf::Keyboard::Scancode::Num3) {
                        current_scenario = 2;
                        initializeBars();
                    }
                }
            }

            render();
        }
    }

    // ============================================
    // АЛГОРИТМ СОРТИРОВКИ (С ВИЗУАЛИЗАЦИЕЙ)
    // ============================================

    void merge(int left, int mid, int right, int thread_id) {
        markMerging(left, right);
        updateStatus("Merging [" + std::to_string(left) + ".." + std::to_string(right) + "]");
        delay(30);

        std::vector<float> temp;
        {
            std::lock_guard<std::mutex> lock(mutex);
            for (int i = left; i <= right; ++i) {
                temp.push_back(bars[i].value);
            }
        }

        int i = 0, j = mid - left + 1, k = left;
        int mid_idx = mid - left;

        while (i <= mid_idx && j < static_cast<int>(temp.size())) {
            markComparing(left + i, left + j);
            delay(20);

            if (temp[i] <= temp[j]) {
                setColor(k, Colors::SORTED);
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    bars[k].value = temp[i];
                }
                i++;
            } else {
                setColor(k, Colors::SORTED);
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    bars[k].value = temp[j];
                }
                j++;
            }
            k++;
            delay(20);
        }

        while (i <= mid_idx) {
            setColor(k, Colors::SORTED);
            {
                std::lock_guard<std::mutex> lock(mutex);
                bars[k].value = temp[i];
            }
            i++;
            k++;
            delay(10);
        }

        markSorted(left, right);
    }

    void insertionSort(int left, int right, int thread_id) {
        markThreshold(left, right);
        updateStatus("std::sort (Threshold) on [" + std::to_string(left) + ".." + std::to_string(right) + "]");
        delay(50);

        for (int i = left + 1; i <= right; ++i) {
            int j = i;
            while (j > left) {
                markComparing(j - 1, j);
                delay(10);

                float val_j, val_j1;
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    val_j = bars[j].value;
                    val_j1 = bars[j - 1].value;
                }

                if (val_j < val_j1) {
                    markSwapping(j - 1, j);
                    swap(j - 1, j);
                    delay(10);
                    j--;
                } else {
                    break;
                }
            }
        }
        markSorted(left, right);
    }

    void parallelMergeSort(int left, int right, int depth) {
        if (left >= right) return;

        const int THRESHOLD = 16;

        if (right - left < THRESHOLD) {
            insertionSort(left, right, depth);
            return;
        }

        int mid = left + (right - left) / 2;

        setThreadColor(left, mid, depth);
        updateStatus("Thread " + std::to_string(depth) + " sorting [" +
                    std::to_string(left) + ".." + std::to_string(mid) + "]");
        delay(50);

        if (depth < 3) {
            auto left_future = std::async(std::launch::async,
                                          [this, left, mid, depth]() {
                parallelMergeSort(left, mid, depth + 1);
            });

            setThreadColor(mid + 1, right, depth + 10);
            parallelMergeSort(mid + 1, right, depth + 1);

            left_future.get();
        } else {
            parallelMergeSort(left, mid, depth + 1);
            parallelMergeSort(mid + 1, right, depth + 1);
        }

        float mid_val, mid_plus_1_val;
        {
            std::lock_guard<std::mutex> lock(mutex);
            mid_val = bars[mid].value;
            mid_plus_1_val = bars[mid + 1].value;
        }

        if (mid_val <= mid_plus_1_val) {
            markSorted(left, right);
            return;
        }

        merge(left, mid, right, depth);
    }
};

// ============================================
// ГЛАВНАЯ ФУНКЦИЯ
// ============================================
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    SortingVisualizer visualizer;
    visualizer.run();

    return 0;
}