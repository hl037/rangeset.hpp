#include <initializer_list>
#include <vector>
#define private public
#define protected public
#include "rangeset.hpp"
#undef private
#undef protected

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

namespace test_rangeset{

template <typename T>
struct data_case_t
{
  private:
    const T* start;
    size_t size;
  public:

  data_case_t()=default;
  ~data_case_t()=default;

  template<typename B>
  explicit data_case_t(const B & collection) :
    start(std::begin(collection))
  {
    size = std::end(collection) - start;
  }

  data_case_t<T> & operator=(const data_case_t<T> &)=default;
  
  template<typename B>
  data_case_t<T> & operator=(const B & collection)
  {
    start = std::cbegin(collection);
    size = std::cend(collection) - start;
    return *this;
  }

  const T* begin() const { return start; }
  const T* end () const { return &start[size]; }
};

template <typename T, bool B>
void assert_rangeset_equals(const std::initializer_list<std::pair<T, T> > & expected, const RangeSet<T, B> & set){
  REQUIRE(expected.size() == set.size());
  auto && it1 = expected.begin(), end1 = expected.end();
  auto && it2 = set.cbegin(), end2 = set.cend();
  for(; it1 != end1 && it2 != end2; ++it1, ++it2) {
    REQUIRE(it2->first == it1->first);
    REQUIRE(it2->second == it1->second);
  }
}

template <typename T, bool B>
void assert_state(const RangeSet<T, B> & set){
  REQUIRE(set.data.size() % 2 == 0 );
  if(set.data.size() % 2){
    return;
  }
  auto && it = set.data.begin(), end = set.data.end();
  while(it != end) {
    REQUIRE((int) it++->dir == (int) RangeSet<T, B>::end_point_t::LOWER);
    REQUIRE((int) it++->dir == (int) RangeSet<T, B>::end_point_t::UPPER);
  }
}


namespace insert {

struct param_t {
  const char * name;
  const std::initializer_list<std::pair<int, int> > inserted;
  const std::initializer_list<std::pair<int, int> > expected;
};

static param_t simple[] = {
  //////// SIMPLE ////////
  {
    "Empty is Empty",
    {},
    {}
  },
  {
    "Empty inserted to Empty is still Empty",
    {{5, 5}},
    {}
  },
  {
    "Insert one element",
    {{10, 20}},
    {{10, 20}}
  },
  {
    "Insert 2 elements",
    {{10, 20}, {30, 40}},
    {{10, 20}, {30, 40}},
  },
  {
    "Change order",
    {{30, 40}, {10, 20}},
    {{10, 20}, {30, 40}},
  },
};

static param_t common[] = {
  //////// COMMON ////////
  {
    "Base for other tests + insert at end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert at start",
    {{10, 20}, {30, 40}, {50, 60}, {2, 8}},
    {{2, 8}, {10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert empty",
    {{10, 20}, {30, 40}, {50, 60}, {2, 2}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert empty middle",
    {{10, 20}, {30, 40}, {50, 60}, {23, 23}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert empty, then insert",
    {{10, 20}, {2, 2}, {30, 40}, {50, 60}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert empty middle, then insert",
    {{10, 20}, {23, 23}, {30, 40}, {50, 60}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert inside range does nothing",
    {{10, 20}, {30, 40}, {50, 60}, {32, 38}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Insert middle",
    {{10, 20}, {30, 40}, {50, 60}, {23, 25}},
    {{10, 20}, {23, 25}, {30, 40}, {50, 60}},
  },
  {
    "Start overlap, begin",
    {{10, 20}, {30, 40}, {50, 60}, {5, 11}},
    {{5 , 20}, {30, 40}, {50, 60}},
  },
  {
    "Start overlap, middle",
    {{10, 20}, {30, 40}, {50, 60}, {25, 31}},
    {{10, 20}, {25, 40}, {50, 60}},
  },
  {
    "Start overlap, end",
    {{10, 20}, {30, 40}, {50, 60}, {45, 51}},
    {{10, 20}, {30, 40}, {45, 60}},
  },
  {
    "End overlap, begin",
    {{10, 20}, {30, 40}, {50, 60}, {19, 25}},
    {{10, 25}, {30, 40}, {50, 60}},
  },
  {
    "End overlap, middle",
    {{10, 20}, {30, 40}, {50, 60}, {39, 45}},
    {{10, 20}, {30, 45}, {50, 60}},
  },
  {
    "End overlap, end",
    {{10, 20}, {30, 40}, {50, 60}, {59, 65}},
    {{10, 20}, {30, 40}, {50, 65}},
  },
  {
    "Both overlap, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 21}},
    {{9 , 21}, {30, 40}, {50, 60}},
  },
  {
    "Both overlap, middle",
    {{10, 20}, {30, 40}, {50, 60}, {29, 41}},
    {{10, 20}, {29, 41}, {50, 60}},
  },
  {
    "Both overlap, end",
    {{10, 20}, {30, 40}, {50, 60}, {49, 61}},
    {{10, 20}, {30, 40}, {49, 61}},
  },
  {
    "Join begin",
    {{10, 20}, {30, 40}, {50, 60}, {19, 31}},
    {{10, 40}, {50, 60}},
  },
  {
    "Join end",
    {{10, 20}, {30, 40}, {50, 60}, {39, 51}},
    {{10, 20}, {30, 60}},
  },
  {
    "Join all",
    {{10, 20}, {30, 40}, {50, 60}, {19, 51}},
    {{10, 60}},
  },
  {
    "Join more start, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 31}},
    {{ 9, 40}, {50, 60}},
  },
  {
    "Join more start, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 51}},
    {{10, 20}, {29, 60}},
  },
  {
    "Join more end, begin",
    {{10, 20}, {30, 40}, {50, 60}, {11, 41}},
    {{10, 41}, {50, 60}},
  },
  {
    "Join more end, end",
    {{10, 20}, {30, 40}, {50, 60}, {31, 61}},
    {{10, 20}, {30, 61}},
  },
  {
    "Join more both, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 41}},
    {{9, 41}, {50, 60}},
  },
  {
    "Join more both, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 61}},
    {{10, 20}, {29, 61}},
  },
  {
    "Join more both, all",
    {{10, 20}, {30, 40}, {50, 60}, {9, 61}},
    {{ 9, 61}},
  },
};

static param_t merge_touching[] = {
  //////// MERGE_TOUCHING ////////
  {
    "Join begin",
    {{10, 20}, {30, 40}, {50, 60}, {20, 30}},
    {{10, 40}, {50, 60}},
  },
  {
    "Join end",
    {{10, 20}, {30, 40}, {50, 60}, {40, 50}},
    {{10, 20}, {30, 60}},
  },
  {
    "Join all",
    {{10, 20}, {30, 40}, {50, 60}, {20, 50}},
    {{10, 60}},
  },
  {
    "Join more start, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 30}},
    {{ 9, 40}, {50, 60}},
  },
  {
    "Join more start, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 50}},
    {{10, 20}, {29, 60}},
  },
  {
    "Join more end, begin",
    {{10, 20}, {30, 40}, {50, 60}, {20, 41}},
    {{10, 41}, {50, 60}},
  },
  {
    "Join more end, end",
    {{10, 20}, {30, 40}, {50, 60}, {40, 61}},
    {{10, 20}, {30, 61}},
  },
  {
    "Join more both, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 41}},
    {{9, 41}, {50, 60}},
  },
  {
    "Join more both, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 61}},
    {{10, 20}, {29, 61}},
  },
};

static param_t keep_touching[] = {
  //////// KEEP_TOUCHING ////////
  {
    "Do not Join begin",
    {{10, 20}, {30, 40}, {50, 60}, {20, 30}},
    {{10, 20}, {20, 30}, {30, 40}, {50, 60}},
  },
  {
    "Do not Join end",
    {{10, 20}, {30, 40}, {50, 60}, {40, 50}},
    {{10, 20}, {30, 40}, {40, 50}, {50, 60}},
  },
  {
    "Do not Join all",
    {{10, 20}, {30, 40}, {50, 60}, {20, 50}},
    {{10, 20}, {20, 50}, {50, 60}},
  },
  {
    "Do not Join more start, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 30}},
    {{ 9, 30}, {30, 40}, {50, 60}},
  },
  {
    "Do not Join more start, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 50}},
    {{10, 20}, {29, 50}, {50, 60}},
  },
  {
    "Do not Join more end, begin",
    {{10, 20}, {30, 40}, {50, 60}, {20, 41}},
    {{10, 20}, {20, 41}, {50, 60}},
  },
  {
    "Do not Join more end, end",
    {{10, 20}, {30, 40}, {50, 60}, {40, 61}},
    {{10, 20}, {30, 40}, {40, 61}},
  },
  {
    "Join end points, begin",
    {{10, 20}, {30, 40}, {50, 60}, {10, 40}},
    {{10, 40}, {50, 60}},
  },
  {
    "Join end points, end",
    {{10, 20}, {30, 40}, {50, 60}, {30, 60}},
    {{10, 20}, {30, 60}},
  },
  {
    "Join more both, begin",
    {{10, 20}, {30, 40}, {50, 60}, {9, 41}},
    {{9, 41}, {50, 60}},
  },
  {
    "Join more both, end",
    {{10, 20}, {30, 40}, {50, 60}, {29, 61}},
    {{10, 20}, {29, 61}},
  },
};

template<typename T, bool B>
void test(RangeSet<T, B> & set, const data_case_t<param_t> & data_case){
  for(auto && param:data_case){
    SECTION(param.name){
      for(auto && p:param.inserted){
        set.insert(p);
      }
      assert_state(set);
      assert_rangeset_equals(param.expected, set);
    }
  }
}

}

namespace remove {

struct param_t {
  const char * name;
  const std::initializer_list<std::pair<int, int> > inserted;
  const std::initializer_list<std::pair<int, int> > removed;
  const std::initializer_list<std::pair<int, int> > expected;
};

static param_t trivial[] = {
  {
    "Remove nothing to nothing is still nothing",
    {},
    {},
    {},
  },
  {
    "Remove anything to nothing is still nothing",
    {},
    {{10, 20}},
    {},
  },
  {
    "Remove multiple anything to nothing is still nothing",
    {},
    {{10, 20}, {20, 30}},
    {},
  },
};

static param_t one_range[] = {
  {
    "Remove before",
    {{10, 20}},
    {{2, 8}},
    {{10, 20}},
  },
  {
    "Remove after",
    {{10, 20}},
    {{25, 30}},
    {{10, 20}},
  },
  {
    "Remove touching before",
    {{10, 20}},
    {{8, 10}},
    {{10, 20}},
  },
  {
    "Remove touching after",
    {{10, 20}},
    {{20, 22}},
    {{10, 20}},
  },
  {
    "Remove middle",
    {{10, 20}},
    {{12, 18}},
    {{10, 12}, {18, 20}},
  },
  {
    "Remove overlap before",
    {{10, 20}},
    {{5, 12}},
    {{12, 20}},
  },
  {
    "Remove overlap after",
    {{10, 20}},
    {{18, 25}},
    {{10, 18}},
  },
  {
    "Remove exact begin",
    {{10, 20}},
    {{10, 12}},
    {{12, 20}},
  },
  {
    "Remove exact end",
    {{10, 20}},
    {{18, 20}},
    {{10, 18}},
  },
  {
    "Remove exact",
    {{10, 20}},
    {{10, 20}},
    {},
  },
  {
    "Remove contained",
    {{10, 20}},
    {{8, 22}},
    {},
  },
};

static param_t three_ranges[] = {
  {
    "Remove begin overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 22}},
    {{30, 40}, {50, 60}},
  },
  {
    "Remove begin exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 20}},
    {{30, 40}, {50, 60}},
  },
  {
    "Remove end exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{50, 60}},
    {{10, 20}, {30, 40}},
  },
  {
    "Remove middle overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 42}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove end overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{48, 62}},
    {{10, 20}, {30, 40}},
  },
  {
    "Remove middle begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{22, 28}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{42, 48}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching before, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 28}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching after, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{22, 30}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching both, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 30}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching before, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{40, 48}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching after, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{42, 50}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle touching both, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{40, 50}},
    {{10, 20}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle overlap before, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 28}},
    {{10, 18}, {30, 40}, {50, 60}},
  },
  {
    "Remove middle overlap after, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{22, 32}},
    {{10, 20}, {32, 40}, {50, 60}},
  },
  {
    "Remove middle overlap both, begin",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 32}},
    {{10, 18}, {32, 40}, {50, 60}},
  },
  {
    "Remove middle overlap before, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{38, 48}},
    {{10, 20}, {30, 38}, {50, 60}},
  },
  {
    "Remove middle overlap after, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{42, 52}},
    {{10, 20}, {30, 40}, {52, 60}},
  },
  {
    "Remove middle overlap both, end",
    {{10, 20}, {30, 40}, {50, 60}},
    {{38, 52}},
    {{10, 20}, {30, 38}, {52, 60}},
  },
  // REMOVE MIDDLE
  {
    "Remove middle exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 40}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle exact overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 42}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle overlap exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 40}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle overlap overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 42}},
    {{10, 20}, {50, 60}},
  },
  // REMOVE MIDDLE BEGIN
  {
    "Remove middle exact, exact previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 40}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle overlap, exact previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 42}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle exact, overlap previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 40}},
    {{10, 18}, {50, 60}},
  },
  {
    "Remove middle overlap, overlap previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 42}},
    {{10, 18}, {50, 60}},
  },
  // REMOVE MIDDLE END
  {
    "Remove middle exact, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 50}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle overlap, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 50}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle exact, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 52}},
    {{10, 20}, {52, 60}},
  },
  {
    "Remove middle overlap, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 52}},
    {{10, 20}, {52, 60}},
  },
  // REMOVE MIDDLE BOTH
  {
    "Remove middle exact previous, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 50}},
    {{10, 20}, {50, 60}},
  },
  {
    "Remove middle overlap previous, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 50}},
    {{10, 18}, {50, 60}},
  },
  {
    "Remove middle exact previous, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 52}},
    {{10, 20}, {52, 60}},
  },
  {
    "Remove middle overlap previous, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 52}},
    {{10, 18}, {52, 60}},
  },
  {
    "Remove all exact exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 60}},
    {},
  },
  {
    "Remove all overlap exact",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 60}},
    {},
  },
  {
    "Remove all exact overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 62}},
    {},
  },
  {
    "Remove all overlap overlap",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 62}},
    {},
  },
  // REMOVE BEGIN MIDDLE
  {
    "Remove begin exact, exact after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 30}},
    {{30, 40}, {50, 60}},
  },
  {
    "Remove begin overlap, exact after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 30}},
    {{30, 40}, {50, 60}},
  },
  {
    "Remove begin exact, overlap after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 32}},
    {{32, 40}, {50, 60}},
  },
  {
    "Remove begin overlap, overlap after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 32}},
    {{32, 40}, {50, 60}},
  },
  {
    "Remove begin middle exact, exact after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 40}},
    {{50, 60}},
  },
  {
    "Remove begin middle overlap, exact after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 40}},
    {{50, 60}},
  },
  {
    "Remove begin middle exact, overlap after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 42}},
    {{50, 60}},
  },
  {
    "Remove begin middle overlap, overlap after",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 42}},
    {{50, 60}},
  },
  {
    "Remove begin middle exact, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 50}},
    {{50, 60}},
  },
  {
    "Remove begin middle overlap, exact next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 50}},
    {{50, 60}},
  },
  {
    "Remove begin middle exact, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{10, 52}},
    {{52, 60}},
  },
  {
    "Remove begin middle overlap, overlap next",
    {{10, 20}, {30, 40}, {50, 60}},
    {{8, 52}},
    {{52, 60}},
  },
  // REMOVE END MIDDLE
  {
    "Remove end exact, exact before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{40, 60}},
    {{10, 20}, {30, 40}},
  },
  {
    "Remove end overlap, exact before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{40, 62}},
    {{10, 20}, {30, 40}},
  },
  {
    "Remove end exact, overlap before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{38, 60}},
    {{10, 20}, {30, 38}},
  },
  {
    "Remove end overlap, overlap before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{38, 62}},
    {{10, 20}, {30, 38}},
  },
  {
    "Remove end middle exact, exact before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 60}},
    {{10, 20}},
  },
  {
    "Remove end middle overlap, exact before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{30, 62}},
    {{10, 20}},
  },
  {
    "Remove end middle exact, overlap before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 60}},
    {{10, 20}},
  },
  {
    "Remove end middle overlap, overlap before",
    {{10, 20}, {30, 40}, {50, 60}},
    {{28, 62}},
    {{10, 20}},
  },
  {
    "Remove end middle exact, exact previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 60}},
    {{10, 20}},
  },
  {
    "Remove end middle overlap, exact previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{20, 62}},
    {{10, 20}},
  },
  {
    "Remove end middle exact, overlap previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 60}},
    {{10, 18}},
  },
  {
    "Remove end middle overlap, overlap previous",
    {{10, 20}, {30, 40}, {50, 60}},
    {{18, 62}},
    {{10, 18}},
  },
};

