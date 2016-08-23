// [[Rcpp::depends(BH)]]

#include <Rcpp.h>
#include <boost/tokenizer.hpp>
#include <map>

using namespace Rcpp;

// [[Rcpp::export]]
List Cpp_dtm_original( const StringVector strings,
                       const int min_term_freq,
                       const int max_term_freq,
                       const unsigned int min_word_length,
                       const unsigned int max_word_length) {

  int column = 1;
  std::map<std::string, int> line, terms_pos;
  std::vector<int> i, j, v;
  std::vector<std::string> terms;

  for (int index = 0; index < strings.size(); index++) {
    boost::tokenizer<> tok(strings(index));

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
    }
  }

  return List::create(Named("i") = i,
                      Named("j") = j,
                      Named("v") = v,
                      Named("terms") = terms);
}
