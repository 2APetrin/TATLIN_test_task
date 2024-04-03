#pragma once

#include <string>
#include <fstream>
#include <iostream>

#include "tape_exceptions.hpp"

namespace tape_simulation {

template<typename T>
class tape final {
    std::fstream file_;
    int          size_;

    static constexpr int elem_sz = sizeof(T);

public:
    tape(std::string path) : file_(path, std::fstream::out | std::fstream::in | std::fstream::binary) {
        if (!file_.is_open())
            throw tape_exceptions::cannot_open_file(path);

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);
    }

    tape() = default;

//------------------Rule of five------------------
    ~tape() = default;

    tape(const tape&) = delete;
    tape(tape&&)      = delete;

    tape& operator=(const tape&) = delete;
    tape& operator=(tape&&)      = delete;
//------------------------------------------------

    void move_next() {
        int pos = file_.tellg() + elem_sz;
        if (pos > size_) throw tape_exceptions::move_next_out_of_range();

        file_.seekg(elem_sz, file_.cur);
        /* seekg and seekp call rdbuf()->pubseekoff() for the same buffer,
        so no need to call seekp to move put pointer */
    }

    void move_prev() {
        int pos = file_.tellg() - elem_sz;
        if (pos < 0) throw tape_exceptions::move_prev_out_of_range();

        file_.seekg(-elem_sz, file_.cur);
    }

    bool read_elem(T &elem) {
        if (file_.tellg() == size_) return false;
        file_.read(reinterpret_cast<char*>(&elem), elem_sz);
        move_prev();

        return true;
    }

    void write_elem(T elem) {
        if (file_.tellg() == size_) size_ += elem_sz;
        file_.write(reinterpret_cast<char*>(&elem), elem_sz);
        move_prev();
    }

#ifdef TAPE_DUMP_MODE
    void dump() {
        auto save_pos = file_.tellg();
        file_.seekg(0, file_.beg);

        std::cout << "TAPE DUMP" << std::endl;

        for (int i = 0, e = size_/elem_sz, tmp{}; i < e; ++i) {
            read_elem(tmp); move_next();
            std::cout << "pos_" << i << "=" << tmp << std::endl;
        }

        file_.seekg(save_pos);
    }
#endif // TAPE_DUMP_MODE
#ifndef TAPE_DUMP_MODE
    void dump() { }
#endif // TAPE_DUMP_MODE

    void open_tape(std::string path) {
        if (file_.is_open())
            throw tape_exceptions::tape_already_initialized(path);

        file_.open(path, std::fstream::out | std::fstream::in | std::fstream::binary | std::fstream::trunc);

        if (!file_.is_open())
            throw tape_exceptions::cannot_open_file(path);

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);
    }

    int rewind_begin() {
        int len = file_.tellg() / elem_sz;
        file_.seekg(0, file_.beg);
        return len;
    }

    template<typename TypeIt>
    int read_buffer(TypeIt beg, TypeIt end) {
        int len = 0;

        for(int tmp{}; beg != end && read_elem(tmp); ++len, *(beg++) = tmp, move_next());

        return len;
    }

    template<typename TypeIt>
    int write_buffer(TypeIt beg, TypeIt end) {
        int len = 0;

        for (; beg != end; write_elem(*(beg++)), ++len, move_next());

        return len;
    }

    int pos()        { return file_.tellg() / elem_sz; }
    int size() const { return size_         / elem_sz; }

    bool is_valid() const { return file_.is_open() && size_ >= 0; }
};

} // <--- namespace tape_simulation