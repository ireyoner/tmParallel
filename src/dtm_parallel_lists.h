#ifndef dtm_parallel_v1
#define dtm_parallel_v1

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

struct WordCountInText {
public:
  int textIndex;
  int wordCount;
  WordCountInText( int p_textIndex, int p_wordCount ) : textIndex(p_textIndex), wordCount(p_wordCount) {}
};

struct WordInTexts {
public:
  std::string word;
  int wordIndex;
  int wordInVectorFirstIndex;
  std::list < WordCountInText > occurrences;

  WordInTexts() : word(""), wordIndex(0), wordInVectorFirstIndex(0) {}
  WordInTexts(
    std::string p_word,
    int p_wordIndex,
    int p_wordInVectorFirstIndex,
    std::list < WordCountInText > p_occurrences
  ) {
    word = p_word;
    wordIndex = p_wordIndex;
    wordInVectorFirstIndex = p_wordInVectorFirstIndex;
    occurrences.splice(occurrences.begin(),p_occurrences,p_occurrences.begin(),p_occurrences.end());
  }

};

struct DtmParallelMapping : public Worker
{
  // source vectors
  const StringVector texts;
  const int min_term_freq;
  const int max_term_freq;
  const unsigned int min_word_length;
  const unsigned int max_word_length;

  // product that I have accumulated
  std::map < std::string, std::list < WordCountInText > > mapping;

  int sparseMatrixVectorSize;

  // constructors
  DtmParallelMapping(const StringVector & p_texts,
                     const int & p_min_term_freq,
                     const int & p_max_term_freq,
                     const unsigned int & p_min_word_length,
                     const unsigned int & p_max_word_length
  ) : texts(p_texts),
  min_term_freq(p_min_term_freq),
  max_term_freq(p_max_term_freq),
  min_word_length(p_min_word_length),
  max_word_length(p_max_word_length),
  sparseMatrixVectorSize(0) {}
  DtmParallelMapping(DtmParallelMapping & dpm,
                     Split
  ) : texts(dpm.texts),
  min_term_freq(dpm.min_term_freq),
  max_term_freq(dpm.max_term_freq),
  min_word_length(dpm.min_word_length),
  max_word_length(dpm.max_word_length),
  sparseMatrixVectorSize(0) {}
  // process just the elements of the range I've been asked to
  void operator()(std::size_t begin, std::size_t end) {
    // mapa liczebności poszczególnych słów w danym dokumnecie
    std::map < std::string, int> wordsInText;

    for (unsigned int textIndex = begin; textIndex < end; textIndex++) {
      // parsowanie tekstu do poszczególnych słów
      boost::tokenizer<> textWords(texts(textIndex));

      // dla każdego słowa w tekście dodanie jego wystąpienia do mapy
      for (boost::tokenizer<>::const_iterator word = textWords.begin();
           word != textWords.end();
           ++word
      ) {
        wordsInText[*word]++;
      }

      // dla każdego słowa w mapie dodanie go do globalnej mapy słów
      for (std::map<std::string, int>::iterator wordAndCountIterator = wordsInText.begin();
           wordAndCountIterator != wordsInText.end();
           ++wordAndCountIterator
      ) {
        std::string term = wordAndCountIterator->first;
        int freq = wordAndCountIterator->second;

        if(min_term_freq <= freq &&
           freq <= max_term_freq &&
           min_word_length <= term.length() &&
           term.length() <= max_word_length ){
          mapping[term].push_back(WordCountInText(textIndex,freq));
        }
      }
#if defined(_MEMORY_DEBUG_)
      checkMaxCurrentRSS();
#endif
      // czyszczenie mapy dla przetwarzanego tekstu
      wordsInText.clear();
    }
  }

  // join my value with that of another InnerProduct
  void join(const DtmParallelMapping& dpm) {
    for (std::map < std::string, std::list < WordCountInText > >::const_iterator otherMapIterator = dpm.mapping.begin();
         otherMapIterator != dpm.mapping.end();
         ++otherMapIterator
    ) {
      std::string stem = otherMapIterator->first;

      if (mapping.count(stem)){
        //mapping[otherMapIterator->first].occurrences.splice(mapping[otherMapIterator->first].occurrences.end(),otherMapIterator->second.occurrences);
        mapping[otherMapIterator->first]
        .insert(
        mapping[stem].end(),
        otherMapIterator->second.begin(),
        otherMapIterator->second.end()
        );
      }else{
        mapping[stem] = otherMapIterator->second;
      }
#if defined(_MEMORY_DEBUG_)
      checkMaxCurrentRSS();
#endif
    }

  }

  std::vector<WordInTexts> recalcMapPointers(){
    int wordIndex = 0;
    int wordInVectorFirstIndex = 0;
    std::vector<WordInTexts> mappingVector;
    mappingVector.reserve(mapping.size());

    for (std::map < std::string, std::list < WordCountInText > >::iterator mapIterator = mapping.begin();
         mapIterator != mapping.end();
         ++mapIterator
    ) {
      int listSize = mapIterator->second.size();

      mappingVector.push_back(WordInTexts( mapIterator->first, wordIndex++, wordInVectorFirstIndex, mapIterator->second));
      wordInVectorFirstIndex += listSize;
    }
#if defined(_MEMORY_DEBUG_)
    checkMaxCurrentRSS();
#endif
    sparseMatrixVectorSize = wordInVectorFirstIndex;
    return mappingVector;
  }
};

