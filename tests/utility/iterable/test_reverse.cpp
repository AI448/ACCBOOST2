#include "reverse.hpp"
#include <vector>
#include "TEST_UTILS.hpp"


int main()
{
  {
    std::vector<int> a = {1, 2, 3, 4};
    TEST_UTILS::dump(ACCBOOST2::reverse(a));
  }
  {
    TEST_UTILS::dump(ACCBOOST2::reverse({1, 2, 3, 4}));
  }
  return 0;
}
