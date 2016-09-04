#' ParallelDocumentTermMatrix
#'
#' Constructs a document-term matrix or a term-document matrix
#'
#'
#' @param texts either \code{\link{vector}} of \code{\link{character}}
#'        or a \code{\link[tm]{Corpus}} from \code{\link{tm}} package.
#' @param control a named list of control options. Available options:
#' \describe{
#'   \item{\code{threadsCount}}{A number of threads used when executing functions
#'   (for more see: \code{\link{RcppParallel}} \code{\link[RcppParallel]{setThreadOptions}})
#'   }
#'
#'   \item{\code{bounds}}{A list whose value must be an \code{integer vector} of length 2.
#'   Terms that appear in less documents than the lower bound \code{bounds$global[1]}
#'   or in more documents than the upper bound \code{bounds$global[2]}
#'   are discarded. Defaults to \code{control = list(bounds = list(global = c(1, Inf)))}
#'   (i.e., every term will be used).
#'   }
#'
#'   \item{\code{wordLengths}}{A value must be an \code{integer vector}  of length 2.
#'   Terms with lengts shorter then the lower bound \code{wordLengths[1]}
#'   or with lengts longer then the upper bound \code{wordLengths[2]}
#'   are discarded. Defaults to \code{control = list(wordLengths = c(3, Inf)}
#'   (i.e., term with length at least 3 will be used).
#'   }
#'
#'   \item{\code{weighting}}{A weighting function capable of
#'   handling a \code{TermDocumentMatrix}.
#'   It defaults to \code{weightTf} for term frequency weighting.
#'   Available weighting functions shipped with this package
#'   are \code{weightTf} and \code{\link{weightTfIdfParallel}}.
#'   For more functions look in \code{tm} package.
#'}
#' }
#'
#' @return The \code{\link[tm]{DocumentTermMatrix}} or \code{\link[tm]{TermDocumentMatrix}} crated from \code{texts}.
#'
#' @seealso \code{\link{RcppParallel}} \code{\link[RcppParallel]{setThreadOptions}}
#'  for threads parallel settings.
#' @seealso \code{\link{tm}} \code{\link[tm]{weighting}}
#'  for other weighting functions.
#'
#' @aliases ParallelDocumentTermMatrix DocumentTermMatrixParallel
#' @keywords ParallelDocumentTermMatrix DocumentTermMatrixParallel
#' ParallelTermDocumentMatrix TermDocumentMatrixParallel
#'
#' @examples
#' pdtm <-
#'  ParallelDocumentTermMatrix(c('This is a test', 'And another test') )
#' pdtm <-
#'  ParallelDocumentTermMatrix(
#'   c('This is a test test', 'And another test'),
#'   control = list(
#'    wordLengths = c(1, Inf),
#'    bounds = list(global = c(2, Inf)),
#'    threadsCount = 1,
#'    weighting = function(x) weightTfIdfParallel(x, normalize = TRUE, plus1inIdf = FALSE)
#'   )
#'  )

#' @rdname ParallelDocumentTermMatrix
#' @export
ParallelDocumentTermMatrix <- function(texts, control = list()) {
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_parallel
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname ParallelDocumentTermMatrix
#' @export
DocumentTermMatrixParallel <- ParallelDocumentTermMatrix


#' @rdname ParallelDocumentTermMatrix
#' @export
ParallelTermDocumentMatrix <- function(texts, control = list()) {
  process (
    texts = texts
    , control = control
    , func = tmParallel::Cpp_dtm_parallel
    , matrixKind = 'TermDocumentMatrix'
  )
}

#' @rdname ParallelDocumentTermMatrix
#' @export
TermDocumentMatrixParallel <- ParallelTermDocumentMatrix
