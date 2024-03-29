#pragma once

#include <iostream>
#include <fstream>
#include <ostream>

#include <exception>

namespace tape_simulation {

namespace detail {
    constexpr int elem_sz = sizeof(int);
}

class tape {
    std::fstream file_;
    int size_;

public:
    tape() : file_("test.tape", std::fstream::binary | std::fstream::in | std::fstream::out) {

        if (!file_.is_open()) {
            std::cout << "Cannot open file" << std::endl;
            return;
        }

        std::cout << "file opened" << std::endl;

        file_.seekg(0, file_.end);
        size_ = file_.tellg();
        file_.seekg(0, file_.beg);

        std::cout << "size_=" << size_ << std::endl;
    }

    void move_next() {
        auto pos = file_.cur + detail::elem_sz;
        if (pos >= size_) throw std::runtime_error("Tape out of range\n");

        file_.seekg(detail::elem_sz, file_.cur);
        file_.seekp(detail::elem_sz, file_.cur);
    }

    ~tape() { file_.close(); }
};

}