template<typename T, bool B>
void test(RangeSet<T, B> & set, const data_case_t<param_t> & data_case){
  for(auto && param:data_case){
    SECTION(param.name){
      for(auto && p:param.inserted){
        set.insert(p);
      }
      for(auto && p:param.removed){
        set.remove(p);
      }
      assert_state(set);
      assert_rangeset_equals(param.expected, set);
    }
  }
}

}

TEST_CASE("rangeset merge touching"){
  RangeSet<int> set;
  SECTION("insert"){
    // Tests insert(pair), insert(start, end), const_iterator, cbegin, cend, size, 
    using namespace insert;
    data_case_t<param_t> data;
    SECTION("simple"){
      data = simple;
      test(set, data);
    }
    SECTION("common"){
      data = common;
      test(set, data);
    }
    SECTION("merge_touching"){
      data = merge_touching;
      test(set, data);
    }
  }
  SECTION("remove"){
    // Tests remove(pair), remove(start, end) 
    using namespace remove;
    data_case_t<param_t> data;
    SECTION("trivial"){
      data = trivial;
      test(set, data);
    }
    SECTION("1_range"){
      data = one_range;
      test(set, data);
    }
    SECTION("3_ranges"){
      data = three_ranges;
      test(set, data);
    }
  }
}

TEST_CASE("rangeset keep touching"){
  RangeSet<int, false> set;
  SECTION("insert"){
    // Tests insert(pair), insert(start, end), const_iterator, cbegin, cend, size, 
    using namespace insert;
    data_case_t<param_t> data;
    SECTION("simple"){
      data = simple;
      test(set, data);
    }
    SECTION("common"){
      data = common;
      test(set, data);
    }
    SECTION("keep_touching"){
      data = keep_touching;
      test(set, data);
    }
  }
  SECTION("remove"){
    // Tests remove(pair), remove(start, end) 
    using namespace remove;
    data_case_t<param_t> data;
    SECTION("trivial"){
      data = trivial;
      test(set, data);
    }
    SECTION("1_range"){
      data = one_range;
      test(set, data);
    }
    SECTION("3_range"){
      data = three_ranges;
      test(set, data);
    }
  }
}


