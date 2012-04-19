#include "indexed_fstream.h"
#include <algorithm>
#include <string>

class tag_db {
    public:
        tag_db(const char *fname, const char *idx_fname):
            ifs_db(fname, idx_fname) {};
        void get_tag_line (std::string tag_name);
        void dmp_all();
    protected:
        indexed_ifstream_vector<int>    ifs_db;
};

void tag_db::get_tag_line (std::string tag_name)
{
    //indexed_ifstream_vector<int>::iterator it = lower_bound(ifs_db.begin(), ifs_db.end(), tag_name);
    //std::cout << "Found "<< *it;
}

void tag_db::dmp_all ()
{
    indexed_ifstream_vector<int>::iterator iter(ifs_db);

    for (iter= ifs_db.begin(); iter != ifs_db.end(); iter++) {
        std::cout << *iter << " <<\n";
    }
}

