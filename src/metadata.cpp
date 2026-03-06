#include "metadata.h"

#include <fstream>
#include <iterator>
#include <iostream>

namespace {
    struct issue_mod_time {
        int id;
        std::time_t t;
        operator std::pair<const int, std::time_t>() const { return { id, t }; }
        friend std::istream& operator>>(std::istream& in, issue_mod_time& v) { return in >> v.id >> v.t; }
    };

    auto read_git_commit_times(std::filesystem::path const& path) -> std::map<int, std::time_t>
    {
        using Iter = std::istream_iterator<issue_mod_time>;
        std::ifstream f{path};
        std::map<int, std::time_t> times{ Iter{f}, Iter{} };
        return times;
    }

    auto read_paper_titles(std::filesystem::path const& path) -> std::unordered_map<std::string, std::string> {
        std::unordered_map<std::string, std::string> titles;
        std::ifstream in{path};
        std::string paper_number, title;
        while (in >> paper_number && std::getline(in, title))
        titles[paper_number] = title;
        return titles;
    }

}

auto lwg::metadata::read_from_path(std::filesystem::path const& path, bool verbose) -> metadata {
    auto filename = path / "meta-data" / "section.data";
    std::ifstream infile{filename};
    if (!infile.is_open()) {
        throw std::runtime_error{"Can't open section.data at " + path.string() + "meta-data"};
    }
    if (verbose)
      std::cout << "Reading section-tag index from: " << filename << std::endl;
    return {
        read_section_db(infile),
        read_git_commit_times(path / "meta-data" / "dates"),
        read_paper_titles(path / "meta-data" / "paper_titles.txt"),
    };
}
