//
// Created by Alex Wyllie on 8/24/22.
//

#include "graph.h"

Graph graph_create() {
    Graph g;

    g = malloc(sizeof(Graph));
    assert(g);

    g->nodes = 0;
    g->nodes_size = 0;
    g->edges = 0;

    return g;
}

void graph_destroy(Graph g) {
    long i, j;
    for (i = 0; i < g->nodes_size; i++) {
        for (j = 0; j < g->node[i]->destinations_size; j++) {
            free(g->node[i]->edge[j]);
        }
        free(g->node[i]);
    }
    free(g);
}

long graph_add_edge(Graph g, long source, long sink, double distance) {
    // TODO: Non directed edge parallel insertion
    long first, last, middle, insertat;  // Will use binary search at some point
    struct destination* node;

    // Make sure that both nodes already exist
    assert(source >= 0);
    assert(sink >= 0);
    assert(source <= g->nodes);
    assert(sink <= g->nodes);

    // If there are no edges from the source, add one
    if (g->node[source]->destinations == 0) {
        if (g->node[source]->destinations_size == 0) {
            g->node[source]->destinations_size = 1;
            g->node[source]->edge[0] = malloc(sizeof(struct destination));
        }
        assert(g->node[source]->edge[0]);

        g->node[source]->edge[0]->node = sink;
        g->node[source]->edge[0]->distance = distance;
        g->node[source]->destinations = 1;
        g->edges++;
        return 0;
    }

    // Insert into the list of edges sorted by node id
    first = 0;
    last = g->node[source]->destinations - 1;
    while (first < last) {
        middle = (first + last) / 2;
        if (g->node[source]->edge[middle]->node < sink) {
            first = middle + 1;
        }
        else if (g->node[source]->edge[middle]->node == sink) {
            if (!MULTIEDGE) {
                return middle;
            }
            else {
                insertat = middle+1;
            }
        }
        else {
            last = middle - 1;
        }
    }
    if (first > last) {
        insertat = first;  // TODO: Check that this is right?
    }

    while (g->node[source]->destinations >= g->node[source]->destinations_size) {
        g->node[source]->destinations_size *= 2;
        g->node[source] = realloc(g->node[source], sizeof(struct successors) + sizeof(struct destination *) * g->node[source]->destinations_size);
    }
    assert(g->node[source]->edge[g->node[source]->destinations]);

    for (last = g->node[source]->destinations - 1; last >= insertat; last--) {
        g->node[source]->edge[last + 1] = g->node[source]->edge[last];
    }

    node = malloc(sizeof(struct destination));
    node->node = sink;
    node->distance = distance;

    g->node[source]->edge[insertat] = node;
    g->node[source]->destinations++;
    g->edges++;
}

void graph_modify_edge_distance(Graph g, long source, long sink, double distance) {
    long *destination = 0;
    assert(graph_has_edge(g, source, sink, destination));

    g->node[source]->edge[*destination]->distance = distance;
}

long graph_add_node(Graph g, char name[256]) {
    long id = g->nodes;

    while (id >= g->nodes_size) {
        g->nodes_size *= 2;
        g = realloc(g, sizeof(struct graph) + sizeof(struct successors *) * g->nodes_size);
    }

    g->node[id] = malloc(sizeof(struct successors));
    snprintf(g->node[id]->name, sizeof(g->node[id]->name), "%s", name);
    g->node[id]->destinations = 0;
    g->node[id]->destinations_size = 0;

    g->nodes++;
    return id;
}

void graph_node_modify_name(Graph g, long id, char name[256]) {
    assert(id >= 0);
    assert(id < g->nodes);
    snprintf(g->node[id]->name, sizeof(g->node[id]->name), "%s", name);
}

long graph_node_count(Graph g) {
    return g->nodes;
}

long graph_edge_count(Graph g) {
    return g->edges;
}

long graph_destinations_out(Graph g, long source) {
    assert(source >= 0);
    assert(source < g->nodes);

    return g->node[source]->destinations;
}

int graph_has_edge(Graph g, long source, long sink, long* destination_number) {
    long first, middle, last;

    assert(source >= 0);
    assert(sink >= 0);
    assert(source < g->nodes);
    assert(sink < g->nodes);

    first = 0;
    last = g->node[source]->destinations - 1;
    while (first < last) {
        middle = (first + last) / 2;
        if (g->node[source]->edge[middle]->node < sink) {
            first = middle + 1;
        }
        else if (g->node[source]->edge[middle]->node == sink) {
            *destination_number = middle;
        }
        else {
            last = middle - 1;
        }
    }

    return last < first;
}

Graph graph_from_txt(char* filepath) {
    FILE *fp;
    long i, j;
    Graph g;

    fp = fopen(filepath, "r+");
    assert(fp);
    g = graph_create();

    fscanf(fp, "%i, %i, %ld, %ld, %ld\n", &DIRECTED, &MULTIEDGE, &g->nodes, &g->nodes_size, &g->edges);
    g = realloc(g, sizeof(struct graph) + sizeof(struct successors*) * g->nodes_size);

    for (i = 0; i < g->nodes; i++) {
        assert(g->node[i]);
        fscanf(fp, "%ld, %ld, ", &g->node[i]->destinations, &g->node[i]->destinations_size);
        g->node[i] = realloc(g->node[i], sizeof(struct successors) + sizeof(struct destination*) * g->node[i]->destinations_size);

        for (j = 0; j < g->node[i]->destinations; j++) {
            g->node[i]->edge[j] = realloc(g->node[i]->edge[j], sizeof(struct destination));
            assert(g->node[i]->edge[j]);

            fscanf(fp, "%ld, %lg, ", &g->node[i]->edge[j]->node, &g->node[i]->edge[j]->distance);
        }
        fscanf(fp, "%s\n", g->node[i]->name);
    }
    fclose(fp);
    return g;
}

void graph_to_txt(Graph g, char* filepath) {
    FILE *fp;
    long i, j;
    fp = fopen(filepath, "w+");
    assert(fp);

    fprintf(fp, "%i, %i, %ld, %ld, %ld\n", DIRECTED, MULTIEDGE, g->nodes, g->nodes_size, g->edges);
    for (i = 0; i < g->nodes; i++) {
        fprintf(fp, "%ld, %ld, ", g->node[i]->destinations, g->node[i]->destinations_size);
        for (j = 0; j < g->node[i]->destinations; j++) {
            fprintf(fp, "%ld, %lg, ", g->node[i]->edge[j]->node, g->node[i]->edge[j]->distance);
        }
        fprintf(fp, "%s\n", g->node[i]->name);
    }
    fclose(fp);
}
