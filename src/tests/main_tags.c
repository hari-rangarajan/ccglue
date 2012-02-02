#include "debug.h"
#include "tagfilerdr.h"

int main() {
    tag_t mytag;
    tag_file_rdr *tagrdr = new tag_file_rdr;

    tagrdr->open("mytags");
    while (tagrdr->read_next_tag(&mytag) == RC_SUCCESS) {
    };
    tagrdr->close();
}
