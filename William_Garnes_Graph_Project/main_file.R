library(Rcpp)
library(package="TDA")

sourceCpp("graph_creating_file.cpp")
#CreateGraph("node_locations.txt")
CreateGraph("smallGraph.txt")

GiveGraph <- function(){
  while(More() == 1){
    GiveNextNode()
  }
}

RunCommand <- function(filename){
  ChangeWriteFileName(filename)
  CoersenGraph()
  GiveGraph()
  CreateTulipFile()
}

size1 = GiveMatrixLength()
v1 = GiveDistanceMatrix()
m1 = matrix(v1,nrow = size1, ncol = size1, byrow = TRUE)
d1 = ripsDiag(m1,maxdimension = 1,maxscale = 5, dist = "arbitrary")

GetNextBottleNeck <- function(){
  CoersenGraph()
  size2 = GiveMatrixLength()
  v2 = GiveDistanceMatrix()
  m2 = matrix(v2,nrow = size2, ncol = size2, byrow = TRUE)
  d2 = ripsDiag(m2,maxdimension = 1,maxscale = 5,dist = "arbitrary")
  print(bottleneck(d1[["diagram"]],d2[["diagram"]],dimension = 1))
}