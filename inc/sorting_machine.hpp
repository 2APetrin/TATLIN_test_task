#pragma once

#include "tape.hpp"
#include "simulation_settings.hpp"

#include <filesystem>
#include <algorithm>
#include <vector>

namespace tape_simulation {

namespace fs = std::filesystem;

template<typename T>
class sorting_machine final {
    tape<T> in_tape,     out_tape,
            fst_subtape, snd_subtape;

    std::vector<T> ram_;

    machine_settings settings_;

    int simulation_timer = 0;

    std::string fst_sub = "/tmp/fst_subtape.tape";
    std::string snd_sub = "/tmp/snd_subtape.tape";

public:
    sorting_machine(std::string in_path, std::string out_path, machine_settings &settings) : in_tape(in_path), settings_(settings) {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        /* creating/clearing files */
        std::ofstream stream;
        stream.open(project_folder.string() + fst_sub); stream.close();
        stream.open(project_folder.string() + snd_sub); stream.close();
        stream.open(out_path);                          stream.close();

        out_tape.open_tape(out_path);

        /* creating tapes on files */
        fst_subtape.open_tape(project_folder.string() + fst_sub);
        snd_subtape.open_tape(project_folder.string() + snd_sub);

        settings_.ram_size_elems = settings_.ram_size_elems / sizeof(T);

        ram_.resize(settings_.ram_size_elems);

        // std::cout << settings_.ram_size_elems << std::endl;
        // std::cout << settings_.time_read      << std::endl;
        // std::cout << settings_.time_write     << std::endl;
        // std::cout << settings_.time_move      << std::endl;
        // std::cout << settings_.time_rewind    << std::endl;
    }

//------------------Rule of five------------------
    ~sorting_machine() {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        fs::remove(project_folder.string() + fst_sub);
        fs::remove(project_folder.string() + snd_sub);
    }

    sorting_machine(sorting_machine&)  = delete;
    sorting_machine(sorting_machine&&) = delete;

    sorting_machine& operator=(const sorting_machine& ) = delete;
    sorting_machine& operator=(      sorting_machine&&) = delete;
//------------------------------------------------

    int sort() {
        read_ram();

        std::cout << simulation_timer << std::endl;

        std::sort(ram_.begin(), ram_.end());

        std::copy(ram_.begin(), ram_.end(), std::ostream_iterator<int>(std::cout, " "));

        return 0;
    }

private:
    void read_ram() {
        int len = in_tape.read_buffer(ram_.begin(), ram_.end());
        simulation_timer += len * (settings_.time_read + settings_.time_move);
    }
};
} // <--- namespace tape_simulation