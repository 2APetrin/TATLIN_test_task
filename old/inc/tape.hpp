#pragma once

#include <fstream>
#include <iostream>

namespace tape_simulation {

template<typename T>
class tape final {
    std::fstream file_;
    int          size_;

    static constexpr int elem_sz = sizeof(T);

public:
    tape(std::string path) : file_(path, std::fstream::out | std::fstream::in | std::fstream::binary) {
        if (!file_.is_open())
            throw std::runtime_error("Cannot open file: " + path + "\n");

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);
    }

    tape() = default;

//------------------Rule of five------------------
    ~tape() { file_.close(); }

    tape(const tape&) = delete;
    tape(tape&&)      = delete;

    tape& operator=(const tape&) = delete;
    tape& operator=(tape&&)      = delete;
//------------------------------------------------

    void move_next() {
        int pos = file_.tellg() + elem_sz;
        if (pos > size_) throw std::runtime_error("Move next out of range\n");

        file_.seekg(elem_sz, file_.cur);
        /* seekg and seekp call rdbuf()->pubseekoff() for the same buffer,
        so no need to call seekp to move put pointer */
    }

    void move_prev() {
        if (file_.tellg() == 0) throw std::runtime_error("Move prev out of range\n");

        file_.seekg(-elem_sz, file_.cur); /* same as move_next */
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

    void dump() {
        auto save_pos = file_.tellg();
        file_.seekg(0, file_.beg);

        std::cout << "DUMP" << std::endl;

        for (int i = 0, e = size_/elem_sz, tmp{}; i < e; ++i) {
            read_elem(tmp); move_next();
            std::cout << "pos_" << i << "=" << tmp << std::endl;
        }

        file_.seekg(save_pos);
    }

    void open_tape(std::string path) {
        if (file_.is_open())
            throw std::runtime_error("Tape is already initialized. Trying to open: " + path + "\n");

        file_.open(path, std::fstream::out | std::fstream::in | std::fstream::binary);

        if (!file_.is_open())
            throw std::runtime_error("Cannot open file in open_tape: " + path + "\n");

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

        // int tmp{};
        // while (read_elem(tmp) && beg != end) {
        //     ++len;
        //     *(beg++) = tmp;
        // }

        for(int tmp{}; beg != end && read_elem(tmp); ++len, *(beg++) = tmp, move_next());

        return len;
    }

    template<typename TypeIt>
    int write_buffer(TypeIt beg, TypeIt end) {
        int len = 0;

        for (; beg != end; write_elem(*(beg++)), ++len, move_next());

        return len;
    }

    int position() { return file_.tellg() / elem_sz; }
    int size()     { return size_         / elem_sz; }
};


} // <--- namespace tape_simulation