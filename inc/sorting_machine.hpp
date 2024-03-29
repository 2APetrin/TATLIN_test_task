#pragma once

#include "tape.hpp"
#include <filesystem>

namespace tape_simulation {

namespace fs = std::filesystem;

template<typename T>
class sorting_machine final {
    tape<T> in_tape,     out_tape,
            fst_subtape, snd_subtape;

    std::vector<int> ram_;

public:
    sorting_machine(std::string in_path, std::string out_path, int ram_size) : in_tape(in_path), out_tape(out_path) {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        std::ofstream stream;
        stream.open(project_folder.string() + "/tmp/fst_subtape.tape"); stream.close();
        stream.open(project_folder.string() + "/tmp/snd_subtape.tape"); stream.close();

        fst_subtape.open_tape(project_folder.string() + "/tmp/fst_subtape.tape");
        snd_subtape.open_tape(project_folder.string() + "/tmp/snd_subtape.tape");

        ram_.reserve(ram_size);
    }

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