#include "sorting_machine.hpp"
#include "tape.hpp"

#include <unordered_map>
#include "settings.hpp"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Wrong number of arguments" << std::endl;
        return 1;
    }

    auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();
    std::ifstream settings_buff(project_folder.string() + "/config.txt");

    if (!settings_buff.is_open()) {
        std::cerr << "Cannot open settings_buff file" << std::endl;
        return 1;
    }

    std::string setting;
    int         value;

    std::unordered_map<std::string, int> map;

    while ((settings_buff >> setting) && (settings_buff >> value))
        map.emplace(setting, value);

    std::cout << "\nMAP:\n";
    for (auto && i : map) {
        std::cout << i.first << " - " << i.second << std::endl;
    }

    tape_simulation::machine_settings settings;
    auto it = map.find("ram_size_byte"); if (it != map.end()) { settings.ram_size_elems = it->second; };
         it = map.find("time_read");     if (it != map.end()) { settings.time_read      = it->second; };
         it = map.find("time_write");    if (it != map.end()) { settings.time_write     = it->second; };
         it = map.find("time_move");     if (it != map.end()) { settings.time_move      = it->second; };
         it = map.find("time_rewind");   if (it != map.end()) { settings.time_rewind    = it->second; };

    if (!settings.is_valid()) {
        std::cerr << "Bad config file" << std::endl;
        return 1;
    }

    tape_simulation::sorting_machine<int> mchn(project_folder.string() + "/test/test.bin",
                                               project_folder.string() + "/test/test_out.bin",
                                               settings);

    return 0;
}