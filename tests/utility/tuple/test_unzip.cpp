#include "zip.hpp"
#include "unzip.hpp"
#include "TEST_UTILS.hpp"


int main()
{
  std::tuple<int, double> a = {1, 0.1};
  std::tuple<int, char> b = {2, 'a'};  

  TEST_UTILS::dump(ACCBOOST2::unzip(ACCBOOST2::zip(a, b)));

  return 0;
}
