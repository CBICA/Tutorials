#include <iostream>
#include <assert.h>
#include <string>

// Tests a string to see if it is empty or longer than 2 characters
void analyze_string ( std::string inputString )
{
  assert (inputString.length( ) > 2);  // Length must exceed 2
}

int main ( )
{
  std::string test1 = "abc", test2, test3 = "";
  std::cout << "Analyzing string '" << test1 << "'\n'";
  analyze_string (test1);
  std::cout << "Analyzing string '" << test2 << "'\n'";
  analyze_string (test2); // assert obtained here
  std::cout << "Analyzing string '" << test3 << "'\n'";
  analyze_string (test3); // assert obtained here
  return EXIT_SUCCESS;
}
