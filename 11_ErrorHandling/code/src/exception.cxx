#include <iostream>
#include <string>
#include <exception>

int main ( )
{
  std::string test1 = "abc", test2 = "d";
  
  try
  {
    std::cout << "Adding two strings: " << test1 + test2 << "\n";
  }
  catch (std::exception &e)
  {
    std::cerr << "Exception caught: " << e.what() << "\n";
  }
  
  return EXIT_SUCCESS;
}
