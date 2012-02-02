#include "indexed_file.h"
#include <iostream>

int main ()
{
    indexed_ofstream ofs("test.out");

    for (int i=0;i < 10; i++) {
        ofs.begin_record();
        ofs << i*1000 << " testing\n";
        ofs.end_record();
    }

    ofs.dump(std::cout);
    return 0;
}
