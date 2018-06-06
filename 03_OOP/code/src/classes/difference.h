#pragma once

#include "base.h"
#include <math.h>

class difference:
  public base
{
public:
  //! Default Constructor
  explicit difference();
  
  //! Actual Constructor
  explicit difference(int n1, int n2): base(n1, n2)
  {
  }
  
  //! Implement the virtual function
  void op()
  {
    m_result = std::abs(m_num1 - m_num2);
  }
  
  //! Since destructor is virtual, it need to be implemented
  virtual ~difference()
  {
  }
};