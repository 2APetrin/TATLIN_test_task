#include <iostream>
#include <fstream>
#include <ostream>
#include <array>
#include <memory>

int main() {
    std::ofstream file("test.tape", std::ofstream::binary | std::fstream::in | std::fstream::out);

    std::cout << "cur pos=" << file.cur << std::endl;

    file.seekp(sizeof(int));

    std::cout << "cur pos=" << file.tellp() << std::endl;
    std::cout << "cur pos=" << file.cur << std::endl;


    auto arr = std::make_unique<int[]>(5);

    arr[0] = 1; arr[1] = 2; arr[2] = 3; arr[3] = 0; arr[4] = -1;

    if (!file.is_open()) { std::cout << "koknulo\n"; return 0; }

    file.write(reinterpret_cast<char*>(arr.get()), 5 * sizeof(int));

    std::cout << "pos=" << file.tellp();
    file.seekp(-sizeof(int), file.cur);
    std::cout << "pos=" << file.tellp();

    file.close();

    return 0;
}