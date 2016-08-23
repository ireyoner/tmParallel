// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]

#include <Rcpp.h>
#include <RcppParallel.h>
#include <boost/tokenizer.hpp>
#include <map>

using namespace Rcpp;
using namespace RcppParallel;

struct DtmParallel : public Worker
{
  // source vectors
  const StringVector texts;
  const int min_term_freq;
  const int max_term_freq;
  const unsigned int min_word_length;
  const unsigned int max_word_length;

  // product that I have accumulated
  std::map<std::string, int> terms_pos;
  std::vector<int> textIndexVector, termIndexVector, termCountVector;
  std::vector<std::string> termsVector;

  // constructors
  DtmParallel( const StringVector & p_texts,
               const int & p_min_term_freq,
               const int & p_max_term_freq,
               const unsigned int & p_min_word_length,
               const unsigned int & p_max_word_length
  ) : texts(p_texts),
  min_term_freq(p_min_term_freq),
  max_term_freq(p_max_term_freq),
  min_word_length(p_min_word_length),
  max_word_length(p_max_word_length) {}
  DtmParallel(DtmParallel & dp,
              Split
  ) : texts(dp.texts),
  min_term_freq(dp.min_term_freq),
  max_term_freq(dp.max_term_freq),
  min_word_length(dp.min_word_length),
  max_word_length(dp.max_word_length) {}

  // process just the elements of the range I've been asked to
  void operator()(std::size_t begin, std::size_t end) {
    int termPos = 1;
    std::map<std::string, int> text_terms;

    for (unsigned int index = begin; index < end; index++) {
      boost::tokenizer<> tok(texts(index));

      for (boost::tokenizer<>::iterator it = tok.begin();
           it != tok.end();
           ++it
      ) {
        std::string token = *it;
        if (min_word_length <= token.length() &&
            token.length() <= max_word_length)
          text_terms[token]++;
      }

      for (std::map<std::string, int>::iterator it = text_terms.begin();
           it != text_terms.end();
           ++it) {
        std::string term = it->first;
        int freq = it->second;

        if (min_term_freq <= freq && freq <= max_term_freq) {
          if (!terms_pos.count(term)) {
            terms_pos[term] = termPos++;
            termsVector.push_back(term);
          }
          textIndexVector.push_back(terms_pos[term]);
          termIndexVector.push_back(index + 1);
          termCountVector.push_back(freq);
        }
      }

      text_terms.clear();
    }
  }

  // join my value with that of another InnerProduct
  void join(const DtmParallel& ut) {
    int termPos = terms_pos.size() + 1;
    std::map<int,int> utTermsMapping;

    for (std::map<std::string,int>::const_iterator otherTermsPosIterator = ut.terms_pos.begin();
         otherTermsPosIterator != ut.terms_pos.end();
         ++otherTermsPosIterator
    ) {

      std::string term = otherTermsPosIterator->first;
      int oldIndex = otherTermsPosIterator->second;

      if (!terms_pos.count(term)) {
        utTermsMapping[oldIndex] = termPos;
        terms_pos[term] = termPos++;
        termsVector.push_back(term);
      }else{
        utTermsMapping[oldIndex] = terms_pos[term];
      }
    }
    std::vector<std::string> (termsVector).swap(termsVector);
    //     for (std::map<std::string,int>::const_iterator otherTermsPosIterator = ut.terms_pos.begin();
    //          otherTermsPosIterator != ut.terms_pos.end();
    //          ++otherTermsPosIterator
    //     ) {
    //       std::string term = otherTermsPosIterator->first;
    //       int oldIndex = otherTermsPosIterator->second;
    //
    //       Rcpp::Rcout << "Mapping: " << term
    //                   << ", oldIndex: " << oldIndex
    //                   << ", newIndex: " << utTermsMapping[oldIndex] << std::endl;
    //     }


    textIndexVector.reserve( textIndexVector.size() + ut.textIndexVector.size() );
    for(std::vector<int>::const_iterator termsPosIter = ut.textIndexVector.begin();
        termsPosIter != ut.textIndexVector.end();
        ++termsPosIter
    ) {
      int termIndex = *termsPosIter;

      textIndexVector.push_back( utTermsMapping[termIndex] );

      //       Rcpp::Rcout << "oldVal: " << termIndex
      //                   << ", newVal: " << utTermsMapping[termIndex]
      //                   << ", term: " << ut.termsVector[termIndex-1]
      //                   << std::endl;

    }
    utTermsMapping.clear();

    termIndexVector.reserve( termIndexVector.size() + ut.termIndexVector.size() );
    termIndexVector.insert( termIndexVector.end(), ut.termIndexVector.begin(), ut.termIndexVector.end() );

    termCountVector.reserve( termCountVector.size() + ut.termCountVector.size() );
    termCountVector.insert( termCountVector.end(), ut.termCountVector.begin(), ut.termCountVector.end() );

  }
};

// [[Rcpp::export]]
List Cpp_dtm_parallel_v2(const StringVector strings,
                         const int min_term_freq,
                         const int max_term_freq,
                         const unsigned int min_word_length,
                         const unsigned int max_word_length) {

  // declare the InnerProduct instance that takes a pointer to the vector data
  DtmParallel dtmParallel(strings,min_term_freq,max_term_freq,min_word_length,max_word_length);

  // call paralleReduce to start the work
  parallelReduce(0, strings.length(), dtmParallel);

  // return the computed product
  return List::create(Named("i") = dtmParallel.textIndexVector,
                      Named("j") = dtmParallel.termIndexVector,
                      Named("v") = dtmParallel.termCountVector,
                      Named("terms") = dtmParallel.termsVector);
}
