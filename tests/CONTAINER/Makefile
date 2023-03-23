TESTS=test_Array test_ZippedArray


RESULTS=$(patsubst %, %.result, $(TESTS))
OUTS=$(patsubst %, %.out, $(TESTS))
DEPENDS=$(patsubst %, %.d, $(TESTS))

CXXFLAGS=-std=c++20 -W -Wall -g -O2 -I../../ACCBOOST2/container -I..


all: $(RESULTS)

clean:
	rm -f $(RESULTS) $(OUTS) $(DEPENDS)

.PRECIOUS: $(OUTS) $(DEPENDS)

-include $(DEPENDS)

%.result: %.out
	valgrind --tool=memcheck --leak-check=full ./$< >$@
	cat $@

%.out: %.cpp
	$(CXX) $< $(CXXFLAGS) -o $@
	$(CXX) -MM $< $(CXXFLAGS) | sed 's%^.*\.o%$@%g' >$(patsubst %.out, %.d, $@)
