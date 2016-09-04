library(data.table)
library(devtools)
library(roxygen2)
document()


#library(tm)
#corpus <- Corpus(VectorSource(texts))

remove(list = ls())
tmParallel::mem_test()

load("tests/texts_25.RData")
tmParallel::mem_test()

dtm <- tmParallel::Cpp_dtm_parallel_v2_mem_test(texts,1,.Machine$integer.max,2,30)
dtm$RSS
tmParallel::mem_test()

dtm <- tmParallel::Cpp_dtm_original_mem_test(texts,1,.Machine$integer.max,2,30)
dtm$RSS
tmParallel::mem_test()

#load("tests/texts_50.RData")
#tmParallel::mem_test()
#dtm <- tmParallel::Cpp_dtm_parallel_v2_mem_test(texts,1,.Machine$integer.max,3,.Machine$integer.max)
tmParallel::mem_test()


# texts <- merged.d$content
#
# save.image("C:/Projects/_mm/tmParallel/tests/texts_100.RData")
# texts <- texts[1:(length(texts)/2)]
# save.image("C:/Projects/_mm/tmParallel/tests/texts_50.RData")
# texts <- texts[1:(length(texts)/2)]
# save.image("C:/Projects/_mm/tmParallel/tests/texts_25.RData")
# texts <- texts[1:(length(texts)/25)]
# save.image("C:/Projects/_mm/tmParallel/tests/texts_1.RData")

library(data.table)
library(devtools)
library(roxygen2)
document()

library(profvis)
library(tm)
profvis({
  load("tests/texts_1.RData")
  texts<- texts[1:5000]
  tmParallel::mem_test()
  dtmPO <- tmParallel::ParallelDocumentTermMatrix_original(texts)
  tmParallel::mem_test()
  dtmP2 <- tmParallel::ParallelDocumentTermMatrix_v2(texts)
  tmParallel::mem_test()
  dtmP1 <- tmParallel::ParallelDocumentTermMatrix_v1(texts)
  tmParallel::mem_test()
  corpus <- Corpus(VectorSource(texts))
  tmParallel::mem_test()
  tdm <- TermDocumentMatrix(corpus,control = list())
  tmParallel::mem_test()
  dtm <- DocumentTermMatrix(corpus,control = list())
  tmParallel::mem_test()
})

document()
dtmP2 <- tmParallel::ParallelDocumentTermMatrix_v2(texts, control = list(weighting = weightTfIdfParallel))
dtmP2 <- tmParallel::ParallelDocumentTermMatrix_v2(texts)

tmParallel::TfIdfParallel(c(1,2,3,1),c(1,1,1,2),c(1,1,1,1),3,2,FALSE,FALSE)
