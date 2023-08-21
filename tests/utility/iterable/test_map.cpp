#include "map.hpp"
#include <vector>
#include "TEST_UTILS.hpp"


int main()
{
  {
    std::vector<int> a = {1, 2, 3, 4};
    std::vector<double> b = {0.1, 0.2, 0.3, 0.4, 0.5};
    std::vector<int> c = {10, 20, 30};
    auto x = ACCBOOST2::map([](auto&& x, auto&& y, auto&& z){return x + y + z;}, a, b, c);
    static_assert(std::ranges::range<decltype(x)>);
    TEST_UTILS::dump(x);
  }

  return 0;
}
