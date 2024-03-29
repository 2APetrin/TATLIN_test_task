#include <iostream>
#include <fstream>

struct foo final {
    std::fstream stream_;

    foo() = default;
};

int main() {
    std::fstream strm;

    std::cout << strm.is_open() << std::endl;
    strm.close();
    std::cout << strm.is_open() << std::endl;

    foo A;
    std::cout << A.stream_.is_open() << std::endl;
    A.stream_.close();
    std::cout << A.stream_.is_open() << std::endl;


    return 0;
}