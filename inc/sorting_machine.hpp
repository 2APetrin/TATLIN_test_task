#pragma once

#include "tape.hpp"
#include "simulation_settings.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
#include <type_traits>

#include <sstream>
#include <thread>


namespace tape_simulation {

namespace detail {
    std::string fst_sub = "fst_sub.bin";
    std::string snd_sub = "snd_sub.bin";
} // <--- namespace detail


namespace fs = std::filesystem;

template<typename T>
//requires std::is_arithmetic_v<T>
class sorting_machine final {
    tape<T> fst_, snd_;

    std::vector<T> ram_;

    detail::sim_timer tmr_;
public:
    sorting_machine(detail::machine_settings &settings) : tmr_(settings, sizeof(T)) {
        auto tmp_path = fs::temp_directory_path();

        fst_.open_tape(tmp_path / detail::fst_sub);
        snd_.open_tape(tmp_path / detail::snd_sub);

        ram_.resize(settings.ram_size_elems / sizeof(T));

        // std::stringstream ss;

        // ss << std::this_thread::get_id();

        // std::cout << ss << std::endl;
    }

//------------------Rule of five------------------
    ~sorting_machine() {
        auto tmp_path = fs::temp_directory_path();

        fs::remove(tmp_path / detail::fst_sub);
        fs::remove(tmp_path / detail::snd_sub);
    }

    sorting_machine(sorting_machine&)  = delete;
    sorting_machine(sorting_machine&&) = delete;

    sorting_machine& operator=(const sorting_machine& ) = delete;
    sorting_machine& operator=(      sorting_machine&&) = delete;
//------------------------------------------------

    void sort(tape<T> &in, tape<T> &out) {
        std::vector<int> fst_ptrs;
        std::vector<int> snd_ptrs;

        first_sort_iteration(fst_ptrs, snd_ptrs, in);

        int i = 1;

        while (!merge_subtapes(fst_ptrs, snd_ptrs, out)) {
            tmr_.rewind(in. rewind_begin());
            tmr_.rewind(out.rewind_begin());
            tmr_.rewind(fst_.rewind_begin());
            tmr_.rewind(snd_.rewind_begin());

            fst_ptrs.clear();
            snd_ptrs.clear();

            separate_to_subtapes(fst_ptrs, snd_ptrs, out);

            tmr_.rewind(out.rewind_begin());
            i++;
        }

        std::cout << "merges=" << i << std::endl;
    }

    double time() { return tmr_.time(); }

private:
    void first_sort_iteration(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs, tape<T> &in) {
        bool decision = true;

        int  fst_pos  = 0;
        int  snd_pos  = 0;
        int  len      = 0;

        while (0 != (len = read_ram(in))) {
            std::sort(ram_.begin(), ram_.begin() + len);

            write_ram((decision ? fst_ : snd_), len);

            if (decision) {
                fst_pos+=len;
                fst_ptrs.push_back(fst_pos-1);
            }
            else {
                snd_pos+=len;
                snd_ptrs.push_back(snd_pos-1);
            }

            decision = !decision;
        }
    }

