#' tmParallel is a package that implementes faster and more memory wise versions of
#' TermDocumentMatrix and DocumentTermMatrix.
#'
#' Package tmParallel uses RcppParallel library for multithreading operations to speed up the
#' computing.
#'
#' Unlike the \code{\link{tm}} package in this one there is no data preprocessing.
#' It gives user freedom to use diffrent data sources, not creating memory heavy
#' \code{\link[tm]{Corpus}} every time.
#'
#' For detailed informations see \code{\link{ParallelDocumentTermMatrix}}.
#'

NULL
