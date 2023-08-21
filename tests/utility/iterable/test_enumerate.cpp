#include "zip.hpp"
#include <vector>
#include "TEST_UTILS.hpp"
#include "range.hpp"

int main()
{
  std::vector<int> a = {1, 2, 3};
  std::vector<double> b = {0.1, 0.2, 0.3};
  // static_assert(std::ranges::range<decltype(ACCBOOST2::enumerate(a))>);
  static_assert(std::ranges::range<decltype(ACCBOOST2::enumerate(a, b))>);
  TEST_UTILS::dump(ACCBOOST2::enumerate(a, b));

  static_assert(std::ranges::range<decltype(ACCBOOST2::enumerate(ACCBOOST2::range(10)))>);

  return 0;
}
