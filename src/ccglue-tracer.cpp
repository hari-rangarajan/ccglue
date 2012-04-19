#include "tag_db.h"

int main() 
{
    tag_db  tdb("ccglue.out", "ccglue.idx");

    tdb.dmp_all();
    return 0;
    tdb.get_tag_line("fprintf");
}
