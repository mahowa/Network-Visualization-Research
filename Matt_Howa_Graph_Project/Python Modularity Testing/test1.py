from hac import GreedyAgglomerativeClusterer
from pandas import *
import dipha_utils
import community
import networkx as nx
import os
import helpers
import read_matrix

import matplotlib.pyplot as plt

read_matrix.read()


clusterer = GreedyAgglomerativeClusterer()

# f = open("C:\Users\jdbba\OneDrive\Documents\School\Research\DataSets\Neural network\celegansneural\celegansneural.gml")

G = nx.karate_club_graph()

size = G.number_of_nodes()
Matrix = helpers.create_shortest_path_matrix(G, size)

dFrame = DataFrame(Matrix)

dFrame.to_csv("Matrix_Files/M1.csv")


file_path = "C:\Users\jdbba\OneDrive\Documents\School\Research\Tests\Python Modularity Testing\dipha\Test"
infile1 = file_path + "\distances.bin"
infile2 = file_path + "\distances2.bin"


# save matrix to file for output to dipha
dipha_utils.writeDistBin(Matrix, infile1)



# run fast modularity
karate_dendrogram = clusterer.cluster(G)

# take the best # of clusters
clusters = karate_dendrogram.clusters(4)

labels = karate_dendrogram.labels(4)
modularity = community.modularity(labels, G)

print modularity

# another modularity algorithm
# part = community.best_partition(G)
#
# modularity = community.modularity(labels, G)
# modularity2 = community.modularity(part, G)
#
# size2 = len(clusters)
# Matrix2 = [[0 for x in range(size2)] for y in range(size2)]
#
#
# for i in range(0, size2):
#     for j in range(0, size2):
#         if j == i or j < i:
#             continue
#
#         for node in clusters[i]:
#             Matrix2[i][j] += helpers.num_edges(node, clusters[j], Matrix)
#             Matrix2[j][i] += helpers.num_edges(node, clusters[j], Matrix)
#
# Matrix3 = helpers.flip_edge_ratio(Matrix2, size2)
# Matrix4 = helpers.edge_ratio(Matrix2, size2)

G2 = community.induced_graph(labels, G)
size2 = G2.number_of_nodes()
Matrix2 = helpers.create_shortest_path_matrix(G2, size2, "weight")
dFrame2 = DataFrame(Matrix2)
dFrame2.to_csv("Matrix_Files/M2.csv")

# save matrix to file for output to dipha
dipha_utils.writeDistBin(Matrix2, infile2)
#os.system("run_dipha.bat")


# nx.draw(G)
# plt.savefig("G.png")


# nx.draw(G2)
# plt.savefig("G2.png")




