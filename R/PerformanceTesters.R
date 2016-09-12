#' tmParallel functions for performance testing
#'
#' Some functions usefull for performance testing - with diffrent implementations
#' for creating DocumentTermMatrix or TermDocumentMatrix
#'
#' For parametrs lists see \likn{ParallelDocumentTermMatrix}
#'

#' @rdname tmParallelPerformanceTesting
DocumentTermMatrixOriginal <- function(texts, control = list(), docsIDs = as.character(1:length(texts))){
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_original
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
TermDocumentMatrixOriginal <- function(texts, control = list(), docsIDs = as.character(1:length(texts))) {
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_original
    , matrixKind = 'TermDocumentMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
DocumentTermMatrixParallelSlower <- function(texts, control = list(), docsIDs = as.character(1:length(texts))) {
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_parallel_Lists
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname tmParallelPerformanceTesting
TermDocumentMatrixParallelSlower <- function(texts, control = list(), docsIDs = as.character(1:length(texts))) {
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_parallel_Lists
    , matrixKind = 'TermDocumentMatrix'
  )
}
