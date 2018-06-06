/**
\brief 07_STL: STL example

This program illustrates some basic STL concepts like vectors and some of its functions
*/
#include <iostream>
#include <vector>

#include <string>
#include <algorithm>

using namespace std;

template<typename TContainerType>
bool findInVector( std::vector<TContainerType> &vector_to_search_in, 
                                   TContainerType element_to_search_for )
{
  typename std::vector<TContainerType>::const_iterator iterator = 
    std::find(vector_to_search_in.begin(), vector_to_search_in.end(), element_to_search_for);
  if( iterator !=  vector_to_search_in.end() )
  {
    return true;
  }
  else
    return false;
}
 
int main()
{
   // create a vector to store int
   vector<int> vec; 

   // display the original size of vec
   cout << "vector size = " << vec.size() << endl;

   // push 5 values into the vector
   for (unsigned int i = 0; i < 5; i++)
   {
     vec.push_back(i);
   }

   // display extended size of vec
   cout << "extended vector size = " << vec.size() << endl;

   // access 5 values from the vec
   for (auto i = 0; i < 5; i++)
   {
     cout << "value of vec [" << i << "] = " << vec[i] << endl;
   }

   // use iterator to access the values
   vector<int>::iterator v = vec.begin();
   while ( v != vec.end()) 
   {
     cout << "value of v = " << *v << endl;
     v++;
   }

   for (auto it = vec.begin(); it != vec.end(); ++it)
   {
     cout << "another way to get value of v = " << *it << endl;
   }

   if (findInVector<int>(vec, 2))
     cout << "found!\n";

   return 0;
}