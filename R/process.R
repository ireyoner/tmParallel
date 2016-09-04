#' @importFrom Rcpp sourceCpp
#' @importFrom RcppParallel RcppParallelLibs
#' @import slam
#' @importFrom slam simple_triplet_matrix
#' @useDynLib tmParallel

process <- function(texts, control = list() , func, matrixKind = c('DocumentTermMatrix','TermDocumentMatrix')) {
  if(inherits(texts,"Corpus")){
    texts<- unname(unlist(sapply(texts, '[', "content")),force = TRUE)
  }
  stopifnot(is.character(texts))
  stopifnot(is.list(control))
  stopifnot(is.function(func))

  matrixKind <- match.arg(matrixKind)

  threadsCount <- control$threadsCount
  if (length(threadsCount) == 1L && is.numeric(threadsCount)) {
    RcppParallel::setThreadOptions(numThreads = threadsCount)
  }

  ## Ensure global bounds
  bg <- control$bounds$global
  min_term_freq <- if (length(bg) == 2L && is.numeric(bg)) bg[1] else 1L
  max_term_freq <- if (length(bg) == 2L && is.numeric(bg))
    min(bg[2], .Machine$integer.max) else .Machine$integer.max

  ## Filter out too short or too long terms
  wl <- control$wordLengths
  min_word_length <- if (is.numeric(wl[1])) wl[1] else 3L
  max_word_length <- if (is.numeric(wl[2]))
    min(wl[2], .Machine$integer.max) else .Machine$integer.max

  m <-
    func(texts,min_term_freq, max_term_freq, min_word_length, max_word_length)
  if (matrixKind == "DocumentTermMatrix") {
    m <- slam::simple_triplet_matrix(
      i = m$i,
      j = m$j,
      v = m$v,
      nrow = length(texts),
      ncol = length(m$terms),
      dimnames =
        list(Docs = as.character(1:length(texts)),
             Terms = m$terms)
    )
    class(m) <- c("DocumentTermMatrix", "simple_triplet_matrix")
  }else{
    m <- slam::simple_triplet_matrix(
      i = m$j,
      j = m$i,
      v = m$v,
      nrow = length(m$terms),
      ncol = length(texts),
      dimnames =
        list(Terms = m$terms,
             Docs = as.character(1:length(texts)))
    )
    class(m) <- c("TermDocumentMatrix", "simple_triplet_matrix")
  }
  weighting <- control$weighting
  if (is.function(weighting))
    m <- weighting(m)
  else
    attr(m, "weighting") <- c("term frequency", "tf")
  m
}