struct DtmParallelVectorsFilling : public Worker
{
  // source data
  std::vector<WordInTexts> mappingVector;

  // destination vectors
  std::vector<int> textIndexVector;
  std::vector<int> termIndexVector;
  std::vector<int> termCountVector;
  std::vector<std::string> termsVector;

  // initialize with source and destination
  DtmParallelVectorsFilling
    ( std::vector<WordInTexts> &mappingVector
        , int sparseMatrixVectorSize
        , int termsVectorSize
    ) : mappingVector(mappingVector)
    , textIndexVector(sparseMatrixVectorSize)
    , termIndexVector(sparseMatrixVectorSize)
    , termCountVector(sparseMatrixVectorSize)
    , termsVector(termsVectorSize) {}

  // take the square root of the range of elements requested
  void operator()(std::size_t begin, std::size_t end) {
    for (std::size_t mapIterator = begin;
         mapIterator < end;
         ++mapIterator
    ){

      termsVector[mappingVector[mapIterator].wordIndex] = mappingVector[mapIterator].word;
      int indexOffset = 0;

      for(std::list < WordCountInText >::iterator listIterator = mappingVector[mapIterator].occurrences.begin();
          listIterator != mappingVector[mapIterator].occurrences.end();
          ++listIterator
      ){
        textIndexVector[mappingVector[mapIterator].wordInVectorFirstIndex+indexOffset] = listIterator->textIndex + 1;
        termIndexVector[mappingVector[mapIterator].wordInVectorFirstIndex+indexOffset] = mappingVector[mapIterator].wordIndex + 1;
        termCountVector[mappingVector[mapIterator].wordInVectorFirstIndex+indexOffset] = listIterator->wordCount;
        indexOffset++;
      }
#if defined(_MEMORY_DEBUG_)
      checkMaxCurrentRSS();
#endif
      mappingVector[mapIterator].occurrences.clear();
    }
  }
};

#if defined(_MEMORY_DEBUG_)
// [[Rcpp::export]]
List Cpp_dtm_parallel_Lists_mem_test(
    const StringVector & strings_,
    const int & min_term_freq,
    const int & max_term_freq,
    const unsigned int & min_word_length,
    const unsigned int & max_word_length) {
#else
// [[Rcpp::export]]
List Cpp_dtm_parallel_Lists(
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
  DtmParallelMapping dtmParallelMapping(strings_,min_term_freq,max_term_freq,min_word_length,max_word_length);

#if defined(_MEMORY_DEBUG_)
  double preParallelRSS = getCurrentRSS();
#endif

  // call paralleReduce to start the work
  parallelReduce(0, strings_.length(), dtmParallelMapping);

#if defined(_MEMORY_DEBUG_)
  double postParallelRSS = getCurrentRSS(),
    inParallelRSS = getMaxCurrentRSS();
#endif

  std::vector<WordInTexts> mappingVector = dtmParallelMapping.recalcMapPointers();

#if defined(_MEMORY_DEBUG_)
  double postMappingVectorRSS = getCurrentRSS(),
    inMappingVectorRSS = getMaxCurrentRSS();
#endif


  // create the worker
  DtmParallelVectorsFilling
    dtmParallelVectorsFilling(mappingVector, dtmParallelMapping.sparseMatrixVectorSize, mappingVector.size());

#if defined(_MEMORY_DEBUG_)
  double preParallelForRSS = getCurrentRSS();
#endif

  // call it with parallelFor
  parallelFor(0, mappingVector.size(), dtmParallelVectorsFilling);

#if defined(_MEMORY_DEBUG_)
  double postParallelForRSS = getCurrentRSS(),
    inParallelForRSS = getMaxCurrentRSS();

  // return the computed product
  List ret =
    List::create(
      Named("vectors") = List::create(
        Named("i") = dtmParallelVectorsFilling.textIndexVector, // i
        Named("j") = dtmParallelVectorsFilling.termIndexVector, // j
        Named("v") = dtmParallelVectorsFilling.termCountVector, // v
        Named("terms") = dtmParallelVectorsFilling.termsVector
      ),
      Named("RSS") = List::create(
        Named("initialRSS") = funcInitRSS,
        Named("createRSS") = getMaxCurrentRSS(),
        Named("preMapReduceRSS") = preParallelRSS,
        Named("inMapReduceRSS") = inParallelRSS,
        Named("postMapReduceRSS") = postParallelRSS,
        Named("inMappingVectorRSS") = inMappingVectorRSS,
        Named("postMappingVectorRSS") = postMappingVectorRSS,
        Named("preParallelForRSS") = preParallelForRSS,
        Named("inParallelForRSS") = inParallelForRSS,
        Named("postParallelForRSS") = postParallelForRSS,
        Named("maxCurrentRSS") = getMaxCurrentRSS(),
        Named("peakRSS") = getPeakRSS()
      )
    );

  ((List)ret[1])[10] = getMaxCurrentRSS();

  return ret;
#else
  // return the computed product
  return List::create(Named("i") = dtmParallelVectorsFilling.textIndexVector, // i
                      Named("j") = dtmParallelVectorsFilling.termIndexVector, // j
                      Named("v") = dtmParallelVectorsFilling.termCountVector, // v
                      Named("terms") = dtmParallelVectorsFilling.termsVector); // terms

#endif
}

#endif
