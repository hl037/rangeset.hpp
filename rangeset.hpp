#include <iterator>
#include <set>
#include <utility>


template <typename T>
class RangeSet{
  private:
  struct end_point_t{
    T v;
    enum {
      LOWER=0,
      UPPER=1,
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

  // interval is [start, end[
  void insert(T start, T end){
    auto && upper = data.upper_bound({end, end_point_t::UPPER});
    // At the container begining
    if(upper == data.begin()){
      data.insert(data.begin(), {end, end_point_t::UPPER});
      data.insert(data.begin(), {start, end_point_t::LOWER});
      return;
    }

    if(upper == data.end() or upper->dir == end_point_t::LOWER){
      if(std::prev(upper)->v != end){
        data.insert(upper, {end, end_point_t::UPPER});
      }
      --upper;
    }
    
    auto && lower = data.upper_bound({start, end_point_t::LOWER});

    if((lower == upper || lower->dir == end_point_t::LOWER) && lower->v != start){
      data.insert(lower, {start, end_point_t::LOWER});
    }
    
    if(lower != upper){
      data.erase(lower, upper);
    }
  }
  
  inline void insert(const std::pair<T,T> & range){
    insertRange(range.first, range.second);
  }
  
  
  // interval is [start, end[
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
    insertRange(range.first, range.second);
  }

  inline void erase(const_iterator it_begin, const_iterator it_end){
    data.erase(it_begin._sub, it_end._sub);
  }

  inline void erase(const_iterator it){
    erase(it, ++it);
  }

  const_iterator find(const T & v){
    auto && lower = data.lower_bound({v, end_point_t::LOWER});
    if(lower != data.end() && lower->dir == end_point_t::LOWER){
      return const_iterator(lower, data.end());
    }
    else {
      return cend();
    }
  }
  
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

  inline size_t size() const { return data.size() / 2; }

  inline void insertRange(std::pair<T, T> range) { insertRange(range.first, range.second); }
  
  inline const_iterator cbegin() const { return const_iterator{data.begin(), data.end()}; }
  inline const_iterator cend() const { return const_iterator{data.end(), data.end()}; }

public:
  RangeSet()=default;
  ~RangeSet()=default;
  
};

