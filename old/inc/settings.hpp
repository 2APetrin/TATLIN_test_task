#pragma once

namespace tape_simulation {

struct machine_settings {
    double ram_size_elems = -1,
           time_read      = -1,
           time_write     = -1,
           time_move      = -1,
           time_rewind    = -1;

public:
    machine_settings() = default;

    bool is_valid() {
        if (ram_size_elems < 0 ||
            time_read      < 0 ||
            time_write     < 0 ||
            time_move      < 0 ||
            time_rewind    < 0)
            return false;

        return true;
    }
};

} // <--- tape_simulation