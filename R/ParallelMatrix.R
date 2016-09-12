#' ParallelDocumentTermMatrix
#'
#' Constructs a document-term matrix or a term-document matrix
#'
#'
#' @param \code{texts} - one of:
#' \itemize{
#'   \item \code{\link{vector}} of \code{\link{character}}
#'   \item \code{\link[tm]{Corpus}} from \code{\link{tm}} package
#' }
#'
#' @param \code{control} a named list of control options. Available options:
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
#'   }
#'
#'   \item{\code{vectorsResizeMemorySettings}}{A value must be an \code{integer vector}
#'   of length 2. When creating DocumentTermMatrix or TermDocumentMatrix the results vectors
#'   will be resized according to those setting. At firs their size will be rising exponentially
#'   (i.e. 1,2,4,8,... maximal elements in vector).
#'   The \code{vectorsResizeMemorySettings[1]} sets the values vectors lenght threshold after passing
#'   which result vectors will be realocated after each
#'   \code{vectorsResizeMemorySettings[2]} elements in each thread specified in threadsCount.
#'   By default their size will rise exponentially whole the time.
#'
#'   This setting has great impact on processing time and memory usage.
#'
#'   For more see \url{http://www.cplusplus.com/reference/vector/vector/} and
#'   \url{http://www.cplusplus.com/reference/vector/vector/reserve/}
#'   }
#'
#'   \item{\code{termsResizeMemorySettings}}{A value must be an \code{integer vector}
#'   of length 2. When creating DocumentTermMatrix or TermDocumentMatrix the terms vector
#'   will be resized according to those setting. At firs its size will be rising exponentially
#'   (i.e. 1,2,4,8,... maximal elements in vector).
#'   The \code{termsResizeMemorySettings[1]} sets the terms vector lenght threshold after passing
#'   which result vectors will be realocated after each
#'   \code{termsResizeMemorySettings[2]} elements in each thread specified in threadsCount.
#'   By default their size will rise exponentially whole the time.
#'
#'   This setting has great impact on processing time and memory usage.
#'
#'   For more see \url{http://www.cplusplus.com/reference/vector/vector/} and
#'   \url{http://www.cplusplus.com/reference/vector/vector/reserve/}
#'   }
#'   \item{\code{dataFrame}}{A named list used for setting when processing \code{data.frame},
#'   for more details look under \code{texts} parameter.
#'   }
#' }
#'
#' @param \code{docsIDs}{ Optional vector of IDs for \code{texts}. It must have the same lenght as the
#' \code{texts} parameter.
#' If not specyfied this parameter has \code{id} value from \code{meta} for \link[tm]{Corpus}
#' or has \code{as.character(1:length(texts))} value for other cases.
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
#'   docsIDs = c('test1','test2'),
#'   control = list(
#'    wordLengths = c(1, Inf),
#'    bounds = list(global = c(2, Inf)),
#'    threadsCount = 1,
#'    vectorsResizeMemorySettings = c(2048,1024),
#'    termsResizeMemorySettings = c(2048,1024),
#'    weighting = function(x) weightTfIdfParallel(x, normalize = TRUE, plus1inIdf = FALSE)
#'   )
#'  )
#'
#'

#' @rdname ParallelDocumentTermMatrix
#' @export
ParallelDocumentTermMatrix <- function(texts, control = list(), docsIDs = as.character(1:length(texts))) {
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_parallel
    , matrixKind = 'DocumentTermMatrix'
  )
}

#' @rdname ParallelDocumentTermMatrix
#' @export
DocumentTermMatrixParallel <- ParallelDocumentTermMatrix


#' @rdname ParallelDocumentTermMatrix
#' @export
ParallelTermDocumentMatrix <- function(texts, control = list(), docsIDs = as.character(1:length(texts))) {
  process (
    texts = texts
    , control = control
    , docsIDs = docsIDs
    , func = tmParallel:::Cpp_dtm_parallel
    , matrixKind = 'TermDocumentMatrix'
  )
}

#' @rdname ParallelDocumentTermMatrix
#' @export
TermDocumentMatrixParallel <- ParallelTermDocumentMatrix
