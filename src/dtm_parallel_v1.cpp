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
      mappingVector[mapIterator].occurrences.clear();
    }
  }
};

// [[Rcpp::export]]
List Cpp_dtm_parallel_v1(const StringVector strings,
                         const int min_term_freq,
                         const int max_term_freq,
                         const unsigned int min_word_length,
                         const unsigned int max_word_length) {

  // declare the InnerProduct instance that takes a pointer to the vector data
  DtmParallelMapping dtmParallelMapping(strings,min_term_freq,max_term_freq,min_word_length,max_word_length);

  // call paralleReduce to start the work
  parallelReduce(0, strings.length(), dtmParallelMapping);

  std::vector<WordInTexts> mappingVector = dtmParallelMapping.recalcMapPointers();

  // create the worker
  DtmParallelVectorsFilling
    dtmParallelVectorsFilling(mappingVector, dtmParallelMapping.sparseMatrixVectorSize, mappingVector.size());

  // call it with parallelFor
  parallelFor(0, mappingVector.size(), dtmParallelVectorsFilling);

  // return the computed product
  return List::create(Named("i") = dtmParallelVectorsFilling.textIndexVector, // i
                      Named("j") = dtmParallelVectorsFilling.termIndexVector, // j
                      Named("v") = dtmParallelVectorsFilling.termCountVector, // v
                      Named("terms") = dtmParallelVectorsFilling.termsVector); // terms
}

