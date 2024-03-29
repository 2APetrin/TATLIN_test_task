#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include "tape.hpp"

#include "sorting_machine.hpp"

using namespace tape_simulation;
namespace fs = std::filesystem;

TEST(basic, test_1) {
    auto file_folder = fs::absolute(__FILE__).parent_path();
    tape<int> tp(file_folder.string() + "/test.bin");
    tp.clear_tape();

    tp.write_elem(1);
    tp.write_elem(2);
    tp.write_elem(3);

    try {
        tp.move_next();
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    tp.dump();
}

TEST(machine, test_1) {
    auto file_folder = fs::absolute(__FILE__).parent_path();
    sorting_machine<int> mchn(file_folder.string() + "/test.bin",
                              file_folder.string() + "/test_out.bin");
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}