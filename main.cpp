#include <iostream>
#include <string_view>
#include <windows.h>
 
int main()
{
    std::string_view s = "Hello world!\n";
    std::cout << s;
    MessageBoxW(NULL, L"My message", L"My title", MB_OK);
    getchar();
    return 0;
}
