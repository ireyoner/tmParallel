#' weightTfIdfParallel
#'
#' Parallel iplementatnion of Tf-Idf weighting function
#'
#' @param m either \code{\link{TextDocumentMatrix}}
#'        or a \code{\link{DocumentTextMatrix}} object that extens
#'        \code{\link{slam}} \code{\link[slam]{simple_triplet_matrix}}.
#'
#' @param normalize logical flag, if \code{TRUE} then Tf-Idf is normalized
#' - words count vector is divided by words count in document in which this word is from.
#' Default \code{TRUE}.
#'
#' @param plus1inIdf logical flag, if \code{TRUE} then when counting Idf as
#' \code{log2( documents count / documents with given word )} the costant \code{1} value
#' is added.
#' Default \code{FALSE}.
#'
#' @return The given \code{\link{DocumentTermMatrix}} or \code{\link{TermDocumentMatrix}}
#' with updated terms frequency values.
#'
#' @seealso \code{\link{tm}} \code{\link[tm]{weighting}}
#'  for other weighting functions.
#'
#' @examples
#' pdtm <-
#'  ParallelDocumentTermMatrix(c('This is a test', 'And another test') )
#' w1pdtm <-
#'  weightTfIdfParallel(pdtm, normalize = TRUE, plus1inIdf = FALSE)
#' w2pdtm <-
#'  weightTfIdfParallel(pdtm, normalize = FALSE, plus1inIdf = TRUE)
#'
#' pdtm <-
#'  ParallelDocumentTermMatrix(
#'   c('This is a test test', 'And another test'),
#'   control = list( weighting = weightTfIdfParallel )
#'  )
#'
#' pdtm <-
#'  ParallelDocumentTermMatrix(
#'   c('This is a test test', 'And another test'),
#'   control = list(
#'    weighting = function(x) weightTfIdfParallel(x, normalize = TRUE, plus1inIdf = FALSE)
#'   )
#'  )

#' @export
weightTfIdfParallel <-
  tm::WeightFunction(function(m, normalize = TRUE, plus1inIdf = FALSE) {

    if (inherits(m, "DocumentTermMatrix")){
      m$v <- tmParallel::TfIdfParallel(m$j,m$i,m$v,m$ncol,m$nrow,normalize,plus1inIdf)
    }else if(inherits(m, "TermDocumentMatrix")){
      m$v <- tmParallel::TfIdfParallel(m$i,m$j,m$v,m$nrow,m$ncol,normalize,plus1inIdf)
    }else{
      stop("Object must inherit either 'DocumentTermMatrix' or 'TermDocumentMatrix'")
    }
    attr(m, "weighting") <- c(sprintf("%s%s",
                                      "term frequency - inverse document frequency",
                                      if (normalize) " (normalized)" else ""),
                              "tf-idf")
    m
  }, "term frequency - inverse document frequency", "tf-idf")
