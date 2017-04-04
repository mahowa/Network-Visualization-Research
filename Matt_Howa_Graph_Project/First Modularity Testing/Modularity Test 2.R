## Load package
library(igraph)
library(TDA)

#Neural Network
modularityTest2 <- function(){
  
  xlist<-read.graph("C:\\Users\\jdbba\\OneDrive\\Documents\\School\\Research\\DataSets\\Neural network\\celegansneural\\celegansneural.gml", format="gml")
  plot(xlist)
  
  ##Persistant Homology of 1st graph
  Diag<-ripsDiag(distances(xlist),1,6, dist = "euclidean", library = "Dionysus",location = TRUE, printProgress = FALSE)
  plot(Diag[["diagram"]], col='red')
  
  ##Cluster WalkTrap
  #Modularity
  wtc <- cluster_edge_betweenness(xlist)
  plot(wtc, xlist )
  
  
  ##Persistant Homology of modular graph
  mDiag <- ripsDiag(merges(wtc),1,20, dist = "euclidean", library = "Dionysus",location = TRUE, printProgress = FALSE)
  plot(mDiag[["diagram"]], col='red')
  
  bottleneck(Diag[["diagram"]],mDiag[["diagram"]])
}


modularityTest2()









#modularity(wtc)
#modularity_matrix(xlist, membership(wtc))
#ripsDiag(modularity_matrix(xlist, membership(wtc)), 1, 10)
#plot(modularity_matrix(xlist, membership(wtc)))
#plot(ripsDiag(modularity_matrix(xlist, membership(wtc)), 1, 10))

##Cluster fast-greedy
#fc <- cluster_fast_greedy(xlist)
#modularity(fc)
#modularity_matrix(xlist, membership(fc))

##Edge Betweenness
#eb <- cluster_edge_betweenness(xlist)
#modularity(eb)
#modularity_matrix(xlist, membership(eb))

##Cluster Spinglass
#cluster_spinglass(xlist, spins=2)
#cluster_spinglass(xlist, vertex=1)


