#include "simulation_settings.hpp"
#include "sorting_machine.hpp"
#include "main_func.hpp"

namespace {
    std::string config_pth = "/config.txt";

    namespace fs = std::filesystem;
    namespace ts = tape_simulation;

    std::string bin_in  = "/tmp/in.bin" ;
    std::string bin_out = "/tmp/out.bin";
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }

    auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

    ts::machine_settings settings;
    if (main_details::process_settings(settings, project_folder.string() + config_pth)) return 1;

    if (main_details::convert_int_to_binary(argv[1], project_folder.string() + bin_in)) return 1;

    ts::sorting_machine<int> mchn(project_folder.string() + bin_in, project_folder.string() + bin_out, settings);

    mchn.sort();

    if (main_details::convert_binary_to_int(argv[2], project_folder.string() + bin_out)) return 1;

    fs::remove(project_folder.string() + bin_in);
    fs::remove(project_folder.string() + bin_out);

    return 0;
}
