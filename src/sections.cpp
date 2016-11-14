#include "sections.h"

#include <cassert>
#include <istream>
#include <ostream>
#include <sstream>
#include <iostream>
#include <cctype>

#if !defined(CPP14_LIBRARY_IS_NEEDED_FOR_STD_EXCHANGE)
// This should be part of <utility> in C++14 lib
// Should also be more efficient than using ostringstream!
// Will be available soon when assuming libc++ with Clang 3.4, or gcc 4.9 and later
namespace {

  template <typename TYPE, typename V_TYPE>
  auto exchange(TYPE & object, V_TYPE && value) -> TYPE {
    auto result = object;
    object = std::forward<V_TYPE>(value);
    return result;
  }

} // close unnamed namespace
#else
#include <utility>
using std::exchange;
#endif

auto lwg::operator<(section_tag const & x, section_tag const & y) noexcept -> bool {
   return (x.prefix < y.prefix) ?  true
        : (y.prefix < x.prefix) ? false
        : x.name < y.name;
}

auto lwg::operator==(section_tag const & x, section_tag const & y) noexcept -> bool {
   return x.prefix == y.prefix && x.name == y.name;
}

auto lwg::operator!=(section_tag const & x, section_tag const & y) noexcept -> bool {
   return !(x == y);
}

auto lwg::operator << (std::ostream& os, section_tag const & tag) -> std::ostream & {
  os << '[';
   if (!tag.prefix.empty()) { os << tag.prefix << "::"; }
   os <<  tag.name << ']';
   return os;
}

std::string lwg::as_string(section_tag const & x)
{
  return x.prefix.empty()
    ? x.name
    : x.prefix + "::" + x.name;
}

auto lwg::operator < (section_num const & x, section_num const & y) noexcept -> bool {
   // prefixes are unique, so there should be no need for a tiebreak.
   return (x.prefix < y.prefix) ?  true
        : (y.prefix < x.prefix) ? false
        : x.num < y.num;
}

auto lwg::operator == (section_num const & x, section_num const & y) noexcept -> bool {
   return (x.prefix != y.prefix)
        ? false
        : x.num == y.num;
}

auto lwg::operator != (section_num const & x, section_num const & y) noexcept -> bool {
   return !(x == y);
}

auto lwg::operator >> (std::istream& is, section_num& sn) -> std::istream & {
   sn.prefix.clear();
   sn.num.clear();
   ws(is);
   if (is.peek() == 'T') {
      is.get();
      if (is.peek() == 'R' || is.peek() == 'S') {
        sn.prefix = 'T';
        std::string w;
        is >> w;
        sn.prefix += w;
        ws(is);
      }
      else {
         sn.num.push_back(100 + 'T' - 'A');
         if (is.peek() != '.') {
            return is;
         }
         is.get();
      }
   }

   while (true) {
      if (std::isdigit(is.peek())) {
         int n;
         is >> n;
         sn.num.push_back(n);
      }
      else {
         char c;
         is >> c;
         sn.num.push_back(100 + c - 'A');
      }
      if (is.peek() != '.') {
         break;
      }
      char dot;
      is >> dot;
   }
   return is;
}

auto lwg::operator << (std::ostream& os, section_num const & sn) -> std::ostream & {
//   if (!sn.prefix.empty()) { os << sn.prefix << " "; }

   bool use_period{false};
   for (auto sub : sn.num ) {
      if (exchange(use_period, true)) {
         os << '.';
      }

      if (sub >= 100) {
         os << char(sub - 100 + 'A');
      }
      else {
         os << sub;
      }
   }
   return os;
}

auto lwg::read_section_db(std::istream & infile) -> section_map {
   section_map section_db;
   while (infile) {
      ws(infile);
      std::string line;
      getline(infile, line);
      if (!line.empty()) {
         // get [x.x....] symbolic tag 
         assert(line.back() == ']');
         auto p = line.rfind('[');
         assert(p != std::string::npos);
         section_tag tag;
         tag.name = line.substr(p);   // save [x.x....] symbolic tag
         assert(tag.name.size() > 2);
         assert(tag.name[0] == '[');
         assert(tag.name[tag.name.size()-1] == ']');
         tag.name.erase(0, 1);  // erase '[' from name
         tag.name.erase(tag.name.size() - 1);  // erase ']' from name
         line.erase(p-1);    // remove symbolic tag from line

         // get the prefix if any
         section_num num;
         while (!line.empty() && std::isspace(line[0]))
           line.erase(0,1);
         if (line.size() > 1 && std::isalpha(line[0])
           && line[1] != ' ' && line[1] != '.') // not an annex
         {
           std::string::size_type end;
           if ((end = line.find(' ')) != std::string::npos) {
             num.prefix = line.substr(0, end);  // save prefix
             line.erase(0, end+1);  // remove prefix + trailing space
           }
         }
         tag.prefix = num.prefix;

         // save [n.n....] numeric tag
         std::istringstream temp(line);
         if (!std::isdigit(line[0])) {
            char c;
            temp >> c;
            num.num.push_back(100 + c - 'A');
            temp >> c;
         }

         while (temp) {
            int n;
            temp >> n;
            if (!temp.fail()) {
               num.num.push_back(n);
               char dot;
               temp >> dot;
            }
         }
//         std::cout << "tag=\"" << tag.prefix << "\", \"" << tag.name << "\"\n";
//         std::cout << "num=\"" << num.prefix << "\", \"" << num.num[0] << "\"\n";
         section_db[tag] = num;  // stuff tag / num pair into section database
      }
   }
   return section_db;
}

