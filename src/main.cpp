#include "simulation_settings.hpp"
#include "sorting_machine.hpp"
#include "main_func.hpp"

#include <string_view>
#include <chrono>


namespace {
    namespace fs  = std::filesystem;
    namespace ts  = tape_simulation;
    namespace tsd = tape_simulation::detail;

    constexpr int default_config_file = 3;
    constexpr int custom_config_file  = 4;

    constexpr std::string_view in_bin  = "in.bin";
    constexpr std::string_view out_bin = "out.bin";
    constexpr std::string_view config  = "config.txt";
}


int main(int argc, char *argv[]) {
    if (argc < default_config_file || argc > custom_config_file) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }

    auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

    tsd::machine_settings settings;

    if (argc == default_config_file) {
        if (main_details::process_settings(settings, project_folder / config)) return 1;
    }
    else if (argc == custom_config_file) {
        if (main_details::process_settings(settings, argv[custom_config_file-1])) return 1;
    }

    auto tmp_path = fs::temp_directory_path();

    if (main_details::convert_int_to_binary(argv[1], tmp_path / in_bin)) return 1;

#ifdef TAPE_DUMP_MODE
    auto start = std::chrono::high_resolution_clock::now();
#endif // TAPE_DUMP_MODE

    ts::sorting_machine<int> mchn(settings);

    ts::tape<int> in(tmp_path / in_bin);
    ts::tape<int> out; out.open_tape(tmp_path / out_bin);

    mchn.sort(in, out);

#ifdef TAPE_DUMP_MODE
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

    std::cout << (double)time / 1000000000 << " sec" << std::endl;
#endif // TAPE_DUMP_MODE

    std::cout << "timer=" << mchn.time() << std::endl;

    if (main_details::convert_binary_to_int(argv[default_config_file-1], tmp_path / out_bin)) return 1;

    fs::remove(tmp_path / in_bin );
    fs::remove(tmp_path / out_bin);

    return 0;
}
