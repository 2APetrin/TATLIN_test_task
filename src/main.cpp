#include "simulation_settings.hpp"
#include "sorting_machine.hpp"
#include "main_func.hpp"

namespace {
    namespace fs  = std::filesystem;
    namespace ts  = tape_simulation;
    namespace tsd = tape_simulation::detail;
    namespace md  = main_details;
}


int main(int argc, char *argv[]) try {
    if (md::arg_check(argc)) return 1;

    ts::machine_settings settings;
    if (md::process_settings(settings, argc, argv)) return 1;

    ts::sorting_machine<int> mchn(settings);

    fs::path bin_in;
    fs::path bin_out;

    if (md::process_paths(bin_in, bin_out, argv)) return 1;

    ts::tape<int> in(bin_in);
    ts::tape<int> out;
    out.open_tape(bin_out);

    mchn.sort(in, out);

    std::cout << "simulation timer = " << mchn.time() << std::endl;

    if (md::process_out_files(argv, bin_in, bin_out)) return 1;

    return 0;
}
catch (std::exception &exception) {
    std::cerr << "Programm error: " << exception.what() << std::endl;
    return 1;
}