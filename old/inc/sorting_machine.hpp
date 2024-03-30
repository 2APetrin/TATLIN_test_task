#pragma once

#include "tape.hpp"
#include "simulation_settings.hpp"

#include <filesystem>
#include <algorithm>
#include <vector>
#include <iterator>


namespace tape_simulation {

namespace fs = std::filesystem;

template<typename T>
class sorting_machine final {
    tape<T> in_tape,     out_tape,
            fst_subtape, snd_subtape;

    std::vector<T> ram_;

    machine_settings settings_;

    double simulation_timer = 0;

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

    void sort() {
        std::vector<int> fst_ptrs;
        std::vector<int> snd_ptrs;

        first_iteration(fst_ptrs, snd_ptrs);

        fst_subtape.dump();
        snd_subtape.dump();

        for (auto && i : fst_ptrs) std::cout << i << std::endl;
        for (auto && i : snd_ptrs) std::cout << i << std::endl;

        merge_subtapes(fst_ptrs, snd_ptrs);

        std::cout << "OUT TAPE:" << std::endl;
        out_tape.dump();

        std::cout << "timer=" << simulation_timer << std::endl;

        simulation_timer += in_tape.rewind_begin()     * settings_.time_rewind();
        simulation_timer += fst_subtape.rewind_begin() * settings_.time_rewind();
        simulation_timer += snd_subtape.rewind_begin() * settings_.time_rewind();


        fst_ptrs.clear();
        snd_ptrs.clear();

        separate_to_subtapes(&fst_ptrs, &snd_ptrs);
    }

private:
    void merge_subtapes(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs) {
        simulation_timer += fst_subtape.rewind_begin() * settings_.time_rewind;
        simulation_timer += snd_subtape.rewind_begin() * settings_.time_rewind;

        auto fst_it  = fst_ptrs.begin(), snd_it  = snd_ptrs.begin(),
             fst_end = fst_ptrs.end(),   snd_end = snd_ptrs.end();

        while (fst_it != fst_end && snd_it != snd_end) {
            merge_subsequences(*fst_it, *snd_it);
            ++fst_it; ++snd_it;
        }

        if (fst_it != fst_end) {
            int last = *std::prev(fst_end);
            std::cout << "last=" << last << std::endl;
            redirect_tape(fst_subtape, last+1, out_tape);
        }
        else if (snd_it != snd_end) {
            int last = *std::prev(snd_end);
            std::cout << "last=" << last << std::endl;
            redirect_tape(snd_subtape, last+1, out_tape);
        }
    }

    void redirect_tape(tape<int> &in, int pos, tape<int> &out) {
        int curr = in.position();
        int tmp{};
        while (curr++ != pos) {
            in.read_elem(tmp);   simulation_timer += settings_.time_read;
            in.move_next();      simulation_timer += settings_.time_move;

            out.write_elem(tmp); simulation_timer += settings_.time_write;
            out.move_next();     simulation_timer += settings_.time_move ;
        }
    }

    void merge_subsequences(int fst, int snd) {
        int fst_pos = fst_subtape.position();
        int snd_pos = snd_subtape.position();

        int tmp1{}, tmp2{};

        while (fst_pos != fst+1 && snd_pos != snd+1) {
            fst_subtape.read_elem(tmp1);   simulation_timer += settings_.time_read;
            snd_subtape.read_elem(tmp2);   simulation_timer += settings_.time_read;

            if (tmp1 <= tmp2) {
                out_tape.write_elem(tmp1); simulation_timer += settings_.time_write;
                out_tape.move_next();      simulation_timer += settings_.time_move ;

                ++fst_pos; fst_subtape.move_next(); simulation_timer += settings_.time_move;
            }
            else {
                out_tape.write_elem(tmp2); simulation_timer += settings_.time_write;
                out_tape.move_next();      simulation_timer += settings_.time_move ;

                ++snd_pos; snd_subtape.move_next(); simulation_timer += settings_.time_move;
            }
        }

        if (fst_pos != fst+1) {
            while (fst_pos != fst+1) {
                fst_subtape.read_elem(tmp1); simulation_timer += settings_.time_read;
                fst_subtape.move_next();     simulation_timer += settings_.time_move;
                ++fst_pos;

                out_tape.write_elem(tmp1);   simulation_timer += settings_.time_write;
                out_tape.move_next();        simulation_timer += settings_.time_move ;
            }
        }
        else {
            while (snd_pos != snd+1) {
                snd_subtape.read_elem(tmp2); simulation_timer += settings_.time_read;
                snd_subtape.move_next();     simulation_timer += settings_.time_move;
                ++snd_pos;

                out_tape.write_elem(tmp2);   simulation_timer += settings_.time_write;
                out_tape.move_next();        simulation_timer += settings_.time_move ;
            }
        }
    }

    void separate_to_subtapes(std::vector<int> *fst_ptrs, std::vector<int> *snd_ptrs) {
        bool decision = true;
        int  sz       = out_tape.size();
        int  pos      = 0;
        int  prev_pos = 0;

        int tmp_prev{};
        out_tape.read_elem(tmp_prev); simulation_timer += settings_.time_read;
        out_tape.move_next(); ++pos;  simulation_timer += settings_.time_move ;

        int tmp{};
        while (pos != in_sz) {
            out_tape.read_elem(tmp);

            if (tmp < tmp_prev) {
                decision ? fst_ptrs.push_back(pos-1 - prev_pos) : snd_ptrs.push_back(pos-1 - prev_pos);

            }
        }
    }

    void first_iteration(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs) {
        bool decision = true;
        int  len      = 0;
        int  fst_pos  = 0;
        int  snd_pos  = 0;

        while (0 != (len = read_ram())) {
            decision ? fst_pos+=len : snd_pos+=len;
            std::sort(ram_.begin(), ram_.begin() + len);

            //std::cout << "len=" << len <<std::endl;

            write_ram((decision ? fst_subtape : snd_subtape), len);
            decision ? fst_ptrs.push_back(fst_pos-1) : snd_ptrs.push_back(snd_pos-1);
            decision = !decision;
        }
    }

    int read_ram() {
        int len = in_tape.read_buffer(ram_.begin(), ram_.end());
        simulation_timer += len * (settings_.time_read + settings_.time_move);

        return len;
    }

    int write_ram(tape<int> &tp, int cnt) {
        int len = tp.write_buffer(ram_.begin(), ram_.begin() + cnt);
        simulation_timer += len * (settings_.time_write + settings_.time_move);

        return len;
    }
};
} // <--- namespace tape_simulation