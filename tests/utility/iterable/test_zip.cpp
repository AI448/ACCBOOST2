#include "zip.hpp"
#include <vector>
#include "TEST_UTILS.hpp"


int main()
{
  std::vector<int> a = {1, 2, 3};
  std::vector<double> b = {0.1, 0.2, 0.3};
  static_assert(std::ranges::range<decltype(ACCBOOST2::zip(a, b))>);
  TEST_UTILS::dump(ACCBOOST2::zip(a, b));

  return 0;
}
