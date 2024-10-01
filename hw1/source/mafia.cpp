#include "shared_ptr.h"
#include <iostream>

int main(void){
    using namespace msp;

    shared_ptr<int> p(new int(42));
    std::cout << p.use_count() << std::endl;

    return 0;
}