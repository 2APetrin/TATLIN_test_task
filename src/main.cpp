#include "simulation_settings.hpp"
#include "sorting_machine.hpp"
#include "main_func.hpp"

namespace {
    namespace fs  = std::filesystem;
    namespace ts  = tape_simulation;
    namespace tsd = tape_simulation::detail;
    namespace md  = main_details;

    constexpr int input_file_num  = 1;
    constexpr int output_file_num = 2;

    std::string in_bin  = "in.bin";
    std::string out_bin = "out.bin";
}


int main(int argc, char *argv[]) {
    if (md::arg_check(argc)) return 1;

    auto tmp_path = fs::temp_directory_path();

    ts::machine_settings settings;
    if (md::process_settings(settings, argc, argv)) return 1;

    ts::sorting_machine<int> mchn(settings);

    std::string pid_str = std::to_string(getpid());
    auto bin_in  = fs::path(tmp_path / (pid_str + in_bin ));
    auto bin_out = fs::path(tmp_path / (pid_str + out_bin));

    if (md::convert_int_to_binary(argv[input_file_num], bin_in)) return 1;

    ts::tape<int> in(bin_in);
    ts::tape<int> out;
    out.open_tape(tmp_path / (pid_str + out_bin));

    mchn.sort(in, out);

    std::cout << "simulation timer = " << mchn.time() << std::endl;

    if (md::convert_binary_to_int(argv[output_file_num], bin_out)) return 1;

    fs::remove(bin_in);
    fs::remove(bin_out);

    return 0;
}
