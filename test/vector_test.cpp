#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <chrono>
#include "vector.hpp"
using namespace oct;
using namespace std::chrono;

template<typename Container>
void print(Container& container, const char ch = 0){
    if (ch)std::cout << ch << ':';
    if (container.empty())std::cout << "EMPTY";
    else for (const auto& elem : container)
        std::cout << elem << ' ';
    std::cout << '\n';
}
int vector_test()
{
    std::cout << "vector_test{" << std::endl;

    std::cout << "----------------test1：构造函数-------------------" << std::endl;
    vector<int>a;                                       print(a,'a');
	vector<int>b(5);                                    print(b, 'b');
    vector<int>c(3, 3);                                 print(c,'c');
    double x[] = { 1.1,2.2,3.3 };
    vector<double>d(x, x + 3);                          print(d,'d');
    vector<double>e(d);                                 print(e,'e');

    vector f{ 5,6,7,8,9 };   /*模版推导*/                print(f, 'f');
    vector g(f);             /*模版推导*/                print(g,'g');          
    vector<int>h(f.begin() + 1, f.end()); print(h, 'h');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "----------------test2：assign/swap----------------" << std::endl;
    a = c;                                              print(a,'a');
    b.assign(6, 2); std::cout << "b.size(): "<<b.size() << std::endl;
    c.assign(8, 2);                                     print(c,'c');
    b.swap(c);                                          print(b,'b');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "-------test3:resize/reverse/对接<algorithm>-------" << std::endl;
    std::cout << b.size() << std::endl;
    std::cout << (b.empty() ? "YES" : "NO") << std::endl;
    b.resize(10, 5);                                    print(b,'b');
    std::cout << "b.capacity(): " << b.capacity() << std::endl;
    b.reserve(50);
    std::cout << "b.capacity(): " << b.capacity() << std::endl;
    b.resize(10, 5);                                    print(b, 'b');
    std::iota(b.begin(), b.end(), 0);                   print(b, 'b');
    std::reverse(b.begin(), b.begin() + 5);             print(b, 'b');
    std::sort(b.begin(), b.end());                      print(b, 'b');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "---------------test4:元素添加/删除----------------" << std::endl;
    a.push_back(10);
    a << 10 << 10 << 10 << 10;                          print(a,'a');
    a.insert(a.begin() + 1, 7);                         print(a, 'a');
    int xx[3] = { 10,11,12 };
    a.insert(a.begin() + 1, xx, xx + 2);                print(a,'a');
    a.pop_back();                                       print(a,'a');
    a.erase(a.begin()+2);                               print(a,'a');
    a.erase(a.begin()+4, a.begin()+6);                  print(a,'a');
    a.erase(a.begin()+2, a.end());                      print(a,'a');
    std::cout << a.capacity() << std::endl;;
    a.clear(); std::cout << (a.empty() ? "YES" : "NO") << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test5:元素访问------------------" << std::endl;
    vector<double>::iterator it = d.begin() + 2; std::cout << *it << std::endl;
    d[2] = 8.8;                                         print(d, 'd');
    std::cout << d.at(1) << std::endl;
    d.insert(d.insert(d.end(), 3, 1.5),{2.5,3.5,4.5});  print(d, 'd');
    std::cout << std::endl;
    std::cout << d.front() << ' ' << d.back() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "--------------test6：比较std::vector--------------" << std::endl;
    const size_t N = 5e8;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push_back compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::vector<decltype(elem)>octVEC;
    std::vector<decltype(elem)>stdVEC;
    auto tick1 = steady_clock::now();
    for (int i = 0; i < N; i++)
        octVEC.push_back(elem);
    auto tick2 = steady_clock::now();
    auto tick3 = steady_clock::now();
    for (int i = 0; i < N; i++)
        stdVEC.push_back(elem);
    auto tick4 = steady_clock::now();;
    std::cout << "oct:"<< duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:"<< duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;

    std::cout << std::endl << "insert compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    octVEC.insert(octVEC.begin(), N, elem);
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    stdVEC.insert(stdVEC.begin(), N, elem);
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;

    std::cout << std::endl << "pop_back compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = 0; i < 2 * N; i++)
        octVEC.pop_back();
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < 2 * N; i++)
        stdVEC.pop_back();
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << " capacity:" << octVEC.capacity() <<std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << " capacity:" << stdVEC.capacity() << std::endl;

    std::cout << std::endl << "constrcut compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    oct::vector<decltype(elem)> octVECx(N, elem);
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    std::vector<decltype(elem)>stdVECx(N,elem);
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(duration<double, std::milli>(tick2 - tick1).count()).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(duration<double, std::milli>(tick4 - tick3).count()).count() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;

    return 0;
}