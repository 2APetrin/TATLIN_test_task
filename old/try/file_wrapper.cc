#include "file_wrapper.hpp"
#include <memory>
#include <fstream>

int main() {
    //tape_simulation::tape tape{};

    std::fstream file("test.tape", std::fstream::binary | std::fstream::in | std::fstream::out);

    if (!file.is_open()) {
        std::cout << "Cannot open file" << std::endl;
        return 1;
    }

    auto arr = std::make_unique<int[]>(5);

    int A = 0xDAED0000;

    // file.seekp(sizeof(int));
    // file.write(reinterpret_cast<char*>(&A), sizeof(int));
    // file.seekg(0, file.beg);
    // file.seekg(sizeof(int), file.cur);
    // file.seekp(sizeof(int), file.cur); // they call rdbuf()->pubseekoff() for the same buffer so no need to call seekp to move put pointer

    file.read(reinterpret_cast<char*>(arr.get()), 5 * sizeof(int));

    for (int i = 0; i < 5; ++i) std::cout << arr[i] << std::endl;

    file.close();

    return 0;
}