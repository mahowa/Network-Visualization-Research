## Load package
library(igraph)
library(TDA)


modularityTest <- function(){
  #ripsDiag(X, maxdimension, maxscale, dist = "euclidean", library = "GUDHI",
  #         location = FALSE, printProgress = FALSE)
  
  xlist<-read.graph("C:\\Users\\jdbba\\OneDrive\\Documents\\School\\Research\\DataSets\\karate\\karate.gml", format="gml")
  plot(xlist)
  
  
  distances(xlist)
  
  
  ##Persistant Homology of 1st graph
  Diag<-ripsDiag(distances(xlist),1,6)
  plot(Diag[["diagram"]], col='red')
  
  ##Cluster WalkTrap
  #Modularity
  wtc <- cluster_walktrap(xlist)
  plot(wtc, xlist )
  
  
  ##Persistant Homology of modular graph
  mDiag <- ripsDiag(wtc,1,50, dist = "euclidean", library = "Dionysus",location = FALSE, printProgress = FALSE)
  plot(mDiag[["diagram"]], col='red')
  
  summary(Diag)
  summary(mDiag)
  
  Diag
  mDiag
  
  bottleneck(Diag,mDiag)
  
  #y <- ripsDiag(xlist,1,1)
  merges(wtc)
  xlist
  x <- ripsDiag(merges(wtc),1,10)
  x
  y
  bottleneck(x,y,1)
  plot(wtc, xlist , col = membership(wtc),
       mark.groups = communities(wtc), edge.color = c("black", "red")[crossing(wtc,xlist) + 1])
  
}


modularityTest()








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


