# RangeSet

This header-only library provides a `RangeSet` class similar to the java google guava one.

it is used like this :

```
RangeSet<int>() set;
set.insert(20, 40);

set.insert(30, 50);

set.insert(80, 90);

set.remove(22, 28);

std::cout<<(set.find(25) != set.end)<<endl; // 0
std::cout<<(set.find(20) != set.end)<<endl; // 1

std::cout<<(set.find(25, 30) != set.end)<<endl; // 0
std::cout<<(set.find(30, 35) != set.end)<<endl; // 1

for(const std::pair<int, int> & r : set){
  std::cout<<"[ "<<r.first<<" , "<<r.second<<")"<<std::endl;
}
// [ 20 , 22 )
// [ 28 , 50 )
// [ 80 , 90 )
```


You can build and run the tests with :
```
make all
make run
```

...It will also generate coverage informations.

The library is still under developpement and features like merge, set difference are coming.

