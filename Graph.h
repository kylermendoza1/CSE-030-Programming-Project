#ifndef GRAPH_H
#define GRAPH_H

#include <LinkedList.h>
#include <ArrayList.h>
#include <HashTable.h>
#include <Queue.h>
#include <Stack.h>
#include <ostream>
#include <string>

struct Edge;  // forward declaration

// ── Vertex ──────────────────────────────
struct Vertex {
    std::string data;
    ArrayList<Edge *> edgeList;

    Vertex(std::string data) { this->data = data; }
    ~Vertex();  // declared here, defined after Edge
};

inline std::ostream &operator<<(std::ostream &os, Vertex *v) {
    os << v->data;
    return os;
}

// ── Edge ─────────────────────────────────
struct Edge {
    Vertex *from;
    Vertex *to;
    int cost;
    int time;

    Edge(Vertex *from, Vertex *to, int cost, int time) {
        this->from = from;
        this->to   = to;
        this->cost = cost;
        this->time = time;
    }
};

// ── Vertex destructor (defined HERE after Edge is complete) ──
inline Vertex::~Vertex() {
    for (int i = 0; i < edgeList.size(); i++) {
        delete edgeList[i];
    }
}

inline std::ostream &operator<<(std::ostream &os, Edge *e) {
    os << "(" << e->from << ", " << e->to
       << ") - cost: " << e->cost
       << ", time: "   << e->time;
    return os;
}

// ─────────────────────────────────────────
// Waypoint
// ─────────────────────────────────────────
struct Waypoint {
    Waypoint *parent;
    Vertex   *vertex;
    ArrayList<Waypoint *> children;
    int partialCost;
    int partialTime;
    int stops;

    Waypoint(Vertex *v) {
        parent      = nullptr;
        vertex      = v;
        partialCost = 0;
        partialTime = 0;
        stops       = 0;
    }

    // Registers every new waypoint in the tracking list
    void expand(ArrayList<Waypoint *> &all) {
        for (int i = 0; i < vertex->edgeList.size(); i++) {
            Waypoint *temp    = new Waypoint(vertex->edgeList[i]->to);
            all.append(temp);
            temp->parent      = this;
            temp->partialCost = partialCost + vertex->edgeList[i]->cost;
            temp->partialTime = partialTime + vertex->edgeList[i]->time;
            temp->stops       = stops + 1;
            children.append(temp);
        }
    }
};

inline std::ostream &operator<<(std::ostream &os, Waypoint *wp) {
    std::string p = "null";
    if (wp->parent != nullptr) {
        p = wp->parent->vertex->data;
    }
    os << p << " -> " << wp->vertex->data
       << " (cost: "  << wp->partialCost
       << ", time: "  << wp->partialTime
       << ", stops: " << wp->stops << ")";
    return os;
}

// ─────────────────────────────────────────
// Search mode
// ─────────────────────────────────────────
enum SearchMode { BY_COST, BY_TIME, BY_STOPS };

// ─────────────────────────────────────────
// Graph
// ─────────────────────────────────────────
struct Graph {
    ArrayList<Vertex *> vertices;

    void addVertex(Vertex *v) { vertices.append(v); }

    void addEdge(Vertex *x, Vertex *y, int cost, int time) {
        x->edgeList.append(new Edge(x, y, cost, time));
        y->edgeList.append(new Edge(y, x, cost, time));
    }

    void addDirectedEdge(Vertex *x, Vertex *y, int cost, int time) {
        x->edgeList.append(new Edge(x, y, cost, time));
    }

    int priority(Waypoint *wp, SearchMode mode) {
        if (mode == BY_COST) return wp->partialCost;
        if (mode == BY_TIME) return wp->partialTime;
        return wp->stops;
    }

