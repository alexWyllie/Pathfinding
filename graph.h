//
// Created by Alex Wyllie on 8/24/22.
//

#ifndef PATHFINDING_GRAPH_H
#define PATHFINDING_GRAPH_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// TODO: Maybe force directed single edge for all cases?

int DIRECTED = 1;  // Directed edges?
int MULTIEDGE = 0;  // Allow multiple edges between pairs of nodes?

struct graph {
    long nodes;
    long nodes_size;  // Number of nodes that have been malloc'd in *node
    long edges;  // number of edges
    struct successors {
        char name[256];  // Name of the node
        long destinations;  // Number of destinations for each node
        long destinations_size;  // Number of destinations that have been alloc'd in *edge
        struct destination {
            long node;  // Destination's node id
            double distance;  // edge weight
        } *edge[0];
    } *node[0];
};

typedef struct graph *Graph;

Graph graph_create();

void graph_destroy(Graph g);

long graph_add_edge(Graph g, long source, long sink, double distance);

void graph_modify_edge_distance(Graph g, long source, long sink, double distance);

long graph_add_node(Graph g, char name[256]);

void graph_node_modify_name(Graph g, long id, char name[256]);

long graph_node_count(Graph g);

long graph_edge_count(Graph g);

long graph_destinations_out(Graph g, long source);

int graph_has_edge(Graph g, long source, long sink, long* destination_number);

Graph graph_from_txt(char* filepath);

void graph_to_txt(Graph g, char* filepath);

#endif //PATHFINDING_GRAPH_H
