#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <map>
#include <string>
#include <chrono>
#include "map.hpp"
using namespace oct;
using namespace std::chrono;

template<typename Container>
void print(Container& container, const char ch = 0) {
    if (ch)std::cout << ch << ": ";
    if (container.empty())std::cout << "EMPTY";
    else for (const auto&  [key, val] : container)
        std::cout << '[' << key << ", " << val << "] ";
    std::cout << '\n';
}
int map_test()
{
    using namespace std::literals::string_literals;
    std::cout << "map_test{" << std::endl;

    std::cout << "----------------test1：构造函数-------------------" << std::endl;
    map<int, int>a;                                     print(a, 'a');
    pair<std::string, int> x[7] = { {"one", 1}, {"eight", 8}, {"three", 3}, {"four", 4}, {"seven", 7}, {"five", 5}, {"nine", 9} };
    map<std::string, int>b(x, x + 7);                  print(b, 'b');
    map<std::string, int>c(b);                          print(c, 'c');
    map<std::string, int>d(std::move(c));               print(d, 'd');
    map e(d);                                           print(e, 'e');
    map<std::string, int>f(++e.begin(), e.end());       print(f, 'f');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "--------------test2：operator=/swap---------------" << std::endl;
    c = d;                                              print(c, 'c');
    e.swap(f);                                          print(e, 'e');
    print(f, 'f');
    f = std::move(e);                                   print(f, 'f');
    print(e, 'e');
    std::cout << std::boolalpha << (e < f) << std::endl;;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "---------------test3:元素添加/删除----------------" << std::endl;
    a.clear();                                          print(a, 'a');
    a.insert({ 0, 1 });                                 print(a, 'a');
    a.insert({ { 1, 2 }, { 2, 4 }, { 3, 8 }, { 4, 16 }, { 5, 32 }, {6, 64} });
    print(a, 'a');
    for (int i : {6, 1, 2, 0, 3, 4, 5}) {
        a.erase(i);
        print(a, 'a');
    }
    a[8] = 256;
    a[7] = 128;
    a[9] = 512;                                         print(a, 'a');
    a[9] = 1024;                                        print(a, 'a');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test4:元素查找------------------" << std::endl;
    a.insert({ { 1, 2 }, { 2, 4 }, { 3, 8 }, { 4, 16 }, { 5, 32 }, {6, 64} });
    std::cout << "a.count(2):" << a.count(2) << std::endl;
    std::cout << "a.find(3):" << (*a.find(7)).second << std::endl;
    std::cout << "a.insert({6, 7}):" << std::boolalpha << a.insert({6, 64}).second << std::endl;
    for (auto it = a.lower_bound(2); it != a.upper_bound(5); it++) {
        auto& [k, v] = *it;
        std::cout << "[" << k <<", " << v << "]";
    }
    std::cout << std::endl;
    auto [lower, upper] = a.equal_range(5);
    std::cout << (*lower).second << ' ' << (*upper).second << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test5：比较std::map------------" << std::endl;
    const size_t N = 1e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "insert compare:(ms)" << std::endl;
    auto elem1 = int(1);
    auto elem2 = int(1);
    oct::map<decltype(elem1), decltype(elem2)>octMAP;
    std::map<decltype(elem1), decltype(elem2)>stdMAP;
    int* data1 = new int[N];
    int* data2 = new int[N];
    std::mt19937 mt;
    for (int i = 0; i < N; i++) {
        data1[i] = mt();
        data2[i] = mt();
    }

    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        octMAP[data1[i]] = data2[i];
    }
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;

    for (int i = 0; i < N; i++) {
        stdMAP[data1[i]] = data2[i];
    }

    auto tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "erase compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = N - 1; ~i; i--) {
        octMAP.erase(data1[i]);
    }
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = N - 1; ~i; i--) {
        stdMAP.erase(data1[i]);
    }
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;
    delete[] data1;
    delete[] data2;
    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "}" << std::endl << std::endl;

    return 0;
}