TEST_CASE("rangeset merge touching insert2"){
    RangeSet<int> set{};
    assert_state(set);
    assert_rangeset_equals({}, set);
    set.insert(10, 20);
    assert_state(set);
    assert_rangeset_equals({
      {10, 20},
    }, set);
    
    set.insert(30, 40);
    assert_state(set);
    assert_rangeset_equals({
      {10, 20},
      {30, 40},
    }, set);
    
    set.insert(50, 60);
    set.insert(70, 80);
    set.insert(90, 100);
    
    assert_state(set);
    assert_rangeset_equals({
      {10, 20},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 100},
    }, set);
    
    set.insert(30, 40);
    assert_state(set);
    assert_rangeset_equals({
      {10, 20},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 100},
    }, set);
    
    set.insert(8, 10);
    assert_state(set);
    assert_rangeset_equals({
      {8, 20},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 100},
    }, set);

    set.insert(6, 10);
    assert_state(set);
    assert_rangeset_equals({
      {6, 20},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 100},
    }, set);
    
    set.insert(5, 22);
    assert_state(set);
    assert_rangeset_equals({
      {5, 22},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 100},
    }, set);
    
    set.insert(100, 105);
    assert_state(set);
    assert_rangeset_equals({
      {5, 22},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 105},
    }, set);
    
    set.insert(100, 107);
    assert_state(set);
    assert_rangeset_equals({
      {5, 22},
      {30, 40},
      {50, 60},
      {70, 80},
      {90, 107},
    }, set);

    set.insert(88, 110);
    assert_state(set);
    assert_rangeset_equals({
      {5, 22},
      {30, 40},
      {50, 60},
      {70, 80},
      {88, 110},
    }, set);
    
    set.insert(22, 30);
    assert_state(set);
    assert_rangeset_equals({
      {5, 40},
      {50, 60},
      {70, 80},
      {88, 110},
    }, set);
    
    set.insert(20, 55);
    assert_state(set);
    assert_rangeset_equals({
      {5, 60},
      {70, 80},
      {88, 110},
    }, set);
    
    set.insert(63, 67);
    assert_state(set);
    assert_rangeset_equals({
      {5, 60},
      {63, 67},
      {70, 80},
      {88, 110},
    }, set);
    
    set.insert(5, 67);
    assert_state(set);
    assert_rangeset_equals({
      {5, 67},
      {70, 80},
      {88, 110},
    }, set);
    
    set.insert(2, 115);
    assert_state(set);
    assert_rangeset_equals({
      {2, 115},
    }, set);
  }  


