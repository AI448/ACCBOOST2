#include "chain_from_iterable.hpp"
#include "range.hpp"
#include "map.hpp"
#include "TEST_UTILS.hpp"


using namespace ACCBOOST2;


int main()
{
  TEST_UTILS::dump(chain_from_iterable(map([](auto i){return range(i + 1);}, range(3)))); // 0 0 1 0 1 2
  TEST_UTILS::dump(chain_from_iterable({range(3), range(4)})); // 0 1 2 0 1 2 3
  return 0;
}
