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
    tape<T> in_,  out_,
            fst_, snd_;

    std::vector<T> ram_;

    sim_timer tmr_;

    std::string fst_sub = "/tmp/fst_subtape.tape";
    std::string snd_sub = "/tmp/snd_subtape.tape";

public:
    sorting_machine(std::string in_path, std::string out_path, machine_settings &settings) : in_(in_path), tmr_(settings, sizeof(T)) {
        auto project_folder = fs::absolute(__FILE__).parent_path().parent_path();

        /* creating/clearing files */
        std::ofstream stream;
        stream.open(project_folder.string() + fst_sub); stream.close();
        stream.open(project_folder.string() + snd_sub); stream.close();
        stream.open(out_path);                          stream.close();

        out_.open_tape(out_path);

        /* creating tapes on files */
        fst_.open_tape(project_folder.string() + fst_sub);
        snd_.open_tape(project_folder.string() + snd_sub);

        ram_.resize(settings.ram_size_elems / sizeof(T));
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

        // fst_.dump();
        // snd_.dump();

        while (!merge_subtapes(fst_ptrs, snd_ptrs)) {

            // std::cout << "fst_ptrs" << std::endl;
            // for (auto && i : fst_ptrs) std::cout << i << std::endl;
            // std::cout << "snd_ptrs" << std::endl;
            // for (auto && i : snd_ptrs) std::cout << i << std::endl;

            // fst_.dump();
            // snd_.dump();

            tmr_.rewind(in_. rewind_begin());
            tmr_.rewind(out_.rewind_begin());
            tmr_.rewind(fst_.rewind_begin());
            tmr_.rewind(snd_.rewind_begin());

            fst_ptrs.clear();
            snd_ptrs.clear();

            //std::cout << "cock\n";
            separate_to_subtapes(fst_ptrs, snd_ptrs);

            tmr_.rewind(out_.rewind_begin());
            tmr_.rewind(fst_.rewind_begin());
            tmr_.rewind(snd_.rewind_begin());
        }

        // std::cout << "OUT TAPE:" << std::endl;
        // out_.dump();

        std::cout << "time=" << tmr_.time() << std::endl;
    }

