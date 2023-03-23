#include "enumerate.hpp"
#include "TEST_UTILS.hpp"


int main()
{
  std::tuple<int, double, char> a = {1, 0.1, 'c'};

  TEST_UTILS::dump(ACCBOOST2::enumerate(a));
}
