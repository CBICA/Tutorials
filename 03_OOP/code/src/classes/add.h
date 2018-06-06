#pragma once

#include "base.h"


class add: 
  public base // add class inherits from "base"
{
public:
  //! Default Constructor
  explicit add();
  
  //! Actual Constructor
  explicit add(int n1, int n2): base(n1, n2)
  {
  }

  //! Implement the virtual function
  void op()
  {
    m_result = m_num1 + m_num2;
  }

  //! Since destructor is virtual, it need to be implemented
  virtual ~add()
  {
  }
};