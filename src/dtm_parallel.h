#ifndef dtm_parallel_v2
#define dtm_parallel_v2

#if defined(_MEMORY_DEBUG_)
#include "memory.h"
#endif

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
#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

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
            if(termsVector.size() + 1 == termsVector.capacity())
              termsVector.reserve(termsVector.capacity()+1024);
            termsVector.push_back(term);
          }
          if(textIndexVector.size()+1 == textIndexVector.capacity()){
            textIndexVector.reserve(textIndexVector.capacity()+1024);
          // }
          // if(termIndexVector.size()+1 == termIndexVector.capacity()){
            termIndexVector.reserve(termIndexVector.capacity()+1024);
          // }
          // if(termCountVector.size()+1 == termCountVector.capacity()){
            termCountVector.reserve(termCountVector.capacity()+1024);
          }
          termIndexVector.push_back(terms_pos[term]);
          textIndexVector.push_back(index + 1);
          termCountVector.push_back(freq);
        }
      }
#if defined(_MEMORY_DEBUG_)
      checkMaxCurrentRSS();
#endif

      text_terms.clear();
    }
  }

  // join my value with that of another InnerProduct
  void join(const DtmParallel& ut) {
    int termPos = terms_pos.size() + 1;
    std::map<int,int> utTermsMapping;

#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

    int distinct_terms = terms_pos.size();
    for (std::map<std::string,int>::const_iterator otherTermsPosIterator = ut.terms_pos.begin();
         otherTermsPosIterator != ut.terms_pos.end();
         ++otherTermsPosIterator
    ) {

      std::string term = otherTermsPosIterator->first;

      if (!terms_pos.count(term)) {
        distinct_terms++;
      }
    }

    termsVector.reserve( distinct_terms );
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
#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

    termIndexVector.reserve( termIndexVector.size() + ut.termIndexVector.size() );
    for(std::vector<int>::const_iterator termsPosIter = ut.termIndexVector.begin();
        termsPosIter != ut.termIndexVector.end();
        ++termsPosIter
    ) {
      int termIndex = *termsPosIter;
      termIndexVector.push_back( utTermsMapping[termIndex] );
      // textIndexVector.push_back( terms_pos[ut.termsVector[termIndex-1]] );
    }
#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

    utTermsMapping.clear();

    textIndexVector.reserve( textIndexVector.size() + ut.textIndexVector.size() );
    textIndexVector.insert( textIndexVector.end(), ut.textIndexVector.begin(), ut.textIndexVector.end() );

    termCountVector.reserve( termCountVector.size() + ut.termCountVector.size() );
    termCountVector.insert( termCountVector.end(), ut.termCountVector.begin(), ut.termCountVector.end() );

#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif

  }
};

#if defined(_MEMORY_DEBUG_)
// [[Rcpp::export]]
List Cpp_dtm_parallel_mem_test(
    const StringVector & strings_,
    const int & min_term_freq,
    const int & max_term_freq,
    const unsigned int & min_word_length,
    const unsigned int & max_word_length) {
#else
  // [[Rcpp::export]]
  List Cpp_dtm_parallel(
      const StringVector & strings_,
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

    // declare the InnerProduct instance that takes a pointer to the vector data
    DtmParallel dtmParallel(strings_,min_term_freq,max_term_freq,min_word_length,max_word_length);

#if defined(_MEMORY_DEBUG_)
    double preParallelRSS = getCurrentRSS();
#endif

    // call paralleReduce to start the work
    parallelReduce(0, strings_.length(), dtmParallel);

#if defined(_MEMORY_DEBUG_)
    double postParallelRSS = getCurrentRSS();

    // return the computed product
    List ret =
      List::create(
        Named("vectors") = List::create(
          Named("i") = dtmParallel.textIndexVector,
          Named("j") = dtmParallel.termIndexVector,
          Named("v") = dtmParallel.termCountVector,
          Named("terms") = dtmParallel.termsVector
        ),
        Named("RSS") = List::create(
          Named("initialRSS") = funcInitRSS,
          Named("createRSS") = getMaxCurrentRSS(),
          Named("preParallelRSS") = preParallelRSS,
          Named("postParallelRSS") = postParallelRSS,
          Named("maxCurrentRSS") = getMaxCurrentRSS(),
          Named("peakRSS") = getPeakRSS()
        )
      );

    ((List)ret[1])[4] = getMaxCurrentRSS();

    return ret;
#else
    return List::create(Named("i") = dtmParallel.textIndexVector,
                        Named("j") = dtmParallel.termIndexVector,
                        Named("v") = dtmParallel.termCountVector,
                        Named("terms") = dtmParallel.termsVector
    );
#endif
  }

#endif
