#ifndef INCLUDE_LWG_METADATA_H
#define INCLUDE_LWG_METADATA_H
#include "sections.h"
#include <map>
#include <unordered_map>
#include <ctime>
#include <filesystem>

namespace lwg {

// Various things read from meta-data/
struct metadata {
    section_map section_db;
    std::map<int, std::time_t> git_commit_times;
    std::unordered_map<std::string, std::string> paper_titles;

    static metadata read_from_path(std::filesystem::path const& path, bool verbose = true);
};

}
#endif
