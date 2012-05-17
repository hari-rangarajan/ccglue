#include "../indexed_fstream.h"
#include <iostream>
#include "tag_db.h"
#include <iterator>
#include "../debug.h"

std::ofstream   log_stream("log.txt");
Logger  logger(&log_stream);

int main ()
{
    indexed_ofstream ofs("test.idx");
    std::ostream fbs(ofs.rdbuf());

    for (int i=1;i < 12; i++) {
        ofs.begin_record();
        fbs << i*1000 << " more than testing\n";
        ofs.end_record();
    }

    //ofs.dump(std::cout);
    ofs.write_index_to_file();
    ofs.close();

#if 0
    indexed_ifstream_vector<int> ifs("test.out", "test.idx");
    std::cout << "\ndumping out stuff " << std::endl;
    //ifs.dump(std::cout);
    
    char buf[512];

    int i = 0;
    for (indexed_ifstream_vector<int>::iterator it=ifs.begin();it != ifs.end(); it++) {
        std::cout << i<< "," << *it << '\n';
        i++;
    }
    std::string test_str = "This is a test stream";

    std::stringstream   ss(test_str);
    bounded_streambuf   bss(ss.rdbuf(), 15);
    std::istream        os(&bss);
    std::istreambuf_iterator<char>  is_it(os);
    
    std::istreambuf_iterator<char> eos;
    while (is_it != eos) {
        std::cout << "char " << *is_it << "\n";
        is_it++;
    }

    return 0;
#endif
    tag_db db("ccglue.out", "ccglue.idx");

    db.get_tag_by_id(1039);
    //db.get_tag_line("ARGP_KEY_INIT");
    //db.get_tag_line("3000 testing");
    //
    logger << "Testing stuff";

    return 0;
}
