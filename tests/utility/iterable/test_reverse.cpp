#include "reverse.hpp"
#include <vector>
#include "TEST_UTILS.hpp"


int main()
{
  {
    std::vector<int> a = {1, 2, 3, 4};
    auto x = ACCBOOST2::reverse(a);
    static_assert(std::ranges::bidirectional_range<decltype(x)>);
    TEST_UTILS::dump(x);
  }
  return 0;
}
