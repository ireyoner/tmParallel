// [[Rcpp::depends(BH)]]
// [[Rcpp::depends(RcppParallel)]]
#include <Rcpp.h>
#include <RcppParallel.h>
#include <math.h>

using namespace Rcpp;
using namespace RcppParallel;

struct TfIdfParallelWorker : public Worker
{
  enum phases { countSumIdf, countSumTf, normalizeTf, recalcIdf0, recalcIdf1, lnrs };

  phases currentPhase;

  // not editable vectors
  const double documentCount;
  const double wordsCount;
  const std::vector<double> word;
  const std::vector<double> text;

  // vector to modify
  std::vector<double> count;

  // helpers vectors:
  std::vector<double> documentsWithWord;
  std::vector<double> wordsInDocument;

  // constructors
  TfIdfParallelWorker(
    const double documentCount_,
    const double wordsCount_,
    const std::vector<double> & word_,
    const std::vector<double> & text_,
    const std::vector<double> & count_
  ) : documentCount(documentCount_),
  wordsCount(wordsCount_),
  word(word_),
  text(text_),
  count(count_),
  documentsWithWord(wordsCount_),
  wordsInDocument(documentCount_) {}

  TfIdfParallelWorker(TfIdfParallelWorker & TfIdfPw,
                      Split
  ) : documentCount(TfIdfPw.documentCount),
  wordsCount(TfIdfPw.wordsCount),
  word(TfIdfPw.word),
  text(TfIdfPw.text),
  count(TfIdfPw.count),
  documentsWithWord(TfIdfPw.wordsCount),
  wordsInDocument(TfIdfPw.documentCount) {}

  // process just the elements of the range I've been asked to
  void operator()(std::size_t begin, std::size_t end) {
    if(currentPhase == countSumIdf) {
      for (unsigned int index = begin; index < end; index++)
      {
        documentsWithWord[word[index]]++;
      }
    } else if (currentPhase == countSumTf) {
      for (unsigned int index = begin; index < end; index++)
      {
        wordsInDocument[text[index]] += count[index];
      }
    } else if (currentPhase == normalizeTf) {
      for (unsigned int index = begin; index < end; index++)
      {
        if (wordsInDocument[text[index]] != 0) {
          count[index] /= wordsInDocument[text[index]];
        } else {
          count[index] = 0;
        }
      }
    } else if (currentPhase == recalcIdf0) {
      for (unsigned int index = begin; index < end; index++)
      {
        if (documentsWithWord[index] != 0) {
          documentsWithWord[index] = log2( documentCount / documentsWithWord[index] );
        } else {
          documentsWithWord[index] = 0;
        }
      }
    } else if (currentPhase == recalcIdf1) {
      for (unsigned int index = begin; index < end; index++)
      {
        if (documentsWithWord[index] != 0) {
          documentsWithWord[index] = 1.0 + log2( documentCount / documentsWithWord[index] );
        } else {
          documentsWithWord[index] = 1.0;
        }
      }
    } else if (currentPhase == lnrs) {
      for (unsigned int index = begin; index < end; index++)
      {
        count[index] *= documentsWithWord[word[index]];
      }
    }
  }

  // join my value with that of another InnerProduct
  void join(const TfIdfParallelWorker& TfIdfPw) {
    if(currentPhase == countSumIdf) {
      for (unsigned int index = 0; index < documentsWithWord.size(); index++)
      {
        documentsWithWord[index] += TfIdfPw.documentsWithWord[index];
      }
    } else if (currentPhase == countSumTf) {
      for (unsigned int index = 0; index < wordsInDocument.size(); index++)
      {
        wordsInDocument[index] += TfIdfPw.wordsInDocument[index];
      }
    }
  }
};

// [[Rcpp::export]]
std::vector<double> TfIdfParallel(
    const std::vector<double> & word_,
    const std::vector<double> & text_,
    const std::vector<double> & count_,
    const unsigned int termsCount_,
    const unsigned int documentCount_,
    bool normalizeTf,
    bool IdfPlus1
){

  TfIdfParallelWorker
  tfIdfParallelWorker(
    documentCount_,
    termsCount_,
    word_,
    text_,
    count_
  );

  tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.countSumIdf;
  parallelReduce(0, count_.size(), tfIdfParallelWorker);

  if (normalizeTf) {
    tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.countSumTf;
    parallelReduce(0, count_.size(), tfIdfParallelWorker);

    tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.normalizeTf;
    parallelFor(0, count_.size(), tfIdfParallelWorker);
  }

  if (IdfPlus1) {
    tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.recalcIdf1;
  }else{
    tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.recalcIdf0;
  }
  parallelFor(0, termsCount_, tfIdfParallelWorker);

  tfIdfParallelWorker.currentPhase = tfIdfParallelWorker.lnrs;
  parallelFor(0, count_.size(), tfIdfParallelWorker);

  return tfIdfParallelWorker.count;
}
