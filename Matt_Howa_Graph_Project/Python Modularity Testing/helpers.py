import networkx as nx


def create_shortest_path_matrix(G, size, weighted='none'):
    Matrix = [[0 for x in range(size)] for y in range(size)]
    for i in range(0, size):
        for j in range(0, size):

            if i <= j:
                continue

            path = nx.shortest_path(G, source=i, target=j, weight=weighted)
            node_hops = len(path) - 1
            dist = 0

            if weighted is "weight":
                if node_hops > 0:
                    for k in range(0, node_hops):
                        dist += G.edge[path[k]][path[k+1]]['weight']

            else:
                dist = node_hops

            Matrix[i][j] = dist
            Matrix[j][i] = dist

    return Matrix


def num_edges(source_node, cluster, matrix):
    edges = 0
    for node in cluster:
        if matrix[source_node][node] == 1:
            edges += 1

    return edges


def total_edges(matrix, length):
    edges = 0
    for i in range(0, length):
        for j in range(0, length):
            if i <= j:
                continue
            edges += matrix[i][j]

    return edges


def flip_edge_ratio(matrix, length):
    Matrix = [[0 for x in range(length)] for y in range(length)]
    edge_total = total_edges(matrix, length)
    for i in range(0, length):
        for j in range(0, length):

            if i <= j:
                continue

            new_value = 0

            if matrix[i][j] > 0:
                new_value = float(edge_total) / float(matrix[i][j])

            Matrix[i][j] = new_value
            Matrix[j][i] = new_value

    return Matrix


def edge_ratio(matrix, length):
    Matrix = [[0 for x in range(length)] for y in range(length)]
    edge_total = total_edges(matrix, length)
    for i in range(0, length):
        for j in range(0, length):

            if i <= j:
                continue

            new_value = float(matrix[i][j]) / float(edge_total)
            Matrix[i][j] = new_value
            Matrix[j][i] = new_value

    return Matrix
