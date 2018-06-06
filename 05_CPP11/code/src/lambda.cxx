#include <iostream>
#include <memory>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <functional>

int fib( int n ) 
{ 
  int first = 1, second = 1, return_val;
  if( n==1 )
    return first;
  if( n==2 )
    return second;
  for( auto i=3; i<=n; i++ )
  {
    return_val = first + second;
    first = second;
    second = return_val;
  }
  return return_val; 
} 

int fib_recursive( int n ) 
{
  return (n < 2 ? 1 : fib_recursive(n-1) + fib_recursive(n-2));
}

int main()
{
  int fNumber = 10;
  std::function<int(int)> lfib = [&lfib](int n) {return (n < 2 ? 1 : lfib(n-1) + lfib(n-2));};
	
  int test_1 = fib(fNumber); // slowest, low memory
  int test_2 = lfib(fNumber-1); // fastest, high memory
  int test_3 = fib_recursive(fNumber-1); // faster than fib() but significatly slower than lfib()
    
	return EXIT_SUCCESS;
}
