#include <iostream>
#include <algorithm>
#include <numeric>
#include <forward_list>
#include <chrono>
#include "forward_list.hpp"
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
int forward_list_test()
{
    std::cout << "forward_list_test{" << std::endl;

    std::cout << "----------------test1：构造函数-------------------" << std::endl;
    forward_list<int>a;                                 print(a, 'a');
    forward_list<int>b(5);                              print(b, 'b');
    forward_list<int>c(3, 3);                           print(c, 'c');
    double x[] = { 1.1,2.2,3.3 };
    forward_list<double>d(x, x + 3);                    print(d, 'd');
    forward_list<double>e(d);                           print(e, 'e');
    forward_list f{ 5,6,7,8,9 };                        print(f, 'f');
    forward_list g(f);                                  print(g, 'g');
    forward_list<int>h(++f.begin(), f.end());           print(h, 'h');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "----------------test2：assign/swap----------------" << std::endl;
    a = c;                                              print(a, 'a');
    b.assign(6, 2); std::cout << "b.size(): " << b.size() << std::endl;
    c.assign(8, 2);                                     print(c, 'c');
    b.swap(c);                                          print(b, 'b');
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
    a.insert_after(a.begin(), 7);                       print(a, 'a');
    int xx[3] = { 10,11,12 };
    a.insert_after(a.begin(), xx, xx + 3);              print(a, 'a');
    a.pop_front();                                      print(a, 'a');
    std::cout << a.size() << std::endl;;
    a.clear(); std::cout << (a.empty() ? "YES" : "NO") << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test5:元素访问------------------" << std::endl;
    forward_list<double>::iterator it = ++d.begin(); std::cout << *it << std::endl;
    *it = 8.8;                                          print(d, 'd');
    std::cout << d.front() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "------------------test6:链表操作------------------" << std::endl;
    forward_list A{ 1,3,5 }, B{ 2,4,6 };
    A.merge(B);                                         print(A, 'A'); print(B, 'B');
    forward_list C{ 6,7,7,8,8,9,9,10,10 };
    C.unique();                                         print(C, 'C');
    A.splice_after(A.before_begin(), C, C.begin(), C.end()); 
                                                        print(A, 'A'); print(C, 'C');
    A.reverse();                                        print(A, 'A');
    A.remove_if([](int elem) {return elem > 8; });      print(A, 'A');
    A.sort();                                           print(A, 'A');
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "-----------test7：比较std::forward_list-----------" << std::endl;
    const size_t N = 5e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push_front compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::forward_list<decltype(elem)>octFLIST;
    std::forward_list<decltype(elem)>stdFLIST;
    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        octFLIST.push_front(elem);
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        stdFLIST.push_front(elem);
    auto tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "insert compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    octFLIST.insert_after(octFLIST.begin(), N, elem);
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    stdFLIST.insert_after(stdFLIST.begin(), N, elem);
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "pop_front compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = 0; i < 2 * N; i++)
        octFLIST.pop_front();
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < 2 * N; i++)
        stdFLIST.pop_front();
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() <<  std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() <<  std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;

    return 0;
}