#ifdef _MSC_VER
# define _CRT_SECURE_NO_WARNINGS
#endif

#include "issues.h"
#include "sections.h"
#include "status.h"

#include <algorithm>
#include <cassert>
#include <istream>
#include <ostream>
#include <string>
#include <stdexcept>

#include <sstream>

#include <iostream>  // eases debugging

#include <ctime>
#include <chrono>

#include <filesystem>
namespace fs = std::filesystem;

namespace {
// date utilites may factor out again
auto parse_month(std::string const & m) -> gregorian::month {
   // This could be turned into an efficient map lookup with a suitable indexed container
   return (m == "Jan") ? gregorian::jan
        : (m == "Feb") ? gregorian::feb
        : (m == "Mar") ? gregorian::mar
        : (m == "Apr") ? gregorian::apr
        : (m == "May") ? gregorian::may
        : (m == "Jun") ? gregorian::jun
        : (m == "Jul") ? gregorian::jul
        : (m == "Aug") ? gregorian::aug
        : (m == "Sep") ? gregorian::sep
        : (m == "Oct") ? gregorian::oct
        : (m == "Nov") ? gregorian::nov
        : (m == "Dec") ? gregorian::dec
        : throw std::runtime_error{"unknown month " + m};
}

auto parse_date(std::istream & temp) -> gregorian::date {
   int d;
   temp >> d;
   if (temp.fail()) {
      throw std::runtime_error{"date format error"};
   }

   std::string month;
   temp >> month;

   auto m = parse_month(month);
   int y{ 0 };
   temp >> y;
   return m/gregorian::day{d}/y;
}

auto make_date(std::tm const & mod) -> gregorian::date {
   return gregorian::year((unsigned short)(mod.tm_year+1900)) / (mod.tm_mon+1) / mod.tm_mday;
}

auto report_date_file_last_modified(std::string const & filename) -> std::time_t {
   auto file_mtime = fs::last_write_time(filename);
   auto sys_mtime = std::chrono::system_clock::now() - (fs::file_time_type::clock::now() - file_mtime);
   return std::chrono::system_clock::to_time_t(sys_mtime);
}

auto make_date(std::time_t t) -> gregorian::date {
   return make_date(*std::gmtime(&t));
}

} // close unnamed namespace

auto lwg::parse_issue_from_file(std::string tx, std::string const & filename,
  lwg::section_map & section_db) -> issue {
   struct bad_issue_file : std::runtime_error {
      bad_issue_file(std::string const & filename, char const * error_message)
         : runtime_error{"Error parsing issue file " + filename + ": " + error_message}
         {
      }
   };

   issue is;

   // Get issue number
   auto k = tx.find("<issue num=\"");
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue number"};
   }
   k += sizeof("<issue num=\"") - 1;
   auto l = tx.find('\"', k);
   std::istringstream temp{tx.substr(k, l-k)};
   temp >> is.num;

   // Get issue status
   k = tx.find("status=\"", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue status"};
   }
   k += sizeof("status=\"") - 1;
   l = tx.find('\"', k);
   is.stat = tx.substr(k, l-k);

   // Get issue title
   k = tx.find("<title>", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue title"};
   }
   k +=  sizeof("<title>") - 1;
   l = tx.find("</title>", k);
   is.title = tx.substr(k, l-k);

   // Extract doc_prefix from title
   if (is.title[0] == '['
     && is.title.find("[CD]") == std::string::npos) // special case for a few titles
   {
      std::string::size_type pos = is.title.find(']');
      if (pos != std::string::npos)
        is.doc_prefix = is.title.substr(1, pos - 1);
//    std::cout << is.doc_prefix << '\n';
   }

   // Get issue sections
   k = tx.find("<section>", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue section"};
   }
   k += sizeof("<section>") - 1;
   l = tx.find("</section>", k);
   while (k < l) {
      k = tx.find('\"', k);
      if (k >= l) {
          break;
      }
      auto k2 = tx.find('\"', k+1);
      if (k2 >= l) {
         throw bad_issue_file{filename, "Unable to find issue section"};
      }
      ++k;
      section_tag tag;
      tag.prefix = is.doc_prefix;
      tag.name = tx.substr(k+1, k2 - k - 2);
//std::cout << "lookup tag=\"" << tag.prefix << "\", \"" << tag.name << "\"\n";
      is.tags.emplace_back(tag);
      if (section_db.find(is.tags.back()) == section_db.end()) {
          section_num num{};
 //         num.num.push_back(100 + 'X' - 'A');
          num.prefix = tag.prefix;
          num.num.push_back(99);
          section_db[is.tags.back()] = num;
      }
      k = k2;
      ++k;
   }

   if (is.tags.empty()) {
      throw bad_issue_file{filename, "Unable to find issue section"};
   }

   // Get submitter
   k = tx.find("<submitter>", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue submitter"};
   }
   k += sizeof("<submitter>") - 1;
   l = tx.find("</submitter>", k);
   is.submitter = tx.substr(k, l-k);

   // Get date
   k = tx.find("<date>", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue date"};
   }
   k += sizeof("<date>") - 1;
   l = tx.find("</date>", k);
   temp.clear();
   temp.str(tx.substr(k, l-k));

   try {
      is.date = parse_date(temp);

      // Get modification timestamp
      is.mod_timestamp = report_date_file_last_modified(filename);
      is.mod_date = make_date(is.mod_timestamp);
   }
   catch(std::exception const & ex) {
      throw bad_issue_file{filename, ex.what()};
   }

   // Get priority - this element is optional
   k = tx.find("<priority>", l);
   if (k != std::string::npos) {
      k += sizeof("<priority>") - 1;
      l = tx.find("</priority>", k);
      if (l == std::string::npos) {
         throw bad_issue_file{filename, "Corrupt 'priority' element: no closing tag"};
      }
      is.priority = std::stoi(tx.substr(k, l-k));
   }

   // Trim text to <discussion>
   k = tx.find("<discussion>", l);
   if (k == std::string::npos) {
      throw bad_issue_file{filename, "Unable to find issue discussion"};
   }
   tx.erase(0, k);

   // Find out if issue has a proposed resolution
   if (is_active(is.stat)  or  "Pending WP" == is.stat) {
      auto k2 = tx.find("<resolution>", 0);
      if (k2 == std::string::npos) {
         is.has_resolution = false;
      }
      else {
         k2 += sizeof("<resolution>") - 1;
         auto l2 = tx.find("</resolution>", k2);
         is.resolution = tx.substr(k2, l2 - k2);
         if (is.resolution.length() < 15) {
            // Filter small ammounts of whitespace between tags, with no actual resolution
            is.resolution.clear();
         }
//         is.has_resolution = l2 - k2 > 15;
         is.has_resolution = !is.resolution.empty();
      }
   }
   else {
      is.has_resolution = true;
   }

   is.text = std::move(tx);
   return is;
}