    // ── BFS: fewest stops ─────────────────────────────────────────────────────
    Waypoint *fewestStops(Vertex *start, Vertex *destination, ArrayList<Waypoint *> &all) {
        Queue<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);
        all.append(first);
        frontier.enqueue(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (!frontier.isEmpty()) {
            result = frontier.dequeue();
            if (result->vertex == destination) return result;

            result->expand(all);

            for (int i = 0; i < result->children.size(); i++) {
                if (!seen.search(result->children[i]->vertex->data)) {
                    frontier.enqueue(result->children[i]);
                    seen.insert(result->children[i]->vertex->data);
                }
            }
        }
        return nullptr;
    }

    // ── UCS: cheapest or fastest ──────────────────────────────────────────────
    Waypoint *ucs(Vertex *start, Vertex *destination, SearchMode mode, ArrayList<Waypoint *> &all) {
        ArrayList<Waypoint *> frontier;
        HashTable<std::string> seen;

        Waypoint *first = new Waypoint(start);
        all.append(first);
        frontier.append(first);
        seen.insert(first->vertex->data);

        Waypoint *result = nullptr;

        while (frontier.size() != 0) {
            result = frontier.removeLast();
            if (result->vertex == destination) return result;

            result->expand(all);

            for (int i = 0; i < result->children.size(); i++) {
                if (!seen.search(result->children[i]->vertex->data)) {
                    frontier.append(result->children[i]);

                    // Insertion sort descending so removeLast() pops lowest
                    int j = frontier.size() - 1;
                    while (j > 0 &&
                           priority(frontier.data[j], mode) >
                           priority(frontier.data[j - 1], mode)) {
                        Waypoint *tmp      = frontier.data[j];
                        frontier.data[j]   = frontier.data[j - 1];
                        frontier.data[j-1] = tmp;
                        j--;
                    }

                    seen.insert(result->children[i]->vertex->data);
                } else {
                    // Check if this child is in frontier with worse priority
                    Waypoint *worsePath = nullptr;
                    for (int k = 0; k < frontier.size(); k++) {
                        if (frontier[k]->vertex->data ==
                            result->children[i]->vertex->data) {
                            if (priority(frontier[k], mode) >
                                priority(result->children[i], mode)) {
                                worsePath = frontier[k];
                                break;
                            }
                        }
                    }

                    if (worsePath) {
                        // Re-parent frontier nodes pointing to worse waypoint
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k]->parent == worsePath)
                                frontier[k]->parent = result->children[i];
                        }
                        // Replace worse with better (no delete — tracked in all)
                        for (int k = 0; k < frontier.size(); k++) {
                            if (frontier[k] == worsePath) {
                                frontier[k] = result->children[i];
                                break;
                            }
                        }
                        // Re-sort frontier
                        for (int a = 1; a < frontier.size(); a++) {
                            int b = a;
                            while (b > 0 &&
                                   priority(frontier.data[b], mode) >
                                   priority(frontier.data[b-1], mode)) {
                                Waypoint *x        = frontier.data[b];
                                frontier.data[b]   = frontier.data[b-1];
                                frontier.data[b-1] = x;
                                b--;
                            }
                        }
                    }
                }
            }
        }
        return nullptr;
    }

    // ── Convenience wrappers ──────────────────────────────────────────────────
    Waypoint *cheapest(Vertex *start, Vertex *destination, ArrayList<Waypoint *> &all) {
        return ucs(start, destination, BY_COST, all);
    }

    Waypoint *fastest(Vertex *start, Vertex *destination, ArrayList<Waypoint *> &all) {
        return ucs(start, destination, BY_TIME, all);
    }

    Waypoint *bfs(Vertex *start, Vertex *destination, ArrayList<Waypoint *> &all) {
        return fewestStops(start, destination, all);
    }
};

inline std::ostream &operator<<(std::ostream &os, const Graph &g) {
    for (int i = 0; i < g.vertices.size(); i++) {
        os << g.vertices[i]->edgeList << std::endl;
    }
    return os;
}

#endif