
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

template <typename T>
struct data_case_t
{
  typedef typename T::param_t param_t;
  typedef T ns;
  private:
    const param_t * start;
    size_t size;
  public:

  data_case_t()=default;
  ~data_case_t()=default;

  template<typename B>
  data_case_t(const B & collection) :
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

  const param_t* begin() const { return start; }
  const param_t* end () const { return &start[size]; }
};

template<typename T, typename... Args>
void test(const data_case_t<T> & data_case, Args&&... args){
	for(auto && param:data_case){
		SECTION(param.name){
			T::test(param, std::forward<Args>(args)...);
		}
	}
}

#define BEGIN_PARAMS_SECTION(a) \
  SECTION(#a) {\
    typedef data_case_t<a> _data_case_t;

#define END }

#define DATA_CASE(a) \
  SECTION(#a){ \
    test(_data_case_t(_data_case_t::ns::a), set);\
  }

#define DECLARE_PARAMS_SECTION(a) struct a

#define DECLARE_PARAM_TYPE struct param_t
#define PARAM_TYPE const param_t &

#define DECLARE_DATA_CASE(a) static constexpr param_t a[] =

#define DATA_CASE_TEST(...) static void test(__VA_ARGS__)

