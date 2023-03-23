#include "chain.hpp"
#include "TEST_UTILS.hpp"


int main()
{
  int a[] = {1, 2};
  std::array<int, 3> b = {3, 4, 5};
  TEST_UTILS::dump(ACCBOOST2::chain(a, b));

  int x = 6;
  
  int y = 7;
  ACCBOOST2::StaticArray<int&, 2> c = {x, y};

  TEST_UTILS::dump(ACCBOOST2::chain(a, b, c));

  TEST_UTILS::dump(ACCBOOST2::chain(a, std::move(b), c));

  return 0;
}