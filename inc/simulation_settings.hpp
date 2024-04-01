#pragma once

namespace tape_simulation {

namespace detail {
struct machine_settings final {
    double ram_size_elems = -1,
           time_read      = -1,
           time_write     = -1,
           time_move      = -1,
           time_rewind    = -1;

public:
    machine_settings() = default;

    bool is_valid() {
        if (ram_size_elems <= 0 ||
            time_read       < 0 ||
            time_write      < 0 ||
            time_move       < 0 ||
            time_rewind     < 0)
            return false;

        return true;
    }
};

/** timer class for tracking simulation time */
class sim_timer final {
    machine_settings stt_;
    double time_ = 0;

public:
    sim_timer(machine_settings& stt, int den) : stt_(stt) {
        stt_.ram_size_elems /= den;
    }

    void read  (int n = 1) { time_ += n * stt_.time_read;   }
    void write (int n = 1) { time_ += n * stt_.time_write;  }
    void move  (int n = 1) { time_ += n * stt_.time_move;   }
    void rewind(int n = 1) { time_ += n * stt_.time_rewind; }

    double time() { return time_; }
};

} // <--- namespace detail

} // <--- namespace tape_simulation