

#include <iostream>

#include "Array.hpp"


int main()
{

  using namespace ACCBOOST2;

  Array<double> a = {0, 1, 2};
  Array<double> b = {3, 4, 5};

  a.expand(b);

  for(auto&& x: a){
    std::cout << x << std::endl;
  }

}