    int read_ram(tape<T> &tp) {
        int len = tp.read_buffer(ram_.begin(), ram_.end());
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

    bool merge_subtapes(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs, tape<T> &out) {
        tmr_.rewind(fst_.rewind_begin());
        tmr_.rewind(snd_.rewind_begin());

        auto fst_it  = fst_ptrs.begin(), snd_it  = snd_ptrs.begin(),
             fst_end = fst_ptrs.end(),   snd_end = snd_ptrs.end();

        bool ret = true;

        while (fst_it != fst_end && snd_it != snd_end) {
            T prev = merge_subsequences(*(fst_it++) + 1, *(snd_it++) + 1, out);

            T f = read(fst_),
              s = read(snd_);

            if (prev > std::min(f, s)) ret = false;
        }

        if (fst_it != fst_end) {
            if (merge_remainder(fst_, out, *std::prev(fst_end))) ret = false;
        }
        else if (snd_it != snd_end)
            if (merge_remainder(snd_, out, *std::prev(snd_end))) ret = false;

        return ret;
    }

    T merge_subsequences(int fst, int snd, tape<T> &out) {
        int fst_pos = fst_.pos();
        int snd_pos = snd_.pos();

        bool decision = false;
        T tmp1{}, tmp2{};

        tmp1 = read(fst_);

        while (fst_pos != fst && snd_pos != snd) {
            decision ? tmp1 = read(fst_) : tmp2 = read(snd_);

            if (tmp1 <= tmp2) {
                write_move(tmp1, out);
                move_next(fst_);

                ++fst_pos;
                decision = true;
            }
            else {
                write_move(tmp2, out);
                move_next(snd_);

                ++snd_pos;
                decision = false;
            }
        }

        if (fst_pos != fst) fst_pos += subseq_remainder_merge(fst_pos, fst, fst_, out);
        else                snd_pos += subseq_remainder_merge(snd_pos, snd, snd_, out);

        move_prev(out);
        return read_move(out);
    }

    int subseq_remainder_merge(int pos, int end, tape<T> &tp, tape<T> &out) {
        int ret = 0;

        T tmp{};
        while (pos++ != end) {
            tmp = read_move(tp);
            ++ret;

            write_move(tmp, out);
        }

        return ret;
    }

    T read_prev(tape<T> &tp) {
        move_prev(tp);

        return read_move(tp);
    }

    bool merge_remainder(tape<T> &tp, tape<T> &out, int last) {
        bool ret = false;

        if (out.pos() != 0) {
            T prev = read_prev(out);
            T f    = read(tp);

            if (prev > f) ret = true;
        }

        copy_tape(tp, last+1, out);

        return ret;
    }

    void copy_tape(tape<int> &in, int pos, tape<int> &out) {
        int curr = in.pos();
        T tmp{};

        while (curr++ != pos) {
            tmp = read_move(in);
            write_move(tmp, out);
        }
    }

    void separate_to_subtapes(std::vector<int> &fst_ptrs, std::vector<int> &snd_ptrs, tape<T> &out) {
        int sz  = out.size();
        int pos = out.pos();
        int len = 1;

        int fst_pos = fst_.pos();
        int snd_pos = snd_.pos();

        bool decision  = true;
        auto curr_tape = &fst_;

        T tmp{};
        T prev = read_move(out); ++pos;
        write_move(prev, *curr_tape);

        while (pos++ != sz) {
            tmp = read_move(out);

            if (tmp < prev) {
                if (decision) {
                    fst_pos += len;
                    fst_ptrs.push_back(fst_pos-1);
                    curr_tape = &snd_;
                }
                else {
                    snd_pos += len;
                    snd_ptrs.push_back(snd_pos-1);
                    curr_tape = &fst_;
                }

                write_move((prev = tmp), *curr_tape);

                decision = !decision;
                len = 1;
            }
            else {
                write_move((prev = tmp), *curr_tape);
                len++;
            }
        }

        if   (decision) fst_ptrs.push_back(fst_pos + len - 1);
        else            snd_ptrs.push_back(snd_pos + len - 1);
    }

//--------------------------------------------

    T read_move(tape<T> &tp) {
        T elem{};
        tp.read_elem(elem); tmr_.read();
        tp.move_next();     tmr_.move();

        return elem;
    }

    void write_move(T elem, tape<T> &tp) {
        tp.write_elem(elem); tmr_.write();
        tp.move_next();      tmr_.move();
    }

    T read(tape<T> &tp) {
        T elem{};
        tp.read_elem(elem); tmr_.read();

        return elem;
    }

    void write(T elem, tape<T> &tp) {
        tp.write_elem(elem); tmr_.write();
    }

    void move_next(tape<T> &tp) {
        tp.move_next(); tmr_.move();
    }

    void move_prev(tape<T> &tp) {
        tp.move_prev(); tmr_.move();
    }
};

} // <--- namespace tape_simulation