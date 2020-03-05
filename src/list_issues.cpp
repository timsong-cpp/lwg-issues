// This program reads all the issues in the issues directory passed as the first command line argument.
// If all documents are successfully parsed, it will generate the standard LWG Issues List documents
// for an ISO SC22 WG21 mailing.

// Based on code originally donated by Howard Hinnant.
// Since modified by Alisdair Meredith and modernised by Jonathan Wakely.

// Note that this program requires a C++17 compiler supporting std::filesystem.

// TODO
// .  Better handling of TR "sections", and grouping of issues in "Clause X"
// .  Sort the Revision comments in the same order as the 'Status' reports, rather than alphabetically
// .  Lots of tidy and cleanup after merging the revision-generating tool
// .  Refactor more common text
// .  Split 'format' function and usage to that the issues vector can pass by const-ref in the common cases
// .  Document the purpose and contract on each function
// .  Waiting on external fix for preserving file-dates
// .  sort-by-last-modified-date should offer some filter or separation to see only the issues modified since the last meeting

// Missing standard facilities that we work around
// . Date

// Missing standard library facilities that would probably not change this program
// . XML parser

// standard headers
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <filesystem>
namespace fs = std::filesystem;

// solution specific headers
#include "issues.h"
#include "sections.h"


auto read_file_into_string(fs::path const & filename) -> std::string {
   // read a text file completely into memory, and return its contents as
   // a 'string' for further manipulation.

   std::ifstream infile{filename};
   if (!infile.is_open()) {
      throw std::runtime_error{"Unable to open file " + filename.string()};
   }

   std::istreambuf_iterator<char> first{infile}, last{};
   return std::string {first, last};
}

// Issue-list specific functionality for the rest of this file
// ===========================================================

auto is_issue_xml_file(fs::directory_entry const & e) {
   if (e.is_regular_file()) {
      fs::path f = e.path().filename();
      return f.string().compare(0, 5, "issue") == 0 && f.extension() == ".xml";
   }
   return false;
}

void filter_issues(fs::path const & issues_path, lwg::section_map & section_db, std::function<bool(lwg::issue const &)> predicate) {
   // Open the specified directory, 'issues_path', and iterate all the '.xml' files
   // it contains, parsing each such file as an LWG issue document.  Write to 'out'
   // the number of every issue that satisfies the 'predicate'.

  for (auto ent : fs::directory_iterator(issues_path)) {
     if (is_issue_xml_file(ent)) {
         fs::path const issue_file = ent.path();
        auto const iss = parse_issue_from_file(read_file_into_string(issue_file), issue_file.string(), section_db);
        if (predicate(iss)) {
           std::cout << iss.num << '\n';
        }
     }
  }
}

// ============================================================================================================

void check_is_directory(fs::path const & directory) {
   if (!is_directory(directory)) {
      throw std::runtime_error(directory.string() + " is not an existing directory");
   }
}

int main(int argc, char const* argv[]) {
   try {
      bool trace_on{false};  // Will pick this up from the command line later

      if (argc != 2) {
         std::cerr << "Must specify exactly one status\n";
         return 2;
      }
      std::string const status{argv[1]};
  
      fs::path path = fs::current_path();

      check_is_directory(path);

      lwg::section_map section_db = [&] {
         // This lambda expression scopes the lifetime of an open 'fstream'
         auto filename = path / "meta-data/section.data";
         std::ifstream infile{filename};
         if (!infile.is_open()) {
            throw std::runtime_error{"Can't open section.data at " + path.string() + "meta-data"};
         }

         if (trace_on) {
            std::cout << "Reading section-tag index from: " << filename << std::endl;
         }

         return lwg::read_section_db(infile);
      }();

      filter_issues(path / "xml/", section_db, [status](lwg::issue const & iss) { return status == iss.stat; });
   }
   catch(std::exception const & ex) {
      std::cout << ex.what() << std::endl;
      return -1;
   }
}

