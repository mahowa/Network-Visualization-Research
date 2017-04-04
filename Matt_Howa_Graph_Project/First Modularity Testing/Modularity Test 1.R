## Load package
library(igraph)
library(TDA)

#Zachs karate club
modularityTest1 <- function(){

  xlist<-read.graph("C:\\Users\\jdbba\\OneDrive\\Documents\\School\\Research\\DataSets\\Zach's karate club\\karate\\karate.gml", format="gml")
  plot(xlist)

  ##Persistant Homology of 1st graph
  Diag<-ripsDiag(distances(xlist),1,6, dist = "euclidean", library = "Dionysus",location = TRUE, printProgress = FALSE)
  plot(Diag[["diagram"]], col='red')
  
  ##Cluster WalkTrap
  #Modularity
  wtc<-cluster_leading_eigen(xlist)
  plot(wtc, xlist )
  
  merges(wtc)
  
  
  ##Persistant Homology of modular graph
  mDiag <- ripsDiag(merges(wtc),1,25, dist = "euclidean", library = "Dionysus",location = TRUE, printProgress = FALSE)
  plot(mDiag[["diagram"]], col='red')
  
  bottleneck(Diag[["diagram"]],mDiag[["diagram"]])
}


modularityTest1()








