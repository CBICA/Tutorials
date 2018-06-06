function [U,S,V] = randpca(A,k,raw,its,l)
%PCA  Principal component analysis
%
%
%   [U,S,V] = PCA(A)  constructs a nearly optimal rank-6 approximation
%             USV' to C(A), where C(A) refers to A after centering its
%             columns, using 2 normalized power iterations, with block
%             size 6+2=8, started with a min(m,n) x 8 random matrix,
%             when A is m x n; the reference below explains "nearly
%             optimal." The smaller dimension of A must be >= 6 when A
%             is the only input to PCA.
%
%   [U,S,V] = PCA(A,k)  constructs a nearly optimal rank-k approximation
%             USV' to C(A), where C(A) refers to A after centering its
%             columns, using 2 normalized power iterations, with block
%             size k+2, started with a min(m,n) x (k+2) random matrix,
%             when A is m x n; the reference below explains "nearly
%             optimal." k must be a positive integer <= the smaller
%             dimension of A.
%
%   [U,S,V] = PCA(A,k,raw)  constructs a nearly optimal rank-k
%             approximation USV' to A, centering the columns of A first
%             when raw is false, using 2 normalized power iterations,
%             with block size k+2, started with a min(m,n) x (k+2)
%             random matrix, when A is m x n; the reference below
%             explains "nearly optimal." k must be a positive integer
%             <= the smaller dimension of A.
%
%   [U,S,V] = PCA(A,k,raw,its)  constructs a nearly optimal rank-k
%             approximation USV' to A, centering the columns of A first
%             when raw is false, using its normalized power iterations,
%             with block size k+2, started with a min(m,n) x (k+2)
%             random matrix, when A is m x n; the reference below
%             explains "nearly optimal." k must be a positive integer
%             <= the smaller dimension of A, and its must be a
%             nonnegative integer.
%
%   [U,S,V] = PCA(A,k,raw,its,l)  constructs a nearly optimal rank-k
%             approximation USV' to A, centering the columns of A first
%             when raw is false, using its normalized power iterations,
%             with block size l, started with a min(m,n) x l random
%             matrix, when A is m x n; the reference below explains
%             "nearly optimal." k must be a positive integer <= the
%             smaller dimension of A, its must be a nonnegative integer,
%             and l must be a positive integer >= k.
%
%
%   The low-rank approximation USV' comes in the form of a singular
%   value decomposition (SVD) -- the columns of U are orthonormal, as
%   are the columns of V, the entries of S are all nonnegative, and all
%   nonzero entries of S appear in non-increasing order on its diagonal.
%   U is m x k, V is n x k, and S is k x k, when A is m x n.
%
%   Increasing its or l improves the accuracy of the approximation USV';
%   the reference below describes how the accuracy depends on its and l.
%   Please note that even its=1 guarantees superb accuracy, whether or
%   not there is any gap in the singular values of the matrix A being
%   approximated, at least when measuring accuracy as the spectral norm
%   ||A-USV'|| of A-USV' (relative to the spectral norm ||A|| of A).
%
%
%   Note: PCA invokes RAND. To obtain repeatable results, reset the seed
%         for the pseudorandom number generator.
%
%   Note: The user may ascertain the accuracy of the approximation USV'
%         to A by invoking DIFFSNORM(A,U,S,V), when raw is true.
%         The user may ascertain the accuracy of the approximation USV'
%         to C(A), where C(A) refers to A after centering its columns,
%         by invoking DIFFSNORMC(A,U,S,V), when raw is false.
%
%
%   inputs (the first is required):
%   A -- matrix being approximated
%   k -- rank of the approximation being constructed;
%        k must be a positive integer <= the smaller dimension of A,
%        and defaults to 6
%   raw -- centers A when raw is false but does not when raw is true;
%          raw must be a Boolean and defaults to false
%   its -- number of normalized power iterations to conduct;
%          its must be a nonnegative integer, and defaults to 2
%   l -- block size of the normalized power iterations;
%        l must be a positive integer >= k, and defaults to k+2
%
%   outputs (all three are required):
%   U -- m x k matrix in the rank-k approximation USV' to A or C(A),
%        where A is m x n, and C(A) refers to A after centering its
%        columns; the columns of U are orthonormal
%   S -- k x k matrix in the rank-k approximation USV' to A or C(A),
%        where A is m x n, and C(A) refers to A after centering its
%        columns; the entries of S are all nonnegative, and all nonzero
%        entries appear in nonincreasing order on the diagonal
%   V -- n x k matrix in the rank-k approximation USV' to A or C(A),
%        where A is m x n, and C(A) refers to A after centering its
%        columns; the columns of V are orthonormal
%
%
%   Example:
%     A = rand(1000,2)*rand(2,1000);
%     A = A/normest(A);
%     [U,S,V] = pca(A,2,true);
%     diffsnorm(A,U,S,V)
%
%     This example produces a rank-2 approximation USV' to A such that
%     the columns of U are orthonormal, as are the columns of V, and
%     all entries of S are nonnegative and are zero off the diagonal.
%     diffsnorm(A,U,S,V) outputs an estimate of the spectral norm
%     of A-USV', which should be close to the machine precision.
%
%
%   Reference:
%   Nathan Halko, Per-Gunnar Martinsson, and Joel Tropp,
%   Finding structure with randomness: probabilistic algorithms
%   for constructing approximate matrix decompositions,
%   arXiv:0909.4061 [math.NA; math.PR], 2009
%   (available at http://arxiv.org).
%
%
%   See also DIFFSNORM, DIFFSNORMC, EIGENS, EIGENN, PCACOV, PRINCOMP,
%   SVDS.
%

%   Copyright 2014 Mark Tygert.



%
% Check the number of inputs.
%
if(nargin < 1)
  error('MATLAB:pca:TooFewIn',...
        'There must be at least 1 input.')
end

if(nargin > 5)
  error('MATLAB:pca:TooManyIn',...
        'There must be at most 5 inputs.')
end

%
% Check the number of outputs.
%
if(nargout ~= 3)
  error('MATLAB:pca:WrongNumOut',...
        'There must be exactly 3 outputs.')
end

%
% Set the inputs k, raw, its, and l to default values, if necessary.
%
if(nargin == 1)
  k = 6;
  raw = false;
  its = 2;
  l = k+2;
end

if(nargin == 2)
  raw = false;
  its = 2;
  l = k+2;
end

if(nargin == 3)
  its = 2;
  l = k+2;
end

if(nargin == 4)
  l = k+2;
end

%
% Check the first input argument.
%
if(~isfloat(A))
  error('MATLAB:pca:In1NotFloat',...
        'Input 1 must be a floating-point matrix.')
end

if(isempty(A))
  error('MATLAB:pca:In1Empty',...
        'Input 1 must not be empty.')
end

%
% Retrieve the dimensions of A.
%
[m n] = size(A);

%
% Check the remaining input arguments.
%
if(size(k,1) ~= 1 || size(k,2) ~= 1)
  error('MATLAB:pca:In2Not1x1',...
        'Input 2 must be a scalar.')
end

if(size(raw,1) ~= 1 || size(raw,2) ~= 1)
  error('MATLAB:pca:In3Not1x1',...
        'Input 3 must be a scalar.')
end

if(size(its,1) ~= 1 || size(its,2) ~= 1)
  error('MATLAB:pca:In4Not1x1',...
        'Input 4 must be a scalar.')
end

if(size(l,1) ~= 1 || size(l,2) ~= 1)
  error('MATLAB:pca:In5Not1x1',...
        'Input 5 must be a scalar.')
end

if(~(k > 0))
  error('MATLAB:pca:In2NonPos',...
        'Input 2 must be > 0.')
end

if((nargin > 1) && ((k > m) || (k > n)))
  error('MATLAB:pca:In2TooBig',...
        'Input 2 must be <= the smaller dimension of Input 1.')
end

if((nargin == 1) && ((k > m) || (k > n)))
  error('MATLAB:pca:InTooTiny',...
        'The smaller dimension of the input must be >= 6.')
end

if(~(its >= 0))
  error('MATLAB:pca:In4Bad',...
        'Input 4 must be >= 0.')
end

if(l < k)
  error('MATLAB:pca:In5ltIn2',...
        'Input 5 must be >= Input 2.')
end



if(raw)


%
% SVD A directly if l >= m/1.25 or l >= n/1.25.
%
  if((l >= m/1.25) || (l >= n/1.25))

    if(~issparse(A))
      [U,S,V] = svd(A,'econ');
    end

    if(issparse(A))
      [U,S,V] = svd(full(A),'econ');
    end
%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V, and the
%   uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

    return

  end


  if(m >= n)

%
%   Apply A to a random matrix, obtaining Q.
%
    if(isreal(A))
      Q = A*(2*rand(n,l)-ones(n,l));
    end

    if(~isreal(A))
      Q = A*( (2*rand(n,l)-ones(n,l)) + i*(2*rand(n,l)-ones(n,l)) );
    end

%
%   Form a matrix Q whose columns constitute a well-conditioned basis
%   for the columns of the earlier Q.
%
    if(its == 0)
      [Q,R,E] = qr(Q,0);
    end

    if(its > 0)
      [Q,R] = lu(Q);
    end

%
%   Conduct normalized power iterations.
%
    for it = 1:its

      Q = (Q'*A)';

      [Q,R] = lu(Q);

      Q = A*Q;

      if(it < its)
        [Q,R] = lu(Q);
      end

      if(it == its)
        [Q,R,E] = qr(Q,0);
      end

    end

    clear E;

%
%   SVD Q'*A to obtain approximations to the singular values
%   and right singular vectors of A; adjust the left singular vectors
%   of Q'*A to approximate the left singular vectors of A.
%
    [R,S,V] = svd(Q'*A,'econ');
    U = Q*R;

    clear Q R;

%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V, and the
%   uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

  end


  if(m < n)

%
%   Apply A' to a random matrix, obtaining Q.
%
    if(isreal(A))
      Q = ((2*rand(l,m)-ones(l,m))*A)';
    end

    if(~isreal(A))
      Q = (( (2*rand(l,m)-ones(l,m)) + i*(2*rand(l,m)-ones(l,m)) )*A)';
    end

%
%   Form a matrix Q whose columns constitute a well-conditioned basis
%   for the columns of the earlier Q.
%
    if(its == 0)
      [Q,R,E] = qr(Q,0);
    end

    if(its > 0)
      [Q,R] = lu(Q);
    end

%
%   Conduct normalized power iterations.
%
    for it = 1:its

      Q = A*Q;

      [Q,R] = lu(Q);

      Q = (Q'*A)';

      if(it < its)
        [Q,R] = lu(Q);
      end

      if(it == its)
        [Q,R,E] = qr(Q,0);
      end

    end

    clear E;

%
%   SVD A*Q to obtain approximations to the singular values
%   and left singular vectors of A; adjust the right singular vectors
%   of A*Q to approximate the right singular vectors of A.
%
    [U,S,R] = svd(A*Q,'econ');
    V = Q*R;

    clear Q R;

%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V, and the
%   uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

  end


end



if(~raw)


%
% Calculate the average of the entries in every column.
%
  c = sum(A)/m;


%
% SVD the centered A directly if l >= m/1.25 or l >= n/1.25.
%
  if((l >= m/1.25) || (l >= n/1.25))

    if(~issparse(A))
      [U,S,V] = svd(A-ones(m,1)*c,'econ');
    end

    if(issparse(A))
      [U,S,V] = svd(full(A)-ones(m,1)*c,'econ');
    end
%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V, and the
%   uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

    return

  end


  if(m >= n)

%
%   Apply the centered A to a random matrix, obtaining Q.
%
    if(isreal(A))
      R = 2*rand(n,l)-ones(n,l);
    end

    if(~isreal(A))
      R = 2*rand(n,l)-ones(n,l) + i*(2*rand(n,l)-ones(n,l));
    end

    Q = A*R - ones(m,1)*(c*R);

%
%   Form a matrix Q whose columns constitute a well-conditioned basis
%   for the columns of the earlier Q.
%
    if(its == 0)
      [Q,R,E] = qr(Q,0);
    end

    if(its > 0)
      [Q,R] = lu(Q);
    end

%
%   Conduct normalized power iterations.
%
    for it = 1:its

      R = Q';
      Q = (R*A - (R*ones(m,1))*c)';

      [Q,R] = lu(Q);

      Q = A*Q - ones(m,1)*(c*Q);

      if(it < its)
        [Q,R] = lu(Q);
      end

      if(it == its)
        [Q,R,E] = qr(Q,0);
      end

    end

    clear E;

%
%   SVD Q' applied to the centered A to obtain approximations
%   to the singular values and right singular vectors of the centered A;
%   adjust the left singular vectors to approximate
%   the left singular vectors of the centered A.
%
    R = Q';
    [R,S,V] = svd(R*A - (R*ones(m,1))*c,'econ');
    U = Q*R;

    clear Q R;

%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V, and the
%   uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

  end


  if(m < n)

%
%   Apply the adjoint of the centered A to a random matrix, obtaining Q.
%
    if(isreal(A))
      R = 2*rand(l,m)-ones(l,m);
    end

    if(~isreal(A))
      R = 2*rand(l,m)-ones(l,m) + i*(2*rand(l,m)-ones(l,m));
    end

    Q = (R*A - (R*ones(m,1))*c)';

%
%   Form a matrix Q whose columns constitute a well-conditioned basis
%   for the columns of the earlier Q.
%
    if(its == 0)
      [Q,R,E] = qr(Q,0);
    end

    if(its > 0)
      [Q,R] = lu(Q);
    end

%
%   Conduct normalized power iterations.
%
    for it = 1:its

      Q = A*Q - ones(m,1)*(c*Q);

      [Q,R] = lu(Q);

      R = Q';
      Q = (R*A - (R*ones(m,1))*c)';

      if(it < its)
        [Q,R] = lu(Q);
      end

      if(it == its)
        [Q,R,E] = qr(Q,0);
      end

    end

    clear E;

%
%   SVD the centered A applied to Q to obtain approximations
%   to the singular values and left singular vectors of the centered A;
%   adjust the right singular vectors to approximate
%   the right singular vectors of the centered A.
%
    [U,S,R] = svd(A*Q - ones(m,1)*(c*Q),'econ');
    V = Q*R;

    clear Q R;

%
%   Retain only the leftmost k columns of U,
%   the leftmost k columns of V,
%   and the uppermost leftmost k x k block of S.
%
    U = U(:,1:k);
    V = V(:,1:k);
    S = S(1:k,1:k);

  end


end
