#include <iostream>
#include <queue>
#include <chrono>
#include <random>
#include "queue.hpp"
using namespace oct;
using namespace std::chrono;

int priority_queue_test() {
    std::cout << "priority_queue_test{" << std::endl;
    priority_queue<int>que1;
    priority_queue<int, vector<int>, greater<int>> que2;
    std::cout << "-------------------test1:入队---------------------" << std::endl;
    for (int i : {5,4,3,1,2,6,7,9,8,0}) {
        que1.push(i);//que1 << i;
        que2.push(i);
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "-------------------test2:出队---------------------" << std::endl;
    std::cout << "less<int>:";
    while (!que1.empty()) {
        std::cout << que1.top() << ' ';
        que1.pop();
    }
    std::cout << std::endl;
    std::cout << "greater<int>:";
    while (!que2.empty()) {
        std::cout << que2.top() << ' ';
        que2.pop();
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "--------------test3：比较std::priority_queue--------------" << std::endl;
    const size_t N = 5e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::priority_queue<decltype(elem)>octQUE;
    std::priority_queue<decltype(elem)>stdQUE;
    decltype(elem)* data = new decltype(elem)[N];
    std::mt19937 mt;
    for (int i = 0; i < N; i++)
        data[i] = mt();
    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        octQUE.push(data[i]);
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        stdQUE.push(data[i]);
    auto tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "pop compare:(ms)" << std::endl;
    tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        octQUE.pop();
    tick2 = steady_clock::now();;
    tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        stdQUE.pop();
    tick4 = steady_clock::now();;
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;;

    return 0;
}