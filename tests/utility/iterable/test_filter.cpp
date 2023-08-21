#include "chain.hpp"
#include "range.hpp"
#include "filter.hpp"
#include "TEST_UTILS.hpp"


using namespace ACCBOOST2;


int main()
{
  {
    auto x = filter([](auto&& x){return x % 2 == 0;}, range(10));
    static_assert(std::ranges::range<decltype(x)>);
    TEST_UTILS::dump(x);
  }
  {
    auto a = range(10);
    auto x = filter([](auto&& x){return x % 2 == 0;}, a);
    static_assert(std::ranges::range<decltype(x)>);
    TEST_UTILS::dump(x);
  }
  return 0;
}
