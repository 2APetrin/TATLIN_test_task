#pragma once

#include <stdexcept>
#include <string>

namespace tape_exceptions {

struct cannot_open_file : public std::runtime_error {
    cannot_open_file(std::string path) : std::runtime_error("Cannot open file: " + path + "\n") {}
};

struct cannot_open_in_file : public std::runtime_error {
    cannot_open_in_file(std::string path) : std::runtime_error("Cannot open IN file: " + path + "\n") {}
};

struct move_next_out_of_range : public std::runtime_error {
    move_next_out_of_range() : std::runtime_error("Move next out of range\n") {}
};

struct move_prev_out_of_range : public std::runtime_error {
    move_prev_out_of_range() : std::runtime_error("Move prev out of range\n") {}
};

struct tape_already_initialized : public std::runtime_error {
    tape_already_initialized(std::string path) : std::runtime_error("Tape is already initialized. Trying to open: " + path + "\n") {}
};

}