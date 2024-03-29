#pragma once

#include <iostream>
#include <fstream>

#include <limits>
#include <exception>

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

        std::cout << "file opened" << std::endl;

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);

        std::cout << "size_=" << size_ << std::endl;
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
        if (pos > size_) throw std::runtime_error("Tape out of range\n");

        file_.seekg(elem_sz, file_.cur);
        // file_.seekp(detail::elem_sz, file_.cur); // they call rdbuf()->pubseekoff() for the same buffer so no need to call seekp to move put pointer
    }

    void move_prev() {
        if (file_.tellg() == 0) throw std::runtime_error("Tape out of range\n");

        file_.seekg(-elem_sz, file_.cur);
    }

    T read_elem() {
        T tmp{};
        file_.read(reinterpret_cast<char*>(&tmp), elem_sz);
        return tmp;
    }

    void write_elem(T elem) {
        if (file_.tellg() == size_) size_ += elem_sz;
        file_.write(reinterpret_cast<char*>(&elem), elem_sz);
    }

    void dump() {
        auto save_pos = file_.tellg();
        file_.seekg(0, file_.beg);

        for (int i = 0; i < size_/elem_sz; ++i)
            std::cout << "pos_" << i << "=" << read_elem() << std::endl;

        file_.seekg(save_pos);
    }

    void open_tape(std::string path) {
        file_.open(path, std::fstream::out | std::fstream::in | std::fstream::binary);

        if (!file_.is_open())
            throw std::runtime_error("Cannot open file: " + path + "\n");

        std::cout << "file opened" << std::endl;

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);

        std::cout << "size_=" << size_ << std::endl;
    }

    void clear_tape() {
        file_.clear();
        file_.ignore(std::numeric_limits<std::streamsize>::max());

        file_.seekg(0, file_.beg);
        size_ = 0;
    }

    void rewind_to(int pos) {
        int curr = file_.tellg() / elem_sz;

        if (pos * elem_sz > size_) throw std::runtime_error("Rewind out of range\n");

        int diff = pos - curr;

        file_.seekg(diff, file_.cur);
    }
};

} // <--- tape_simulation