TEST_CASE("rangeset merge touching delete2"){
    RangeSet<int> set{};
    assert_state(set);
    assert_rangeset_equals({}, set);
    
    set.remove(20, 50);
    assert_state(set);
    assert_rangeset_equals({}, set);
    
    set.insert(10, 200);
    assert_state(set);
    assert_rangeset_equals({
      {10, 200},
    }, set);
    
    set.remove(100, 150);
    assert_state(set);
    assert_rangeset_equals({
      {10, 100},
      {150, 200},
    }, set);
    
    set.remove(90, 110);
    assert_state(set);
    assert_rangeset_equals({
      {10, 90},
      {150, 200},
    }, set);
    
    set.remove(120, 170);
    assert_state(set);
    assert_rangeset_equals({
      {10, 90},
      {170, 200},
    }, set);
    
    set.remove(85, 90);
    assert_state(set);
    assert_rangeset_equals({
      {10, 85},
      {170, 200},
    }, set);
    
    set.remove(170, 175);
    assert_state(set);
    assert_rangeset_equals({
      {10, 85},
      {175, 200},
    }, set);
    
    set.remove(180, 185);
    assert_state(set);
    assert_rangeset_equals({
      {10, 85},
      {175, 180},
      {185, 200},
    }, set);
    
    set.remove(70, 80);
    assert_state(set);
    assert_rangeset_equals({
      {10, 70},
      {80, 85},
      {175, 180},
      {185, 200},
    }, set);
    
    set.remove(50, 190);
    assert_state(set);
    assert_rangeset_equals({
      {10, 50},
      {190, 200},
    }, set);
    
    set.remove(0, 300);
    assert_state(set);
    assert_rangeset_equals({}, set);
}

}
