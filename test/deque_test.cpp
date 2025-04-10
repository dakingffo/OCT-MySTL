#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include <deque>
#include <chrono>
#include "deque.hpp"
using namespace oct;
using namespace std::chrono;

template<typename Container>
void print(Container& container, const char ch = 0) {
    if (ch)std::cout << ch << ':';
    if (container.empty())std::cout << "EMPTY";
    else for (const auto& elem : container)
        std::cout << elem << ' ';
    std::cout << '\n';
}
int deque_test()
{
    std::cout << "deque_test{" << std::endl;

    std::cout << "----------------test1：构造函数-------------------" << std::endl;
    deque<int>a;                                        print(a, 'a');
    deque<int>b(5);                                     print(b, 'b');
    deque<int>c(3, 3);                                  print(c, 'c');
    double x[] = { 1.1,2.2,3.3 };
    deque<double>d(x, x + 3);                           print(d, 'd');
    deque<double>e(d);                                  print(e, 'e');

    deque f{ 5,6,7,8,9 };                               print(f, 'f');
    deque g(f);                                         print(g, 'g');
    deque<int>h(f.begin() + 1, f.end());                print(h, 'h');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "----------------test2：assign/swap----------------" << std::endl;
    a = c;                                              print(a, 'a');
    b.assign(6, 2); std::cout << "b.size(): " << b.size() << std::endl;
    c.assign(8, 2);                                     print(c, 'c');
    b.swap(c);                                          print(b, 'b');
    std::cout << "--------------------------------------------------" << std::endl;
    
    std::cout << "-------test3:resize/reverse/对接<algorithm>-------" << std::endl;
    std::cout << b.size() << std::endl;
    std::cout << (b.empty() ? "YES" : "NO") << std::endl;
    //b.resize(10, 5); print(b, 'b');
    std::iota(b.begin(), b.end(), 0);                   print(b, 'b');
    std::reverse(b.begin(), b.begin() + 5);             print(b, 'b');
    std::sort(b.begin(), b.end());                      print(b, 'b');
    std::cout << "--------------------------------------------------" << std::endl;
    
    std::cout << "---------------test4:元素添加/删除----------------" << std::endl;
    a.clear();0 >> (0 >> (0 >> a));                     print(a, 'a');
    for (int i = 1; i < 24; i += 2) {
        i >> a << i + 1;
        print(a, 'a');
    }
    while (a.size() > 1) {
        a.pop_front();
        a.pop_back(); 
        print(a, 'a');
    }
    /*
    a.insert(a.begin() + 1, 7); print(a, 'a');
    int xx[3] = { 10,11,12 };
    a.insert(a.begin() + 1, xx, xx + 2); print(a, 'a');
    a.erase(a.begin() + 2); print(a, 'a');
    a.erase(a.begin() + 4, a.begin() + 6); print(a, 'a');
    a.erase(a.begin() + 2, a.end()); print(a, 'a');
    std::cout << a.size() << std::endl;
    \*/
    a.clear(); std::cout << (a.empty() ? "YES" : "NO") << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test5:元素访问------------------" << std::endl;
    deque<double>::iterator it = d.begin() + 2; std::cout << *it << std::endl;
    d[2] = 8.8;                                         print(d, 'd');
    std::cout << d.at(1) << std::endl;
    //d.insert(d.insert(d.end(),3,1.5),{ 2.5,3.5,4.5}); print(d, 'd');
    std::cout << std::endl;
    std::cout << d.front() << ' ' << d.back() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;
    
    std::cout << "--------------test6：比较std::deque--------------" << std::endl;
    size_t N = 5e8;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::deque<decltype(elem)>octDEQ;
    std::deque<decltype(elem)>stdDEQ;
    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        octDEQ.push_front(elem);
        octDEQ.push_back(elem);
    }
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        stdDEQ.push_front(elem);
        stdDEQ.push_back(elem);
    }
    auto tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;

    std::cout << std::endl << "pop compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        octDEQ.pop_front();
        octDEQ.pop_back();
    }
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        stdDEQ.pop_front();
        stdDEQ.pop_back();
    }
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;

    std::cout << std::endl << "random operation compare:(ms)" << std::endl;
    std::mt19937 mt;
    tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        if (mt() & 1)
            octDEQ.push_front(i);
        else 
            octDEQ.push_back(i);
    }
    for (int i = 0; i < N; i++) {
        if (mt() & 1)
            octDEQ.pop_front();
        else
            octDEQ.pop_back();
    }
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        if (mt() & 1)
            stdDEQ.push_front(i);
        else
            stdDEQ.push_back(i);
    }
    for (int i = 0; i < N; i++) {
        if (mt() & 1)
            stdDEQ.pop_front();
        else
            stdDEQ.pop_back();
    }
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;

    std::cout << std::endl << "constrcut compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    oct::deque<decltype(elem)> octVECx(N, elem);
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    std::deque<decltype(elem)>stdVECx(N, elem);
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;

    return 0;
}