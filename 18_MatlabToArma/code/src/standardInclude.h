/**
\file standardInclude.h

\brief Provides standard includes for all files to use in this project along with some basic functions and variables.
*/
# pragma once

#include <algorithm>
#include <type_traits>
#include <tuple>

#include "armadillo"

using DefaultDataType = double; //! self-explanatory
using ArmaDefaultMat = arma::Mat< DefaultDataType >; //! self-explanatory

static bool sparseMatEnabled; //! flag that triggers sparse computations

/**
\brief Sanity check of arma::matrix

Current checks:
  1. is_finite()
  2. is_negative()
  3. size == 0,0

\param inputMatrix The input Matrix
*/
static bool sanityCheck_matrix(const ArmaDefaultMat &inputMatrix)
{
  if (!inputMatrix.is_finite())
  {
    std::cerr << "Input matrix has NaN or Inf numbers.\n";
    //exit(EXIT_FAILURE);
    return false;
  }
  if (!arma::any(arma::vectorise(inputMatrix) < 0)) // vectorise is needed for arma semantics
  {
    std::cerr << "Input matrix should contain non-negative values.\n";
    //exit(EXIT_FAILURE);
    return false;
  }
  if ((inputMatrix.n_cols == 0) || (inputMatrix.n_rows == 0))
  {
    std::cerr << "The size of the matrix must be valid. Either the number of rows or cols is '0' right now.\n";
    return false;
  }

  return true;
}