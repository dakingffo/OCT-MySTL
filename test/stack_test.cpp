#include <iostream>
#include <stack>
#include <chrono>
#include "stack.hpp"
using namespace oct;
using namespace std::chrono;

int stack_test() {
    std::cout << "stack_test{" << std::endl;
	stack<char>stk1;
	std::cout << "-------------------test1:入栈---------------------" << std::endl;
	for (char i = 'a'; i <= 'z'; i++) {
		stk1<<i;
		std::cout << i <<' ';
	}
	std::cout << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "-------------------test2:出栈---------------------" << std::endl;
	stack<char>stk2(stk1);
	while (!stk2.empty()) {
		std::cout << stk2.top()<<' ';
		stk2.pop();
	}
	std::cout << std::endl;
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "--------------test3：比较std::stack--------------" << std::endl;
    const size_t N = 5e7;
    std::cout << "数据量：" << N << std::endl;
    std::cout << "push compare:(ms)" << std::endl;
    auto elem = int(1);
    oct::stack<decltype(elem)>octSTK;
    std::stack<decltype(elem)>stdSTK;
    auto tick1 = steady_clock::now();
    for (int i = 0; i < N; i++)
        octSTK.push(elem);
    auto tick2 = steady_clock::now();
    auto tick3 = steady_clock::now();
    for (int i = 0; i < N; i++)
        stdSTK.push(elem);
    auto tick4 = steady_clock::now();
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;

    std::cout << std::endl << "pop compare:(ms)" << std::endl;
    tick1 = steady_clock::now();
    for (int i = 0; i < N; i++)
        octSTK.pop();
    tick2 = steady_clock::now();
    tick3 = steady_clock::now();
    for (int i = 0; i < N; i++)
        stdSTK.pop();
    tick4 = steady_clock::now();
    std::cout << "oct:" << duration<double, std::milli>(tick2 - tick1).count() << std::endl;
    std::cout << "std:" << duration<double, std::milli>(tick4 - tick3).count() << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    std::cout << "}" << std::endl << std::endl;;

	return 0;
}