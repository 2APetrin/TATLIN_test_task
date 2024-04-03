#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "sorting_machine.hpp"
#include "main_func.hpp"
#include "tape.hpp"

#include "tape_exceptions.hpp"
#include "simulation_exceptions.hpp"


namespace fs  = std::filesystem;
namespace ts  = tape_simulation;
namespace tsd = tape_simulation::detail;
namespace md  = main_details;

class tape_test : public testing::Test {
protected:
    tape_simulation::tape<int> tp;

    void SetUp() override {
        fs::path file_folder = fs::absolute(__FILE__).parent_path();
        tp.open_tape(file_folder / "google_test" / "tape.bin");

        tp.write_elem(1); tp.move_next();
        tp.write_elem(2); tp.move_next();
        tp.write_elem(3); tp.move_next();

        tp.rewind_begin();
    }
};


TEST_F(tape_test, test_1) {
    EXPECT_THROW(tp.move_prev(), tape_exceptions::move_prev_out_of_range);
}

TEST_F(tape_test, test_2) {
    for (int i = 0; i < 3; ++i) tp.move_next();
    EXPECT_THROW(tp.move_next(), tape_exceptions::move_next_out_of_range);
}

TEST_F(tape_test, test_3) {
    for (int i = 0; i < 3; ++i) tp.move_next();
    EXPECT_THROW(tp.open_tape("123"), tape_exceptions::tape_already_initialized);
}

TEST_F(tape_test, test_4) {
    for (int i = 0; i < 3; ++i) tp.move_next();
    int i{};
    EXPECT_EQ(tp.read_elem(i), false);
}

TEST(basic_tape, test5) {
    EXPECT_THROW(tape_simulation::tape<int> tp("aljksdhgfajdhls"), tape_exceptions::cannot_open_file);
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//


TEST(sorting_machine_test, test_1) {
    ts::machine_settings settings{4, 1, 1, 1, 1};
    ts::sorting_machine<int> mchn(settings);
    EXPECT_NEAR(mchn.time(), 0, 0.00001);
}

TEST(sorting_machine_test, test_2) {
    ts::machine_settings settings{1, 1, 1, 1, 1};
    EXPECT_THROW(ts::sorting_machine<int> mchn(settings), simulation_exceptions::config_file_troubles);
}

TEST(sorting_machine_test, test_3) {
    ts::machine_settings settings{4, 0, 1, 1, 1};
    EXPECT_NO_THROW(ts::sorting_machine<int> mchn(settings));
}

TEST(sorting_machine_test, test_4) {
    ts::machine_settings settings{4, -1, 1, 1, 1};
    EXPECT_THROW(ts::sorting_machine<int> mchn(settings), simulation_exceptions::config_file_troubles);
}

TEST(sorting_machine_test, test_5) {
    ts::machine_settings settings{4, 1, 1, 1, 1};
    ts::sorting_machine<int> mchn(settings);

    tape_simulation::tape<int> a;
    tape_simulation::tape<int> b;
    EXPECT_THROW(mchn.sort(a, b), simulation_exceptions::not_valid_tapes);
}

TEST(sorting_machine_test, test_6) {
    ts::machine_settings settings{4, 1, 1, 1, 1};
    ts::sorting_machine<int> mchn(settings);

    tape_simulation::tape<int> a;
    EXPECT_THROW(mchn.sort(a, a), simulation_exceptions::not_valid_tapes);
}

TEST(sorting_machine_test, test_7) {
    ts::machine_settings settings{4, 1, 1, 1, 1};
    ts::sorting_machine<int> mchn(settings);

    fs::path file_folder = fs::absolute(__FILE__).parent_path();

    tape_simulation::tape<int> a;
    a.open_tape(file_folder / "google_test" / "tape.bin");

    EXPECT_THROW(mchn.sort(a, a), simulation_exceptions::sotring_the_same_tape);
}


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
