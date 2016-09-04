#include "memory.h"

#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
List mem_test() {
  clearMaxCurrentRSS();
  return List::create(
    Named("createRSS") = getMaxCurrentRSS(),
    Named("peakRSS") = getPeakRSS()
  );
}
