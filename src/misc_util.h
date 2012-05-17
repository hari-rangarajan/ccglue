#include <string>
#include <sstream>
#include <vector>


namespace misc_utils {
    void string_split (const std::string& src_str, char delim,
            std::vector<std::string>& v);
    template<class T> T atoi (std::string& s)
    {
        T value;

        std::stringstream   ss;

        ss <<  s;
        ss >> value;

        return value;
    };
    bool read_string_vector_from_file (const std::string& ,
                std::vector<std::string>& );
}
