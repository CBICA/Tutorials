#include <iostream>
#include <memory>
#include <stdlib.h>
 
void foo( int *p ) 
{ 
  std::cout << *p << "\n";
} 

void bar( int * p ) 
{ 
  ++(*p); 
} 
 
int main()
{
  std::shared_ptr< int > p1( new int( 42 ) );
  std::shared_ptr< int > p2 = p1; 

  bar( p1.get() ); 
  foo( p2.get( ) );
	
	return EXIT_SUCCESS;
}
