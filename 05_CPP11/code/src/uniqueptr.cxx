#include <iostream>
#include <memory>
#include <stdlib.h>
 
struct Foo
{
  Foo()      { std::cout << "Foo::Foo\n";  }
  ~Foo()     { std::cout << "Foo::~Foo\n"; }
  void bar() { std::cout << "Foo::bar\n";  }
};
 
void f(const Foo &)
{
  std::cout << "f(const Foo&)\n";
}
 
int main()
{
  std::unique_ptr<Foo> p1(new Foo);  // p1 owns Foo  
  if (p1) p1->bar();  
  
  {  
    std::unique_ptr<Foo> p2(std::move(p1));  // now p2 owns Foo    
    f(*p2);    
    
    p1 = std::move(p2);  // ownership returns to p1. without this, there will be compilation error
    std::cout << "destroying p2...\n";    
  }  
  
  if (p1) p1->bar();  
  
  // Foo instance is destroyed when p1 goes out of scope  
	
	return EXIT_SUCCESS;
}
