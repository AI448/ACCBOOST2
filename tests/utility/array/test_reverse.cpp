#include "reverse.hpp"
#include "TEST_UTILS.hpp"


int main()
{
  int a[] = {1, 2, 3};

  TEST_UTILS::dump(ACCBOOST2::reverse(a));

  return 0;
}
