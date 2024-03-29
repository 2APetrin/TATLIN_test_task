#include "tape.hpp"

int main(int argc, char *argv[]) {
    std::cout << argv[1] << std::endl;

    tape_simulation::tape<int> tp{std::string(argv[1])};

    tp.dump();

    tp.move_next();
    tp.write_elem(14892348);

    tp.dump();

    return 0;
}