#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#include "misc_util.h"

bool misc_utils::read_string_vector_from_file (const std::string& filename,
        std::vector<std::string>& vec)
{
    std::ifstream   ifs(filename.c_str());
    std::string     s;
    
    if (ifs.fail()) {
        return false;
    }

    while (!ifs.eof()) {
        getline(ifs, s);
        std::cout << "[" << s << " ]\n";
        vec.push_back(s);
    }
    return true;
}

void misc_utils::string_split (const std::string& src_str, char delim,
        std::vector<std::string>& v)
{
    std::istringstream   is(src_str);
        std::string     tmp_str;

    while (is.good()) {
        getline(is, tmp_str, delim);
        v.push_back(tmp_str);
    }
}

#if 0
template<class T> 
T misc_utils::atoi (std::string& s)
{
    T value;

    std::stringstream   ss;

    ss <<  s;
    ss >> value;

    return value;
}
#endif
