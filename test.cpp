#include <initializer_list>
#include <vector>
#define private public
#define protected public
#include "rangeset.hpp"
#undef private
#undef protected

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

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

template <typename T>
void assert_rangeset_equals(const std::initializer_list<std::pair<T, T> > & expected, const RangeSet<T> & set){
  REQUIRE(expected.size() == set.size());
  auto && it1 = expected.begin(), end1 = expected.end();
  auto && it2 = set.cbegin(), end2 = set.cend();
  for(; it1 != end1 && it2 != end2; ++it1, ++it2) {
    REQUIRE(it2->first == it1->first);
    REQUIRE(it2->second == it1->second);
  }
}

template <typename T>
void assert_state(const RangeSet<T> & set){
  REQUIRE(set.data.size() % 2 == 0 );
  if(set.data.size() % 2){
    return;
  }
  auto && it = set.data.begin(), end = set.data.end();
  while(it != end) {
    REQUIRE((int) it++->dir == (int) RangeSet<T>::end_point_t::LOWER);
    REQUIRE((int) it++->dir == (int) RangeSet<T>::end_point_t::UPPER);
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

template<typename T>
void test(RangeSet<T> & set, const data_case_t<param_t> & data_case){
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

TEST_CASE("rangeset merge touching"){
  using namespace insert;
  RangeSet<int> set;
  SECTION("insert"){
    data_case_t<param_t> data;
    SECTION("simple"){
      data = simple;
      test(set, data);
    }
    SECTION("common"){
      data = common;
      test(set,data);
    }
    SECTION("merge_touching"){
      data = merge_touching;
      test(set,data);
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
