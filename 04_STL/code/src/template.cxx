/**
\file template.cxx

\brief 07_STL: Template example

This program illustrates usage of function template
*/
#include <iostream>

// define template function
template <typename VarType>
VarType add( VarType a, VarType b )
{
  return (a+b);
}

int  main()
{
  // initialize variables
  int a = 1, b = 2;
  float x = 1.0, y = 2.0;
  double m = 3.0, n = 4.0;
  char p = 1, q = 2;
  
  // perform operation
  int c = add< int >( a, b );
  float z = add< float >( x, y );
  double o = add< double >( m, n );
  char r = add< char >( p, q );

  // display results
  std::cout << "Int result between " << a << " and " << b << " : " << c << "\n";
  std::cout << "Float result " << x << " and " << y << " : " << z << "\n";
  std::cout << "Double result " << m << " and " << n << " : " << o << "\n";
  std::cout << "Char result " << p << " and " << q << " : " << r << "\n";

  return EXIT_SUCCESS;
}

