#include <iostream>
#include "tagfilerdr.h"

using namespace std;

int main ()
{
    //std::string test_str = "blah";

    std::string test_str = "1135#we		d»/^/;\" 	c:	p:¥	";

    std::stringstream   ss(test_str);
    std::istream        os(ss.rdbuf());
    istreambuf_iterator<char>  is_it(os);
    
    std::istreambuf_iterator<char> eos;
#if 0
    while (is_it != eos) {
        std::cout << "char " << (int) *is_it << "\n";
        is_it++;
    }
#endif
 //   return 0;
    tag  my_tag(ss.rdbuf());

    my_tag.dump();
}


