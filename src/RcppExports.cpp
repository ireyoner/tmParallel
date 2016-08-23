// This file was generated by Rcpp::compileAttributes
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// Cpp_dtm_original
List Cpp_dtm_original(const StringVector strings, const int min_term_freq, const int max_term_freq, const unsigned int min_word_length, const unsigned int max_word_length);
RcppExport SEXP tmParallel_Cpp_dtm_original(SEXP stringsSEXP, SEXP min_term_freqSEXP, SEXP max_term_freqSEXP, SEXP min_word_lengthSEXP, SEXP max_word_lengthSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< const StringVector >::type strings(stringsSEXP);
    Rcpp::traits::input_parameter< const int >::type min_term_freq(min_term_freqSEXP);
    Rcpp::traits::input_parameter< const int >::type max_term_freq(max_term_freqSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type min_word_length(min_word_lengthSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type max_word_length(max_word_lengthSEXP);
    __result = Rcpp::wrap(Cpp_dtm_original(strings, min_term_freq, max_term_freq, min_word_length, max_word_length));
    return __result;
END_RCPP
}
// Cpp_dtm_parallel_v1
List Cpp_dtm_parallel_v1(const StringVector strings, const int min_term_freq, const int max_term_freq, const unsigned int min_word_length, const unsigned int max_word_length);
RcppExport SEXP tmParallel_Cpp_dtm_parallel_v1(SEXP stringsSEXP, SEXP min_term_freqSEXP, SEXP max_term_freqSEXP, SEXP min_word_lengthSEXP, SEXP max_word_lengthSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< const StringVector >::type strings(stringsSEXP);
    Rcpp::traits::input_parameter< const int >::type min_term_freq(min_term_freqSEXP);
    Rcpp::traits::input_parameter< const int >::type max_term_freq(max_term_freqSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type min_word_length(min_word_lengthSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type max_word_length(max_word_lengthSEXP);
    __result = Rcpp::wrap(Cpp_dtm_parallel_v1(strings, min_term_freq, max_term_freq, min_word_length, max_word_length));
    return __result;
END_RCPP
}
// Cpp_dtm_parallel_v2
List Cpp_dtm_parallel_v2(const StringVector strings, const int min_term_freq, const int max_term_freq, const unsigned int min_word_length, const unsigned int max_word_length);
RcppExport SEXP tmParallel_Cpp_dtm_parallel_v2(SEXP stringsSEXP, SEXP min_term_freqSEXP, SEXP max_term_freqSEXP, SEXP min_word_lengthSEXP, SEXP max_word_lengthSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< const StringVector >::type strings(stringsSEXP);
    Rcpp::traits::input_parameter< const int >::type min_term_freq(min_term_freqSEXP);
    Rcpp::traits::input_parameter< const int >::type max_term_freq(max_term_freqSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type min_word_length(min_word_lengthSEXP);
    Rcpp::traits::input_parameter< const unsigned int >::type max_word_length(max_word_lengthSEXP);
    __result = Rcpp::wrap(Cpp_dtm_parallel_v2(strings, min_term_freq, max_term_freq, min_word_length, max_word_length));
    return __result;
END_RCPP
}
