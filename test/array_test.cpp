#include <iostream>
#include <algorithm>
#include <numeric>
#include <random>
#include "array.hpp"
using namespace oct;
template<typename Container>
void print(const Container& container, const char ch = 0){
	if (ch)std::cout << ch << ':';
	if (container.empty())std::cout << "EMPTY";
	else for (const auto& elem : container)
		std::cout << elem << ' ';
	std::cout << '\n';
}
int array_test()
{
	std::cout << "array_test{" << std::endl;

	std::cout << "----------test1：构造/访问/对接<algorithm>----------" << std::endl;
	array<int, 10>a = {0,1,2,3,4,5,6,7,8,9};			print(a,'a');
	std::iota(a.begin(), a.begin() + 10, 10);			print(a,'a');
	std::cout << "front:" << a.front() << " back:" << a.back() << std::endl;
	std::mt19937 mt;
	array b{ 1,2,3,4,5 };	/*模版推导*/					print(b, 'b');
	for (int i = 0; i < 5; i++) 
		b[i] = mt() % 50;								print(b, 'b');
	std::sort(b.begin(), b.end());						print(b, 'b');
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "----------test2：range_for循环/结构化绑定---------" << std::endl;
	array arr{ "Cpp","Hello","World" };	//模版推导
	auto [x, y, z] {arr};				//结构化绑定
	std::cout << "array<const char*, 3> arr{\"Cpp\",\"Hello\",\"World\"};" << std::endl;
	std::cout << "auto [x, y, z] = arr;\nx:" << x << "  y:" << y << "  z:" << z << std::endl;
	std::cout << "for (const char* msg : arr)" << std::endl;
	for (const char* msg : arr) 
		std::cout << msg << ' ';
	std::cout << std::endl;
	std::cout << "--------------------------------------------------" << std::endl; 

	std::cout << "-----------------test3：fill/swap-----------------" << std::endl;
	b.fill(101);										print(b, 'b');
	array<int, 5>c;
	c.swap(b);											print(c, 'c');
	std::cout << "--------------------------------------------------" << std::endl;

	std::cout << "}" << std::endl << std::endl;

	return 0;
}