#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "simulation_settings.hpp"

namespace main_details {
    namespace fs  = std::filesystem;
    namespace ts  = tape_simulation;
    namespace tsd = tape_simulation::detail;

namespace detail {
    bool open_ifstream(std::string &path, std::ifstream &strm) {
        strm.open(path);

        if (!strm.is_open()) {
            std::cerr << "Cannot open settings_buff file:" << path << std::endl;
            return true;
        }

        return false;
    }

    void place_settings(tsd::machine_settings &settings, std::unordered_map<std::string, double> &map) {
        auto it = map.find("ram_size_byte"); if (it != map.end()) { settings.ram_size_elems = it->second; };
             it = map.find("time_read");     if (it != map.end()) { settings.time_read      = it->second; };
             it = map.find("time_write");    if (it != map.end()) { settings.time_write     = it->second; };
             it = map.find("time_move");     if (it != map.end()) { settings.time_move      = it->second; };
             it = map.find("time_rewind");   if (it != map.end()) { settings.time_rewind    = it->second; };
    }
} // <--- namespace detail

    bool process_settings(tsd::machine_settings &settings, std::string path) {
        std::ifstream settings_strm;
        if (detail::open_ifstream(path, settings_strm)) return true;

        std::string setting;
        double      value;

        std::unordered_map<std::string, double> map;

        while ((settings_strm >> setting) && (settings_strm >> value))
            map.emplace(setting, value);

        detail::place_settings(settings, map);

        if (!settings.is_valid()) {
            std::cerr << "Smth is wrong with configure file:" << path << std::endl;
            return true;
        }

        return false;
    }

    bool convert_int_to_binary(std::string path, std::string bin_path) {
        std::ifstream in_stream(path);

        if (!in_stream.is_open()) {
            std::cerr << "Cannot open IN_int_file:" << path << std::endl;
            return true;
        }

        std::ofstream out_stream(bin_path, std::ofstream::binary);

        if (!out_stream.is_open()) {
            std::cerr << "Cannot open OUT_binary_file:" << bin_path << std::endl;
            return true;
        }

        int elem_sz = sizeof(int);

        std::string str;
        int         tmp{};

        while ((in_stream >> str)) {
            tmp = std::stoi(str);
            out_stream.write(reinterpret_cast<char*>(&tmp), elem_sz);
        }

        in_stream.close(); out_stream.close();

        return false;
    }

    bool convert_binary_to_int(std::string path, std::string bin_path) {
        std::ofstream out_stream(path);

        if (!out_stream.is_open()) {
            std::cerr << "Cannot open OUT_int_file:" << path << std::endl;
            return true;
        }

        std::ifstream in_stream(bin_path, std::ofstream::binary);

        if (!in_stream.is_open()) {
            std::cerr << "Cannot open IN_binary_file:" << bin_path << std::endl;
            return true;
        }

        int elem_sz = sizeof(int);

        int tmp{};

        in_stream.seekg(0, in_stream.end);
        int size_ = in_stream.tellg();
        in_stream.seekg(0, in_stream.beg);

        int pos = 0;

        while (pos != size_) {
            in_stream.read(reinterpret_cast<char*>(&tmp), elem_sz);
            pos += elem_sz;

            out_stream << tmp << "\n";
        }

        in_stream.close(); out_stream.close();

        return false;
    }
} // <--- namespace main_details
