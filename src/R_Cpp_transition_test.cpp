
#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::export]]
List R_Cpp_transition_test( const StringVector terms,
                       const int out_vectors_length) {

  std::vector<int> i,j,v;
  i.resize(out_vectors_length,0);
  j.resize(out_vectors_length,1);
  v.resize(out_vectors_length,2);

  List test = List::create(Named("i") = i,
                      Named("j") = j,
                      Named("v") = v,
                      Named("terms") = terms);
  return test;
}
