#pragma once

//! Stadard headers
#include <stdio.h>
#include <iostream>

class base
{
public: //! interface of "base"
  
  //! Default Constructor
  explicit base();

  //! Actual Constructor
  explicit base(int n1, int n2)
  {
    m_num1 = n1;
    m_num2 = n2;
  }

  //! Destructor
  virtual ~base()
  {      
  }

  //! set the new variables
  void setNewVariables( int n1, int n2 )
  {
    m_num1 = n1;
    m_num2 = n2;
  }

  //! Pure virtual function
  virtual void op() = 0;

  //! Output function
  int getResult()
  {
    return m_result;
  }

protected:
  int m_num1, m_num2, m_result;
};