#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <ctime>
#include "sorts/sorts.hpp"

typedef std::pair<std::vector<uint16_t>, uint64_t> (*Func)(size_t len, std::vector<uint16_t> arr);

constexpr int numOfFuncs = 12;
constexpr int milis = 1000;
constexpr int sortAttempts = 100;
constexpr int arrLength = 1000;

/**
 * @brief Test Is Sorted
 *
 * @param testing_arr
 */
void testIsSorted(std::vector<uint16_t> const &testing_arr)
{
    bool is_sorted = isSorted(testing_arr);
    if (!is_sorted)
    {
        std::cout << "testIsSorted [-]\n";
        return;
    }
    else
    {
        std::cout << "testIsSorted [+]\n";
    }
}
struct RandomArray
{
    std::vector<uint16_t> random_array;

    RandomArray()
    {
        random_array = std::vector<uint16_t>(arrLength);
        for (int i = 0; i < arrLength; ++i)
        {
            random_array[i] = i;
        }
    }

    void updateRandomArray()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(random_array.begin(), random_array.end(), gen);
    }
};

struct CSVSaver
{
    std::vector<std::string> func_names = {
        "BubbleSort",
        "BubbleSortIverson1",
        "BubbleSortIverson1&2",
        "SelectionSort",
        "LinearInsertionSort",
        "BinaryInsertionSort",
        "CountingSort",
        "RadixSort",
        "MergeSort",
        "HoarSort",
        "LomutoSort",
        "HeapSort",
    };
    std::vector<double> sort_name_vector;
    std::pair<std::vector<double>, std::vector<double>> rnd_test;
    std::pair<std::vector<double>, std::vector<double>> sorted_test;
    std::pair<std::vector<double>, std::vector<double>> reversed_test;
    std::pair<std::vector<double>, std::vector<double>> const_el_test;

    void write_csv(std::string filename)
    {
        std::vector<std::pair<std::string, std::vector<double>>> dataset = {
            {"sort_name", sort_name_vector},
            {"random_time", rnd_test.first},
            {"random_steps", rnd_test.second},
            {"sorted_time", sorted_test.first},
            {"sorted_steps", sorted_test.second},
            {"reversed_time", reversed_test.first},
            {"reversed_steps", reversed_test.second},
            {"same_el_time", const_el_test.first},
            {"same_el_steps", const_el_test.second},
        };
        std::ofstream myFile(filename);
        for (int j = 0; j < dataset.size(); ++j)
        {
            myFile << dataset.at(j).first;
            if (j != dataset.size() - 1)
                myFile << ";";
        }
        myFile << "\n";
        for (int i = 0; i < dataset.at(0).second.size(); ++i)
        {
            for (int j = 0; j < dataset.size(); ++j)
            {
                if (j == 0)
                {
                    myFile << func_names[dataset.at(j).second.at(i)];
                }
                else
                {
                    myFile << dataset.at(j).second.at(i);
                }
                if (j != dataset.size() - 1)
                    myFile << ";";
            }
            myFile << "\n";
        }
        myFile.close();
    }
};

/**
 * @brief Тест на ранд. наборе
 *
 * @param func
 * @return std::pair<double, uint64_t> <time, el operations>
 */
std::pair<double, uint64_t> randomArrayMeasure(std::vector<uint16_t> const &random_array, Func func)
{
    std::vector<uint16_t> arr(arrLength);
    for (int i = 0; i < arrLength; ++i)
    {
        arr[i] = random_array[i];
    }
    clock_t start = clock();
    auto sortRes = func(arrLength, arr);
    clock_t end = clock();
    testIsSorted(sortRes.first);
    return std::make_pair(static_cast<double>(end - start) * milis / CLOCKS_PER_SEC, sortRes.second);
}

std::pair<double, uint64_t> sortedArrayMeasure(Func func)
{
    std::vector<uint16_t> arr(arrLength);
    for (int i = 0; i < arrLength; ++i)
    {
        arr[i] = i;
    }

    clock_t start = clock();
    auto sortRes = func(arrLength, arr);
    clock_t end = clock();
    testIsSorted(sortRes.first);
    return std::make_pair(static_cast<double>(end - start) * milis / CLOCKS_PER_SEC, sortRes.second);
}

std::pair<double, uint64_t> reverseSortedArrayMeasure(Func func)
{
    std::vector<uint16_t> arr(arrLength);
    for (int i = 0; i < arrLength; ++i)
    {
        arr[i] = arrLength - i - 1;
    }

    clock_t start = clock();
    auto sortRes = func(arrLength, arr);
    clock_t end = clock();
    testIsSorted(sortRes.first);
    return std::make_pair(static_cast<double>(end - start) * milis / CLOCKS_PER_SEC, sortRes.second);
}

std::pair<double, uint64_t> allElementsAreSameArrayMeasure(Func func)
{
    std::vector<uint16_t> arr(arrLength, arrLength);

    clock_t start = clock();
    auto sortRes = func(arrLength, arr);
    clock_t end = clock();
    testIsSorted(sortRes.first);
    return std::make_pair(static_cast<double>(end - start) * milis / CLOCKS_PER_SEC, sortRes.second);
}

void measure(CSVSaver *scv_saver, std::vector<uint16_t> *current_random_array, Func func, int func_ind)
{
    scv_saver->sort_name_vector.push_back(func_ind);
    auto res = randomArrayMeasure(*current_random_array, func);
    scv_saver->rnd_test.first.push_back(res.first);
    scv_saver->rnd_test.second.push_back(res.second);
    auto res2 = sortedArrayMeasure(func);
    scv_saver->sorted_test.first.push_back(res2.first);
    scv_saver->sorted_test.second.push_back(res2.second);
    auto res3 = reverseSortedArrayMeasure(func);
    scv_saver->reversed_test.first.push_back(res3.first);
    scv_saver->reversed_test.second.push_back(res3.second);
    auto res4 = allElementsAreSameArrayMeasure(func);
    scv_saver->const_el_test.first.push_back(res4.first);
    scv_saver->const_el_test.second.push_back(res4.second);
}

int main()
{
    std::cout << "Start" << '\n';

    std::vector<Func> funcs = {
        bubbleSort,
        bubbleIversonOneSort,
        bubbleIversonOneAndTwoSort,
        selectionSort,
        linearInsertionSort,
        binaryInsertionSort,
        countingSort,
        radixSort,
        mergeSort,
        hoarSort,
        lomutoSort,
        heapSort,
    };
    auto random_array = new RandomArray();
    auto csv_saver = new CSVSaver();
    for (size_t j = 0; j < sortAttempts; j++)
    {
        random_array->updateRandomArray();
        for (int i = 0; i < numOfFuncs; ++i)
        {
            measure(csv_saver, &(random_array->random_array), funcs[i], i);
        }
    }

    csv_saver->write_csv("../docs/results.csv");
    std::cout << "End" << '\n';
    return 0;
}
