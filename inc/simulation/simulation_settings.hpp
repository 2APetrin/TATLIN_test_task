#pragma once

#include "simulation_exceptions.hpp"

namespace tape_simulation {

struct machine_settings final {
    int    ram_size_elems = -1;
    double time_read      = -1,
           time_write     = -1,
           time_move      = -1,
           time_rewind    = -1;

public:
    bool is_valid() const {
        if (ram_size_elems <= 0 ||
            time_read       < 0 ||
            time_write      < 0 ||
            time_move       < 0 ||
            time_rewind     < 0)
            return false;

        return true;
    }
};

namespace detail {

class sim_timer final {
    machine_settings stt_;
    double time_ = 0;

public:
    sim_timer(const machine_settings& stt, int den) : stt_(stt) {
        stt_.ram_size_elems /= den;

        if (!stt_.is_valid())
            throw simulation_exceptions::config_file_troubles();
    }

    void read  (int n = 1) { time_ += n * stt_.time_read;   }
    void write (int n = 1) { time_ += n * stt_.time_write;  }
    void move  (int n = 1) { time_ += n * stt_.time_move;   }
    void rewind(int n = 1) { time_ += n * stt_.time_rewind; }

    double time() const { return time_; }
};

} // <--- namespace detail

} // <--- namespace tape_simulation