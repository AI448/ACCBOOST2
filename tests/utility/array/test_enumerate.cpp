#include "enumerate.hpp"
#include "TEST_UTILS.hpp"


int main()
{
  int a[2] = {1, 2};
  std::array<double, 2> b = {0.1, 0.2};
  char x = 'x';
  char y = 'y';
  ACCBOOST2::StaticArray<char&, 2> c = {x, y};

  TEST_UTILS::dump(ACCBOOST2::enumerate(a, std::move(b), c));

  return 0;
}
