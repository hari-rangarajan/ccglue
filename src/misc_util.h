#include <string>
#include <sstream>
#include <vector>


void string_split (const std::string& src_str, char delim,
        std::vector<std::string>& v)
{
    std::istringstream   is(src_str);
    std::string     tmp_str;

    while (is.good()) {
        getline(is, tmp_str, delim);
        v.push_back(tmp_str);
    }
}
