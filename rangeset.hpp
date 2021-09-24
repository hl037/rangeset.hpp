#include <iterator>
#include <set>
#include <utility>

/**
 * Range set ot type T.
 *
 * A range set is a set comprising zero or more nonempty, disconnected ranges of type T.
 *
 * This class supports adding and removing ranges from the set, and testing if a given object or range is contained in the set.
 */
template <typename T, bool MERGE_TOUCHING=true>
class RangeSet{
  private:
  /** \internal
   *  Representation of an end_point (lower/upper bound) of a range.
   *  RangeSet should alternate lower and upper bounds.
   */
  struct end_point_t{
    T v;
    enum {
      LOWER=MERGE_TOUCHING ? 0 : 1,
      UPPER=1-LOWER,
    } dir;
    bool operator<(const end_point_t & oth) const{
      return v == oth.v ? dir < oth.dir : v < oth.v;
    }
    bool operator ==(const end_point_t & oth) const{
      return v == oth.v && dir == oth.dir;
    }
  };

  std::set<end_point_t> data;

  public:
  /**
   *  The iterator is bidirectionnal. Its dereferenced value is a std::pair<T, T>.
   */
  struct const_iterator{
    using difference_type = long;
    using value_type = std::pair<T, T>;
    using pointer = const value_type *;
    using reference = const value_type &;
    using iterator_category = std::bidirectional_iterator_tag;

    using _sub = typename std::set<end_point_t>::const_iterator;
    
    value_type val;
    _sub lower;
    _sub end;
  protected:
    inline void update(){
      if(lower != end){
        val = {lower->v, std::next(lower)->v};
      }
    }
  public:
    inline const_iterator() : lower{} {}
    inline const_iterator(const _sub & lower, const _sub & end) : lower{lower}, end{end}{ update(); }

    inline reference operator*() { return val; }
    inline pointer operator->() { return &val; }
    inline const_iterator & operator++() { ++++lower; update(); return *this; }
    inline const_iterator operator++(int) { const_iterator res{*this}; ++*this; return res; }
    inline const_iterator & operator--() { ----lower; update(); return *this; }
    inline const_iterator operator--(int) { const_iterator res{*this}; --*this; return res; }

    inline bool operator==(const const_iterator & oth) { return lower == oth.lower; }
    inline bool operator!=(const const_iterator & oth) { return !(*this == oth); }
  };

  /**
   *  Add the range [start, end) (or "[start; end[" in other notation) to the set.
   *  If overlap occurs, the ranges are merged. if STRICT_OVERLAP is False, [start, mid) and [mid, end) will be merged to [start, end). Else, they will coeexist.
   */
  void insert(T start, T end){
    if(end <= start){
      return;
    }
    auto && upper = data.upper_bound({end, end_point_t::UPPER}); // end) < upper OR upper == end() 
    // At the container begining
    if(upper == data.begin()){  //    [start , end) < [ upper=begin(), end() )
      data.insert(data.begin(), {end, end_point_t::UPPER});
      data.insert(data.begin(), {start, end_point_t::LOWER});
      return;
    }

    if(upper == data.end() or upper->dir == end_point_t::LOWER){  // ')' < end < '['
      if(std::prev(upper)->v != end){ // if not same value, insert, else just skip and take upper's precedent
        data.insert(upper, {end, end_point_t::UPPER});
      }
      --upper;
    }
    
    auto && lower = data.upper_bound({start, end_point_t::LOWER});//    [start < lower

    if((lower == upper || lower->dir == end_point_t::LOWER) && lower->v != start
        && (lower == data.begin() || std::prev(lower)->dir == end_point_t::UPPER)){
      data.insert(lower, {start, end_point_t::LOWER});
    }
    
    if(lower != upper){
      data.erase(lower, upper);
    }
  }
  
  inline void insert(const std::pair<T,T> & range){
    insert(range.first, range.second);
  }
  
  
  /**
   * Remove the interval [start, end) (or "[start; end[" in other notation) from the set.
   */
  void remove(const T & start, const T & end){
    auto && lower = data.lower_bound({start, end_point_t::LOWER});
    // At the container end
    if(lower == data.end()){
      return; //nothing to do...
    }

    bool lower_inserted = false;
    if(lower->dir == end_point_t::UPPER){
      if(lower->v == start){
        ++lower;
      }
      else{
        data.insert(lower, {start, end_point_t::UPPER});
        --lower;
        lower_inserted = true;
      }
    }
    
    auto && upper = data.lower_bound({end, end_point_t::LOWER});

    if(upper != data.end() && upper->dir == end_point_t::UPPER){
      if(upper->v == end){
        ++upper;
      }
      else{
        data.insert(upper, {end, end_point_t::LOWER});
        --upper;
      }
    }
    
    if(lower_inserted){
      ++lower;
    }
    if(lower != upper){
      data.erase(lower, upper);
    }
  }
  
  inline void remove(const std::pair<T,T> & range){
    remove(range.first, range.second);
  }

  /**
   * Remove unit ranges from the set (could be faster than remove)
   */
  inline void erase(const_iterator it_begin, const_iterator it_end){
    data.erase(it_begin._sub, it_end._sub);
  }

  inline void erase(const_iterator it){
    erase(it, ++it);
  }

  /**
   * Find the unit range that contains a specific value.
   * Returns cend() if not v is not in the set.
   */
  const_iterator find(const T & v){
    auto && lower = data.lower_bound({v, end_point_t::LOWER});
    if(lower != data.end() && lower->dir == end_point_t::LOWER){
      return const_iterator(lower, data.end());
    }
    else {
      return cend();
    }
  }
  
  /**
   * Find the unit range that contains the sub range [start, end) (or [start; end[ )
   */
  const_iterator find(const T & start, const T & end){
    auto && lower = data.lower_bound({start, end_point_t::LOWER});
    auto && upper = ++data.lower_bound({end, end_point_t::UPPER});
    if(lower != data.end() && upper != data.end
      && lower->dir == end_point_t::LOWER
      && upper->dir == end_point_t::UPPER
      && std::next(lower) == upper
    ){
      return const_iterator(lower);
    }
    else {
      return cend();
    }
  }
  inline const_iterator find(const std::pair<T,T> & range){
    return find(range.first, range.second);
  }

  /**
   * Return the number of unit range in the set (The number of iterator beetwin cbegin() and cend())
   */
  inline size_t size() const { return data.size() / 2; }

  /**
   * Return an iterator to the first unit range. When dereferencing an iterator, the value is a std::pair<T,T> describing the interval [ res.first, res.end )
   */
  inline const_iterator cbegin() const { return const_iterator{data.begin(), data.end()}; }
  /**
   * Return a past-the-end iterator of this set.
   */
  inline const_iterator cend() const { return const_iterator{data.end(), data.end()}; }

public:
  RangeSet()=default;
  ~RangeSet()=default;
  
};

