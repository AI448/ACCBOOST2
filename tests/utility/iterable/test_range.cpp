#include "range.hpp"
#include <vector>
#include "TEST_UTILS.hpp"


int main()
{
  static_assert(std::ranges::range<decltype(ACCBOOST2::range(10))>);
  static_assert(std::ranges::random_access_range<decltype(ACCBOOST2::range(10))>);

  TEST_UTILS::dump(ACCBOOST2::range(10));

  return 0;
}
