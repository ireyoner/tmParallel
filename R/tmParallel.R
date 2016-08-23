#' hunspell Spell Checking and Morphological Analysis
#'
#' @rdname tmParallel
#' @importFrom Rcpp sourceCpp

#' @rdname tmParallel
#' @export
dtm_original <- function(texts
                         , min_term_freq = 0L
                         , max_term_freq = .Machine$integer.max
                         , min_word_length = 0L
                         , max_word_length = .Machine$integer.max
                         , threadsCount = RcppParallel::defaultNumThreads()) {
  stopifnot(is.character(texts))

  RcppParallel::setThreadOptions(numThreads = threadsCount)

  min_term_freq <-
    if (is.numeric(min_term_freq) &&
        min_term_freq >= 0L)
      min_term_freq
  else
    0L

  max_term_freq <-
    if (is.numeric(max_term_freq) &&
        max_term_freq >= 0L)
      min(max_term_freq, .Machine$integer.max)
  else
    .Machine$integer.max

  min_word_length <-
    if (is.numeric(min_word_length) &&
        min_word_length >= 0L)
      min_word_length
  else
    0L

  max_word_length <-
    if (is.numeric(max_word_length) &&
        max_word_length >= 0L)
      min(max_word_length
          , .Machine$integer.max)
  else
    .Machine$integer.max

  Cpp_dtm_original(texts,min_term_freq, max_term_freq, min_word_length, max_word_length)
}

#' @rdname tmParallel
#' @export
dtm_parallel_v1 <- function(texts
                            , min_term_freq = 0L
                            , max_term_freq = .Machine$integer.max
                            , min_word_length = 0L
                            , max_word_length = .Machine$integer.max
                            , threadsCount = RcppParallel::defaultNumThreads()) {
  stopifnot(is.character(texts))

  RcppParallel::setThreadOptions(numThreads = threadsCount)

  min_term_freq <-
    if (is.numeric(min_term_freq) &&
        min_term_freq >= 0L)
      min_term_freq
  else
    0L

  max_term_freq <-
    if (is.numeric(max_term_freq) &&
        max_term_freq >= 0L)
      min(max_term_freq, .Machine$integer.max)
  else
    .Machine$integer.max

  min_word_length <-
    if (is.numeric(min_word_length) &&
        min_word_length >= 0L)
      min_word_length
  else
    0L

  max_word_length <-
    if (is.numeric(max_word_length) &&
        max_word_length >= 0L)
      min(max_word_length
          , .Machine$integer.max)
  else
    .Machine$integer.max

  Cpp_dtm_parallel_v1(texts,min_term_freq, max_term_freq, min_word_length, max_word_length)
}


#' @rdname tmParallel
#' @export
dtm_parallel_v2 <- function(texts
                            , min_term_freq = 0L
                            , max_term_freq = .Machine$integer.max
                            , min_word_length = 0L
                            , max_word_length = .Machine$integer.max
                            , threadsCount = RcppParallel::defaultNumThreads()) {
  stopifnot(is.character(texts))

  RcppParallel::setThreadOptions(numThreads = threadsCount)

  min_term_freq <-
    if (is.numeric(min_term_freq) &&
        min_term_freq >= 0L)
      min_term_freq
  else
    0L

  max_term_freq <-
    if (is.numeric(max_term_freq) &&
        max_term_freq >= 0L)
      min(max_term_freq, .Machine$integer.max)
  else
    .Machine$integer.max

  min_word_length <-
    if (is.numeric(min_word_length) &&
        min_word_length >= 0L)
      min_word_length
  else
    0L

  max_word_length <-
    if (is.numeric(max_word_length) &&
        max_word_length >= 0L)
      min(max_word_length
          , .Machine$integer.max)
  else
    .Machine$integer.max

  Cpp_dtm_parallel_v2(texts,min_term_freq, max_term_freq, min_word_length, max_word_length)
}

