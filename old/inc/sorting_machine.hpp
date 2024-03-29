#pragma once

#include "tape.hpp"
#include "settings.hpp"

#include <filesystem>
#include <vector>
#include <unordered_map>

namespace tape_simulation {

namespace fs = std::filesystem;

template<typename T>
class sorting_machine final {
    tape<T> in_tape,     out_tape,
            fst_subtape, snd_subtape;

    std::vector<T> ram_;

    machine_settings settings_;

public:
    sorting_machine(std::string in_path, std::string out_path, machine_settings &settings) : in_tape(in_path), out_tape(out_path), settings_(settings) {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        std::ofstream stream;
        stream.open(project_folder.string() + "/tmp/fst_subtape.tape"); stream.close();
        stream.open(project_folder.string() + "/tmp/snd_subtape.tape"); stream.close();

        fst_subtape.open_tape(project_folder.string() + "/tmp/fst_subtape.tape");
        snd_subtape.open_tape(project_folder.string() + "/tmp/snd_subtape.tape");

        settings_.ram_size_elems = settings_.ram_size_elems / sizeof(T);

        ram_.reserve(settings_.ram_size_elems);

        // std::cout << settings_.ram_size_elems << std::endl;
        // std::cout << settings_.time_read << std::endl;
        // std::cout << settings_.time_write << std::endl;
        // std::cout << settings_.time_move << std::endl;
        // std::cout << settings_.time_rewind << std::endl;
    }

    void sort() { }

//------------------Rule of five------------------
    ~sorting_machine() {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        fs::remove(project_folder.string() + "/tmp/fst_subtape.tape");
        fs::remove(project_folder.string() + "/tmp/snd_subtape.tape");
    }

    sorting_machine(sorting_machine&)  = delete;
    sorting_machine(sorting_machine&&) = delete;

    sorting_machine& operator=(const sorting_machine& ) = delete;
    sorting_machine& operator=(      sorting_machine&&) = delete;
//------------------------------------------------



};

} // <--- tape_simulation