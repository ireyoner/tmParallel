#ifndef dtm_orginal
#define dtm_orginal

#if defined(_MEMORY_DEBUG_)
#include "memory.h"
#endif

// [[Rcpp::depends(BH)]]
#include <Rcpp.h>
#include <boost/tokenizer.hpp>
#include <map>

using namespace Rcpp;

#if defined(_MEMORY_DEBUG_)
// [[Rcpp::export]]
List Cpp_dtm_original_mem_test(
    const StringVector strings_,
    const int & min_term_freq,
    const int & max_term_freq,
    const unsigned int & min_word_length,
    const unsigned int & max_word_length) {
#else
// [[Rcpp::export]]
List Cpp_dtm_original(
    const StringVector strings_,
    const int & min_term_freq,
    const int & max_term_freq,
    const unsigned int & min_word_length,
    const unsigned int & max_word_length) {
#endif
#if defined(_MEMORY_DEBUG_)
  clearMaxCurrentRSS();

  double funcInitRSS = getMaxCurrentRSS();

  checkMaxCurrentRSS();
#endif

  int column = 1;
  std::map<std::string, int> line, terms_pos;
  std::vector<int> i, j, v;
  std::vector<std::string> terms;

  for (int index = 0; index < strings_.size(); index++) {
    boost::tokenizer<> tok(strings_(index));

#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

    line.clear();
    for (boost::tokenizer<>::iterator it = tok.begin();
         it != tok.end();
         ++it) {
      std::string token = *it;
      if (min_word_length <= token.length() &&
          token.length() <= max_word_length)
        line[token]++;
    }

    for (std::map<std::string, int>::iterator it = line.begin();
         it != line.end();
         ++it) {
      std::string term = it->first;
      int freq = it->second;

      if (min_term_freq <= freq && freq <= max_term_freq) {
        if (!terms_pos.count(term)) {
          terms_pos[term] = column++;
          terms.push_back(term);
        }
        i.push_back(index + 1);
        j.push_back(terms_pos[term]);
        v.push_back(freq);
      }
#if defined(_MEMORY_DEBUG_)
      checkMaxCurrentRSS();
#endif
    }
  }

#if defined(_MEMORY_DEBUG_)
  List ret =
    List::create(
      Named("vectors") = List::create(
        Named("i") = i,
        Named("j") = j,
        Named("v") = v,
        Named("terms") = terms
      ),
      Named("RSS") = List::create(
        Named("initialRSS") = funcInitRSS,
        Named("createRSS") = getMaxCurrentRSS(),
        Named("maxCurrentRSS") = getMaxCurrentRSS(),
        Named("peakRSS") = getPeakRSS()
      )
    );

  ((List)ret[1])[2] = getMaxCurrentRSS();

  return ret;
#else
  return List::create(Named("i") = i,
                      Named("j") = j,
                      Named("v") = v,
                      Named("terms") = terms);
#endif
}
#endif
