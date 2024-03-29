#include "chain.hpp"
#include "range.hpp"
#include "map.hpp"
#include "TEST_UTILS.hpp"


using namespace ACCBOOST2;


int main()
{
  {
    auto a = range(3);
    auto b = range(5);
    static_assert(std::ranges::range<decltype(chain(a, b))>);
    TEST_UTILS::dump(chain(a, b));
  }

  {
    static_assert(std::ranges::range<decltype(chain(range(3), range(5)))>);
    TEST_UTILS::dump(chain(range(3), range(5)));
  }

  return 0;
}
