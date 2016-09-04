#' tmParallel functions for performance testing
#'
#' Some functions usefull for performance testing
#'

#' @rdname tmParallelPerformanceTesting
#' @export
DocumentTermMatrixOriginal <- function(texts, control = list()){
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_original
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
#' @export
TermDocumentMatrixOriginal <- function(texts, control = list()) {
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_original
    , matrixKind = 'TermDocumentMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
#' @export
DocumentTermMatrixParallelSlower <- function(texts, control = list()) {
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_parallel_Lists
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
#' @export
TermDocumentMatrixParallelSlower <- function(texts, control = list()) {
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_parallel_Lists
    , matrixKind = 'TermDocumentMatrix'
  )
}
