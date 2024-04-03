#pragma once

#include <stdexcept>

namespace simulation_exceptions {

struct not_valid_tapes : public std::runtime_error {
    not_valid_tapes() : std::runtime_error("Tapes to sort are not valid!\n") {}
};

struct sotring_the_same_tape : public std::runtime_error {
    sotring_the_same_tape() : std::runtime_error("In tape cannot be out tape in sorting_machine::sort()\n") {}
};

struct config_file_troubles : public std::runtime_error {
    config_file_troubles() : std::runtime_error("Config file is incorrect, check it\n") {}
};

}