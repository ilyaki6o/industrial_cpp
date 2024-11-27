#include "functions/func.h"
#include "factory/factory.h"
#include <iostream>


int main(void){
    TFactory factory;

    auto c1 = factory.CreateObject("polynomial", {-10, 3, 1});
    auto c2 = factory.CreateObject("exp");

    auto c3 = c1 + c2;

    std::cout << c1->ToString() << " Value: " << (*c1)(10) << " DerivValue: " << c1->DerivativeValue(10) << std::endl;

    std::cout << FuncRoot(c1, -10) << std::endl;

    return 0;
}
