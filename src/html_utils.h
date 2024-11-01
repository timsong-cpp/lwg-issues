#include <string>
#include <string_view>

namespace lwg
{
// Replace reserved characters with entities, for use as an attribute value.
// Use for attributes like title="..." and content="...".
inline std::string replace_reserved_char(std::string text, char c, std::string_view repl) {
   for (auto p = text.find(c); p != text.npos; p = text.find(c, p+repl.size()))
      text.replace(p, 1, repl);
   return text;
}

// Remove XML elements from the argument to just get the text nodes.
// Used to generate plain text versions of elements like <title>.
inline std::string strip_xml_elements(std::string xml) {
   for (auto p = xml.find('<'); p != xml.npos; p = xml.find('<', p))
      xml.erase(p, xml.find('>', p) + 1 - p);
   return xml;
}

struct issue;

// Create an <a> element linking to an issue
auto make_html_anchor(issue const & iss) -> std::string;

}
