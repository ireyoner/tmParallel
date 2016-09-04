#include "memory.h"

#include <Rcpp.h>

using namespace Rcpp;

// [[Rcpp::export]]
List R_Cpp_transition_test( const StringVector terms,
                       const int out_vectors_length) {

  clearMaxCurrentRSS();

  double funcInitCurrentRSS = getMaxCurrentRSS();

  checkMaxCurrentRSS();

  std::vector<int> i,j,v;
  i.resize(out_vectors_length,0);
  j.resize(out_vectors_length,1);
  v.resize(out_vectors_length,2);

  checkMaxCurrentRSS();

    List test =
      List::create(
        Named("vectors") = List::create(
          Named("i") = i,
          Named("j") = j,
          Named("v") = v,
          Named("terms") = terms
        ),
        Named("RSS") = List::create(
          Named("initialRSS") = funcInitCurrentRSS,
          Named("createRSS") = getMaxCurrentRSS(),
          Named("maxCurrentRSS") = getMaxCurrentRSS(),
          Named("peakRSS") = getPeakRSS()
        )
      );

  ((List)test[1])[2] = getMaxCurrentRSS();

  return test;
}
