/**
\brief 03_OOP: Object oriented programming example

This program illustrates some basic OOP concepts like classes, inheritance,
encapsulation, etc.
*/
#include "base.h"
#include "add.h"
#include "difference.h"

int main()
{
  int x,y;
  std::cout << "\nEnter two numbers separated by space {press Ctrl+Z to Exit}:\n";
  std::cin >> x >> y;
  add addition_object(x, y);
  difference difference_object(x,y);

  base *m;
  m = &addition_object;
  m->op();
  m->getResult();
  std::cout << "Result of Addition operation = " << m->getResult() << std::endl;

  m = &difference_object;
  m->op();
  m->getResult();
  std::cout << "Result of Difference operation = " << m->getResult() << std::endl;

  m = NULL;

  std::cin.ignore();
  std::cin.get();

  return EXIT_SUCCESS;
}