#include <iostream>
#include <algorithm>
#include <vector>

int main() {
    std::vector<int> vec;
    int elem{};
    while ((std::cin >> elem)) vec.push_back(elem);

    std::sort(vec.begin(), vec.end());

    for (auto && i : vec) std::cout << i << std::endl;

    return 0;
}