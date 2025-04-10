#include <iostream>
#include <algorithm>
#include <numeric>
#include <list>
#include <chrono>
#include "list.hpp"
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
int list_test()
{
    std::cout << "list_test{" << std::endl;

    std::cout << "----------------test1：构造函数-------------------" << std::endl;
    list<int>a;                                         print(a, 'a');
    list<int>b(5);                                      print(b, 'b');
    list<int>c(3, 3);                                   print(c, 'c');
    double x[] = { 1.1,2.2,3.3 };
    list<double>d(x, x + 3);                            print(d, 'd');
    list<double>e(d);                                   print(e, 'e');
    list f{ 5,6,7,8,9 };                                print(f, 'f');
    list g(f);                                          print(g, 'g');
    list<int>h(++f.begin(), f.end());                   print(h, 'h');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "----------------test2：assign/swap----------------" << std::endl;
    a = c;                                              print(a, 'a');
    b.assign(6, 2); std::cout << "b.size(): " << b.size() << std::endl;
    c.assign(8, 2);                                     print(c, 'c');
    b.swap(c);                                          print(b, 'b');
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------test3:resize/对接<algorithm>----------" << std::endl;
    std::cout << b.size() << std::endl;
    std::cout << (b.empty() ? "YES" : "NO") << std::endl;
    b.resize(10, 5);                                    print(b, 'b');
    std::iota(b.begin(), b.end(), 0);                   print(b, 'b');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "---------------test4:元素添加/删除----------------" << std::endl;
    a.push_front(10);
    10 >> (10 >> a);                                    print(a, 'a');
    int xx[3] = { 10,11,12 };
    a.pop_front(); 
    a.insert(a.end(), xx, xx + 3);                      print(a, 'a');
    std::cout << a.size() << std::endl;;
    a.clear(); std::cout << (a.empty() ? "YES" : "NO") << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test5:元素访问------------------" << std::endl;
    list<double>::iterator it = ++d.begin(); std::cout << *it << std::endl;
    *it = 8.8;                                          print(d, 'd');
    for (const double& db : d)std::cout << db << ' ';
    std::cout << std::endl;
    std::cout << d.front() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test6:链表操作------------------" << std::endl;
    list A{ 1, 3, 5 }, B{ 2, 4, 6, 7, 8 };
    A.merge(B);                                         print(A, 'A'); print(B, 'B');
    list C{ 6,7,7,8,8,9,9,10,10 };
    C.unique();                                         print(C, 'C');
    A.splice(A.begin(), C, C.begin(), C.end());         print(A, 'A'); print(C, 'C');
    A.reverse();                                        print(A, 'A');
    A.remove_if([](int elem) {return elem > 8; });      print(A, 'A');
    A.sort();                                           print(A, 'A');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test7：比较std::list------------" << std::endl;
    const size_t N = 5e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::list<decltype(elem)>octLIST;
    std::list<decltype(elem)>stdLIST;
    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N / 2; i++) {
        octLIST.push_front(elem);
        octLIST.push_back(elem);
    }
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;
    for (int i = 0; i < N / 2; i++) {
        stdLIST.push_front(elem);
        stdLIST.push_front(elem);
    }
    auto tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "insert compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    octLIST.insert(octLIST.end(), N, elem);
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    stdLIST.insert(stdLIST.end(), N, elem);
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;
    

    std::cout << std::endl << "pop compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        octLIST.pop_front();
        octLIST.pop_back();
    }
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        stdLIST.pop_front();
        stdLIST.pop_back();
    }
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;

    return 0;
}