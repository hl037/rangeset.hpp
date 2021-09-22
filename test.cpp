#define private public
#define protected public
#include "rangeset.hpp"
#undef private
#undef protected
#include <criterion/criterion.h>

template <typename T>
void assert_rangeset_equals(const std::initializer_list<std::pair<T, T> > expected, const RangeSet<T> & set){
  cr_assert_eq(expected.size(), set.size());
  auto && it1 = expected.begin(), end1 = expected.end();
  auto && it2 = set.cbegin(), end2 = set.cend();
  for(; it1 != end1 && it2 != end2; ++it1, ++it2) {
    cr_assert_eq(it1->first, it2->first);
    cr_assert_eq(it1->second, it2->second);
  }
}

template <typename T>
void assert_state(const RangeSet<T> & set){
  cr_assert_eq(true, set.data.size() % 2 == 0 );
  if(set.data.size() % 2){
    return;
  }
  auto && it = set.data.begin(), end = set.data.end();
  while(it != end) {
    cr_assert_eq((int) it++->dir, (int) RangeSet<T>::end_point_t::LOWER);
    cr_assert_eq((int) it++->dir, (int) RangeSet<T>::end_point_t::UPPER);
  }
}


Test(rangeset, insert){
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


Test(rangeset, erase){
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
