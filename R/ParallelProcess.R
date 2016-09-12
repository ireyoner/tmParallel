#' @importFrom Rcpp sourceCpp
#' @importFrom RcppParallel RcppParallelLibs
#' @importFrom slam simple_triplet_matrix
#' @useDynLib tmParallel

process <- function(
  texts,
  control = list(),
  docsIDs = NA,
  func,
  matrixKind = c("DocumentTermMatrix","TermDocumentMatrix")
) {

  stopifnot(is.list(control))

  if(inherits(texts,"Corpus")){
    if(is.na(docsIDs)){
      docsIDs <- unlist(meta(texts,"id"))
    }
    texts<- unname(unlist(sapply(texts, "[", "content")),force = TRUE)
  }else if(is.na(docsIDs)){
    as.character(1:length(texts))
  }

  stopifnot(is.character(texts))
  stopifnot(length(docsIDs) == length(texts))
  stopifnot(is.function(func))

  matrixKind <- match.arg(matrixKind)

  threadsCount <- control$threadsCount
  if (length(threadsCount) == 1L && is.numeric(threadsCount)) {
    RcppParallel::setThreadOptions(numThreads = threadsCount)
  }

  ## Ensure global bounds
  bg <- control$bounds$global
  min_term_freq <-
    if (length(bg) == 2L && is.numeric(bg)) bg[1]
    else 1L
  max_term_freq <-
    if (length(bg) == 2L && is.numeric(bg)) min(bg[2], .Machine$integer.max)
    else .Machine$integer.max

  ## Filter out too short or too long terms
  wl <- control$wordLengths
  min_word_length <-
    if (length(wl) == 2L && is.numeric(wl[1])) wl[1]
    else 1L
  max_word_length <-
    if (length(wl) == 2L && is.numeric(wl[2])) min(wl[2], .Machine$integer.max)
    else .Machine$integer.max

  ## Get vectors resize settings:
  vr <- control$vectorsResizeMemorySettings
  vectors_resize_threshold <-
    if (length(vr) == 2L && is.numeric(vr[1])) max(vr[1], 1L)
    else .Machine$integer.max
  vectors_resize_size <-
    if (length(vr) == 2L && is.numeric(vr[2])) max(vr[2], 1L)
    else 1024L

  ## Get terms resize settings:
  tr <- control$termsResizeMemorySettings
  terms_resize_threshold <-
    if (length(tr) == 2L && is.numeric(tr[1])) max(tr[1], 1L)
    else .Machine$integer.max
  terms_resize_size <-
    if (length(tr) == 2L && is.numeric(tr[2])) max(tr[2], 1L)
    else 1024L

  m <-
    func(
      texts,
      min_term_freq,
      max_term_freq,
      min_word_length,
      max_word_length,
      vectors_resize_size,
      vectors_resize_threshold,
      terms_resize_size,
      terms_resize_threshold
    )

  if (matrixKind == "DocumentTermMatrix") {
    m <- slam::simple_triplet_matrix(
      i = m$i,
      j = m$j,
      v = m$v,
      nrow = length(texts),
      ncol = length(m$terms),
      dimnames =
        list(Docs = docsIDs,
             Terms = m$terms)
    )
    class(m) <- c("DocumentTermMatrix", "simple_triplet_matrix")
  }else if (matrixKind == "TermDocumentMatrix"){
    m <- slam::simple_triplet_matrix(
      i = m$j,
      j = m$i,
      v = m$v,
      nrow = length(m$terms),
      ncol = length(texts),
      dimnames =
        list(Terms = m$terms,
             Docs = docsIDs)
    )
    class(m) <- c("TermDocumentMatrix", "simple_triplet_matrix")
  }else{
    stop("Created object must be either 'DocumentTermMatrix' or 'TermDocumentMatrix'")
  }

  weighting <- control$weighting
  if (is.function(weighting))
    m <- weighting(m)
  else
    attr(m, "weighting") <- c("term frequency", "tf")

  m
}
