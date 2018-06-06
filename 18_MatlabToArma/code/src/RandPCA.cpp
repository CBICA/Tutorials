/**
\file RandPCA.cpp

\brief Randomized PCA Implementation

*/

#include "RandPCA.h"


RandPCA::RandPCA()
{

}

RandPCA::RandPCA(const RandPCA &origin)
{
  A = origin.A;
  U = origin.U;
  V = origin.V;
  S = origin.S; 

  k = origin.k; 
  its = origin.its;
  l = origin.l;

  m = origin.m;
  n = origin.n;

  raw = origin.raw;
  algorithmDone = origin.algorithmDone; 
}

void RandPCA::Run()
{
  if (!algorithmDone)
  {
    if (!sanityCheck_matrix(A))
    {
      std::cerr << "A is invalid.\n";
      exit(EXIT_FAILURE);
    }
    if (raw) // A is not to be centered
    {
      // SVD 'A' directly if (l >= m/1.25) or (l >= n/1.25)
      if ((l >= m / 1.25) || (l >= n / 1.25))
      {
        if (sparseMatEnabled)
        {
          arma::svd_econ(U, S, V, arma::mat(A));
        }
        else
        {
          arma::svd_econ(U, S, V, A);
        }
      }
      else if (m >= n)
      {
        arma::mat Q, R;
        // Apply 'A' to a random matrix, obtaining 'Q' -- assuming 'A' to be always real
        Q = A * (2 * arma::randn(n, n) - arma::ones(n, n));

        //// if not real, do this
        //Q = A * (2 * arma::randn(n, n) - arma::ones(n, n)) + i * ((2 * arma::randn(n, n) - arma::ones(n, n)));

        // Form a matrix 'Q' whose columns constitute a well-conditioned basis for the columns of the earlier 'Q'
        if (its == 0)
        {
          arma::qr_econ(Q, R, A);
        }
        else
        {
          arma::lu(Q, R, A);
        }

        // Conduct normalized power iterations
        for (size_t i = 0; i <= its; i++)
        {
          Q = (Q.t() * A).t();

          arma::lu(Q, R, Q);

          Q = A * Q;

          if (i < its)
          {
            arma::lu(Q, R, Q);
          }
          else
          {
            arma::qr_econ(Q, R, Q);
          }
        }

        // SVD (Q'*A) to obtain approximations to the singular values and right singular vectors of A; 
        // adjust the left singular vectors of (Q'*A) to approximate the left singular vectors of A.
        arma::svd_econ(R, S, V, Q.t() * A);
        U = Q * R;
      }
      else if (m < n)
      {
        arma::mat Q, R;
        // Apply 'A' to a random matrix, obtaining 'Q' -- assuming 'A' to be always real
        Q = ((2 * arma::randn(m, m) - arma::ones(m, m)) * A).t();

        //// if not real, do this
        //Q = (((2 * arma::randn(m, m) - arma::ones(m, m)) + i * (2 * arma::randn(m, m) - arma::ones(m, m))) * A).t();

        // Form a matrix 'Q' whose columns constitute a well-conditioned basis for the columns of the earlier 'Q'
        if (its == 0)
        {
          arma::qr_econ(Q, R, A);
        }
        else
        {
          arma::lu(Q, R, A);
        }

        // Conduct normalized power iterations
        for (size_t i = 0; i <= its; i++)
        {
          Q = A * Q;

          arma::lu(Q, R, Q);

          Q = (Q.t() * A).t();

          if (i < its)
          {
            arma::lu(Q, R, Q);
          }
          else
          {
            arma::qr_econ(Q, R, Q);
          }
        }

        // SVD (Q'*A) to obtain approximations to the singular values and right singular vectors of A; 
        // adjust the left singular vectors of (Q'*A) to approximate the left singular vectors of A.
        arma::svd_econ(U, S, R, A * Q);
        V = Q * R;
      }
    }
    else  // A is to be centered
    {
      double c = arma::accu(A) / m;

      // SVD the centered A directly if l >= m/1.25 or l >= n/1.25
      if ((l >= m / 1.25) || (l >= n / 1.25))
      {
        if (sparseMatEnabled)
        {
          arma::svd_econ(U, S, V, arma::mat(A) - arma::ones(m, m) * c);
        }
        else
        {
          arma::svd_econ(U, S, V, A - arma::ones(m, m) * c);
        }
      }
      else if (m >= n)
      {
        arma::mat Q, R;
        // Apply the centered 'A' to a random matrix, obtaining 'Q' -- assuming it is real
        R = 2 * arma::randn(n, n) - arma::ones(n, n);

        //// for complex numbers
        //R = 2 * arma::randn(n, n) - arma::ones(n, n) + i * 2 * arma::randn(n, n) - arma::ones(n, n);

        Q = A * R - arma::ones(m, m) * (c * R);

        // Form a matrix 'Q' whose columns constitute a well-conditioned basis for the columns of the earlier 'Q'
        if (its == 0)
        {
          arma::qr_econ(Q, R, Q);
        }
        else
        {
          arma::lu(Q, R, Q);
        }

        // Conduct normalized power iterations
        for (size_t i = 0; i <= its; i++)
        {
          R = Q.t();

          Q = (R * A - (R * arma::ones(m, m)) * c).t();

          arma::lu(Q, R, Q);

          Q = A * Q - arma::ones(m, m) * (c * Q);

          if (i < its)
          {
            arma::lu(Q, R, Q);
          }
          else
          {
            arma::qr_econ(Q, R, Q);
          }
        }

        // SVD Q.t() applied to the centered 'A' to obtain approximations to the singular values and right singular vectors of the centered A;
        // adjust the left singular vectors to approximate the left singular vectors of the centered 'A'
        R = Q.t();

        arma::svd_econ(R, S, V, R * A - (R * arma::ones(m, m) * c));

        U = Q * R;
      }
      else if (m < n)
      {
        arma::mat Q, R;
        // Apply the adjoint of the centered 'A' to a random matrix, obtaining 'Q'
        // for real case
        R = 2 * arma::randn(m, m) - arma::ones(m, m);

        // for complex
        //R = 2 * arma::randn(m, m) - arma::ones(m, m) + i * arma::randn(m, m) - arma::ones(m, m);

        Q = (R * A - (R * arma::ones(m, m))*c).t();

        // Form a matrix 'Q' whose columns constitute a well-conditioned basis for the columns of the earlier 'Q'
        if (its == 0)
        {
          arma::qr_econ(Q, R, Q);
        }
        else
        {
          arma::lu(Q, R, Q);
        }

        // Conduct normalized power iterations
        for (size_t i = 0; i <= its; i++)
        {
          Q = A * Q - arma::ones(m, m)*(c * Q);

          arma::lu(Q, R, Q);

          R = Q.t();
          Q = (R*A - (R*arma::ones(m, m))*c).t();

          if (i < its)
          {
            arma::lu(Q, R, Q);
          }

          if (i == its)
          {
            arma::qr_econ(Q, R, Q);
          }
        }

        // SVD the centered 'A' applied to 'Q' to obtain approximations to the singular values and left singular vectors of the centered 'A';
        // adjust the right singular vectors to approximate the right singular vectors of the centered 'A'
        arma::svd_econ(U, S, R, A*Q - arma::ones(m, m)*(c*Q));
        V = Q * R;
      }
    }

    // Retain only the leftmost 'k' columns of 'U', the leftmost 'k' columns of 'V', and the uppermost leftmost 'kxk' block of 'S'
    arma::uword sizeToPreserve = static_cast<arma::uword>(k - 1);
    U = U.cols(0, sizeToPreserve);
    V = V.cols(0, sizeToPreserve);
    S = S(arma::span(0, sizeToPreserve), arma::span(0, sizeToPreserve));

    algorithmDone = true;
  }
}

std::tuple< arma::mat, arma::vec, arma::mat > RandPCA::GetOutputs()
{
  if (!algorithmDone)
  {
    Run();
  }
  return std::tie(U, S, V);
}
