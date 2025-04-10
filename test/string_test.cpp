#include <string>
#include <iostream>
#include "string.hpp"
using namespace oct;
template <typename Container>
void print(Container& container, const char ch = 0) {
    if (ch)std::wcout << ch << ':';
    if (container.empty())std::wcout << "EMPTY";
    else for (const auto& elem : container)
        std::wcout << elem;
    std::wcout << '\n';
}

int string_test(){
    /*
	string a = "asdfghjqwertyuiopasdfghjkl,zxcvbnm;";print(a);
    wstring b = L"我是顶针"; print(b);
    b.assign(L"我礼堂王"); print(b);
    wstring c = b[{1, 4}]; print(c);
    */
    return 0;
}