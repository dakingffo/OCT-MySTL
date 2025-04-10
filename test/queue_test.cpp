#include <iostream>
#include <queue>
#include <chrono>
#include "queue.hpp"
using namespace oct;
using namespace std::chrono;

int queue_test() {
    std::cout << "queue_test{" << std::endl;
    queue<char>que1;
    std::cout << "-------------------test1:入队---------------------" << std::endl;
    for (char i = 'a'; i <= 'z'; i++) {
        que1 << i;
        que1.push(i);
        std::cout << i << ' ';
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "-------------------test2:出队---------------------" << std::endl;
    queue<char>que2(que1);
    while (!que2.empty()) {
        std::cout << que2.front() << ' ';
        que2.pop();
    }
    std::cout << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "--------------test3：比较std::queue--------------" << std::endl;
    const size_t N = 5e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::queue<decltype(elem)>octQUE;
    std::queue<decltype(elem)>stdQUE;
    auto tick1 = steady_clock::now();;
    for (int i = 0; i < N; i++) {
        octQUE.push(elem);
    }
    auto tick2 = steady_clock::now();;
    auto tick3 = steady_clock::now();;
    for (int i = 0; i < N; i++)
        stdQUE.push(elem);
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