private:
    void separate_to_subtapes(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs) {
        bool decision = true;
        int  sz       = out_.size();

        int  pos      = out_.pos();
        int  prev_pos = out_.pos();

        int fst_pos = 0;
        int snd_pos = 0;

        auto curr_tape = &fst_;

        int tmp{};
        int prev_tmp{};

        out_.read_elem(prev_tmp); tmr_.read();
        out_.move_next();         tmr_.move();
        ++pos;

        curr_tape->write_elem(prev_tmp); tmr_.write();
        curr_tape->move_next();          tmr_.move();

        while (pos != sz) {
            out_.read_elem(tmp); tmr_.read();
            out_.move_next();    tmr_.move();

            if (tmp < prev_tmp) {
                if (decision) {
                    fst_pos += pos - prev_pos;
                    fst_ptrs.push_back(fst_pos-1);
                    prev_pos = pos;

                    curr_tape = &snd_;
                    curr_tape->write_elem(tmp); tmr_.write();
                    curr_tape->move_next();     tmr_.move();

                    prev_tmp = tmp;

                    decision = !decision;
                }
                else {
                    snd_pos += pos - prev_pos;
                    snd_ptrs.push_back(snd_pos-1);
                    prev_pos = pos;

                    curr_tape = &fst_;
                    curr_tape->write_elem(tmp); tmr_.write();
                    curr_tape->move_next();     tmr_.move();

                    prev_tmp = tmp;

                    decision = !decision;
                }
            }
            else {
                curr_tape->write_elem(tmp); tmr_.write();
                curr_tape->move_next();     tmr_.move();
                prev_tmp = tmp;
            }

            ++pos;
        }

        if (decision) {
            fst_pos += pos - prev_pos;
            fst_ptrs.push_back(fst_pos-1);
        }
        else {
            snd_pos += pos - prev_pos;
            snd_ptrs.push_back(snd_pos-1);
        }
    }

    bool merge_subtapes(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs) {
        tmr_.rewind(fst_.rewind_begin());
        tmr_.rewind(snd_.rewind_begin());

        auto fst_it  = fst_ptrs.begin(), snd_it  = snd_ptrs.begin(),
             fst_end = fst_ptrs.end(),   snd_end = snd_ptrs.end();

        bool ret = true;

        while (fst_it != fst_end && snd_it != snd_end) {
            merge_subsequences(*fst_it, *snd_it);
            ++fst_it; ++snd_it;

            int tmp{};
            out_.move_prev();    tmr_.move();
            out_.read_elem(tmp); tmr_.read();
            out_.move_next();    tmr_.move();

            int f{}, s{};
            fst_.read_elem(f);   tmr_.read();
            snd_.read_elem(s);   tmr_.read();
            if (tmp > std::min(f, s)) ret = false;
        }

        if (fst_it != fst_end) {
            if (out_.pos() != 0) {
                int tmp{};
                out_.move_prev();    tmr_.move();
                out_.read_elem(tmp); tmr_.read();
                out_.move_next();    tmr_.move();

                int f{};
                fst_.read_elem(f);   tmr_.read();

                if (tmp > f) ret = false;
            }

            int last = *std::prev(fst_end);
            redirect_tape(fst_, last+1, out_);
        }
        else if (snd_it != snd_end) {
            if (out_.pos() != 0) {
                int tmp{};
                out_.move_prev();    tmr_.move();
                out_.read_elem(tmp); tmr_.read();
                out_.move_next();    tmr_.move();

                int s{};
                snd_.read_elem(s);   tmr_.read();

                if (tmp > s) ret = false;
            }

            int last = *std::prev(snd_end);
            redirect_tape(snd_, last+1, out_);
        }

        return ret;
    }

    void redirect_tape(tape<int> &in, int pos, tape<int> &out) {
        int curr = in.pos();
        int tmp{};
        while (curr++ != pos) {
            in.read_elem(tmp); tmr_.read();
            in.move_next();    tmr_.move();

            out.write_elem(tmp); tmr_.write();
            out.move_next();     tmr_.move();
        }
    }

    void merge_subsequences(int fst, int snd) {
        int fst_pos = fst_.pos();
        int snd_pos = snd_.pos();

        int tmp1{}, tmp2{};

        while (fst_pos != fst+1 && snd_pos != snd+1) {
            fst_.read_elem(tmp1); tmr_.read();
            snd_.read_elem(tmp2); tmr_.read();

            if (tmp1 <= tmp2) {
                out_.write_elem(tmp1); tmr_.write();
                out_.move_next();      tmr_.move();

                ++fst_pos; fst_.move_next(); tmr_.move();
            }
            else {
                out_.write_elem(tmp2); tmr_.write();
                out_.move_next();      tmr_.move();

                ++snd_pos; snd_.move_next(); tmr_.move();
            }
        }

        if (fst_pos != fst+1) {
            while (fst_pos != fst+1) {
                fst_.read_elem(tmp1); tmr_.read();
                fst_.move_next();     tmr_.move();
                ++fst_pos;

                out_.write_elem(tmp1); tmr_.write();
                out_.move_next();      tmr_.move();
            }
        }
        else {
            while (snd_pos != snd+1) {
                snd_.read_elem(tmp2); tmr_.read();
                snd_.move_next();     tmr_.move();
                ++snd_pos;

                out_.write_elem(tmp2); tmr_.write();
                out_.move_next();      tmr_.move();
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

            write_ram((decision ? fst_ : snd_), len);
            decision ? fst_ptrs.push_back(fst_pos-1) : snd_ptrs.push_back(snd_pos-1);
            decision = !decision;
        }
    }

    int read_ram() {
        int len = in_.read_buffer(ram_.begin(), ram_.end());
        tmr_.move(len);
        tmr_.read(len);

        return len;
    }

    int write_ram(tape<int> &tp, int cnt) {
        int len = tp.write_buffer(ram_.begin(), ram_.begin() + cnt);
        tmr_.move (len);
        tmr_.write(len);

        return len;
    }
};

} // <--- namespace tape_simulation