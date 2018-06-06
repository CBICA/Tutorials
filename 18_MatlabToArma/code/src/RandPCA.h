/**
\file RandPCA.h

\brief Randomized PCA 


Reference:
N. Halko, PG. Martinsson & J. Tropp; 
Finding structure with randomness: probabilistic algorithms for constructing approximate matrix decompositions
arXiv:0909.4061 [math.NA; math.PR], 2009 (available at http://arxiv.org).

*/
#pragma once

#include "standardInclude.h"

using DefaultDataType = double;
using ArmaDefaultMat = arma::Mat< DefaultDataType >;

class RandPCA
{
public:
  //! Default Constructor
  RandPCA();

  //! Copy Constructor
  RandPCA(const RandPCA &origin);

  //! Destructor
  virtual ~RandPCA()
  {
    // nothing to do, really - all members are smart pointers and will get deleted by themselves
  }

  /**
  \brief Set all inputs

  \param input_A Matrix being approximated
  \param input_k Rank of the approximation being constructed; should be <= smaller dimension of A; defaults to 6
  \param input_raw Centers A when raw is false but does not when raw is true; defaults to false
  \param input_its Number of normalized power iterations to conduct; defaults to 2
  \param input_l Block size of the normalized power iterations; should be >= k; defaults to k+2
  */
  template <bool isSparse = true>
  void SetInputsAndRun(const ArmaDefaultMat &input_A, 
    const size_t input_k = 6, 
    bool input_raw = false, 
    const size_t input_its = 2, 
    const size_t input_l = 8)
  {
    if (!sanityCheck_matrix(input_A))
    {
      std::cerr << "A is invalid.\n";
      exit(EXIT_FAILURE);
    }
    A = input_A;
    if (input_k > std::min(A.n_cols, A.n_rows))
    {
      std::cout << "The input 'k' is greater than the smallest dimensions of 'A', using the smaller A dimension instead.\n";
      k = std::min(A.n_cols, A.n_rows);
    }
    else
    {
      k = input_k;
    }
    m = A.n_rows;
    n = A.n_cols;

    raw = input_raw;
    if (input_its < 0)
    {
      std::cout << "The input 'its' is less than '0', using '2' instead.\n";
      its = 2;
    }
    else
    {
      its = input_its;
    }

    if (input_l < k)
    {
      std::cout << "The input 'l' is less than 'k', using 'k + 2' instead.\n";
      l = k + 2;
    }
    else
    {
      l = input_l;
    }

    sparseMatEnabled = isSparse;
    // at this point, all inputs have been populated 

    Run();
  }

  //! Main algorithm
  void Run();

  /*
  \brief Get the output of the class in the same form as [U, S, V]

  \return U
  \return S
  \return V
  */
  std::tuple< arma::mat, arma::vec, arma::mat > GetOutputs();

private:
  ArmaDefaultMat A; //! Matrix being approximated

  arma::mat U, V; //! Decomposition matrix results
  arma::vec S; //! Decomposition vector results

  size_t k, //! Rank of the approximation being constructed
    its, //! Number of normalized power iterations to conduct
    l; //! Block size of the normalized power iterations
  
  arma::uword m, n; //! Size of 'A'

  bool raw, //! Centers 'A' when false
    algorithmDone; //! self-explanatory
};
