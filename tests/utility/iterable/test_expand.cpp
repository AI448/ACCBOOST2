#include "chain_from_iterable.hpp"
#include "range.hpp"
#include "map.hpp"
#include "TEST_UTILS.hpp"


using namespace ACCBOOST2;


int main()
{
  {
    auto x = chain_from_iterable(map([](auto i){return range(i + 1);}, range(3)));
    static_assert(std::ranges::range<decltype(x)>);
    TEST_UTILS::dump(x); // 0 0 1 0 1 2
  }
  return 0;
}
