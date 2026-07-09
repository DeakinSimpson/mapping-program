#include "ch.h"
#include <queue>
#include <vector>
#include <algorithm>
#include <cfloat>
#include <climits>
#include <ctime>
#include <cstdio>

#define HOP_LIMIT 5

using HeapEntry = std::pair<double, long long>;
using MinHeap   = std::priority_queue<HeapEntry, std::vector<HeapEntry>, std::greater<HeapEntry>>;

// initialises contraction hierarchy
static CHGraph *ch_init(Graph *g) {
    CHGraph *ch_g = new CHGraph();
    ch_g->g = g;

    // initialises rank to be -1 (uncontracted) for the whole graph
    ch_g->rank.resize(g->nodes.size(), -1);

    return ch_g;
}

// resets the dist array to DBL_MAX
static void reset_search_state(std::vector<double> &dist, std::vector<long long> &touched, int &touched_count) {
    for (int i = 0; i < touched_count; i++) {
        dist[touched[i]] = DBL_MAX;
    }
}

// resets visited and hops
static void clear_visited_hops(std::vector<long long> &visited, std::vector<int> &hops, std::vector<long long> &touched, int &touched_count) {
    for (int i = 0; i < touched_count; i++) {
        visited[touched[i]] = 0;
        hops[touched[i]]    = INT_MAX;
    }
}

// checks the speed limit, if it is 0 (a contracted node) just return weight
static double edge_travel_time(const AdjEdge &edge) {
    if (edge.speed_limit == 0) return edge.weight;
    return edge.weight / (edge.speed_limit / 3.6);
}

/*
runs a dijkstra from v outwards to max_dist
*/
static std::vector<double> &local_dijkstra(CHGraph *ch_g, std::vector<AdjList> &adj, long long src, long long skip_node, double max_dist, std::vector<double> &dist, std::vector<long long> &visited, std::vector<int> &hops, MinHeap &heap, std::vector<long long> &touched, int &touched_count) {
    reset_search_state(dist, touched, touched_count);

    // set all values to 0
    touched_count = 0;
    heap = MinHeap{};
    dist[src] = 0;
    hops[src] = 0;
    touched[touched_count++] = src;

    // push src to heap
    heap.push({0.0, src});

    while (!heap.empty()) {
        auto [d, u] = heap.top(); heap.pop();

        // continue states
        if (visited[u])           continue;
        if (u == skip_node)       continue;
        if (hops[u] >= HOP_LIMIT) continue;
        if (dist[u] > max_dist)   break;

        visited[u] = 1; // mark current node as visited

        for (auto &edge : adj[u]) {
            long long v = edge.dst_index;
            if (v == -1 || visited[v] || ch_g->rank[v] != -1) continue;

            double alt = dist[u] + edge_travel_time(edge);
            if (alt < dist[v]) {
                if (dist[v] == DBL_MAX) touched[touched_count++] = v;
                dist[v] = alt;
                hops[v] = hops[u] + 1;
                heap.push({alt, v});
            }
        }
    }

    // reset visited and hops only — dist stays valid for the caller to read
    clear_visited_hops(visited, hops, touched, touched_count);

    return dist;
}

static double find_farthest_outgoing_travel_time(std::vector<AdjList> &adj, CHGraph *ch_g, long long v, long long u, double u_time) {
    double max_dist = 0.0;
    for (auto &w_edge : adj[v]) {
        long long w = w_edge.dst_index;

        if (w == -1 || w == v || w == u || ch_g->rank[w] != -1) continue;

        double t = u_time + edge_travel_time(w_edge);

        if (t > max_dist) max_dist = t;
    }
    return max_dist;
}

/*
Gets the number of shortcuts between nodes that can be made if v was removed
*/
static int edge_difference(CHGraph *ch_g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, long long v, std::vector<double> &dist, std::vector<long long> &visited, std::vector<int> &hops,MinHeap &heap, std::vector<long long> &touched, int &touched_count) {
    int shortcuts = 0;

    for (auto &u_edge : adj_r[v]) {
        // initialise u variables
        long long u      = u_edge.dst_index;
        double    u_time = edge_travel_time(u_edge);

        if (u == -1 || u == v || ch_g->rank[u] != -1) continue;

        // gets the maximum distance of node u to w, this is so local_dijkstra doesnt look any further
        double max_dist = find_farthest_outgoing_travel_time(adj, ch_g, v, u, u_time);

        // run dijkstra with the bounding box of max_dist
        auto &l_dist = local_dijkstra(ch_g, adj, u, v, max_dist, dist, visited, hops, heap, touched, touched_count);

        // if there is a faster path around the node v add the shortcut u-w to the shortcut count
        for (auto &w_edge : adj[v]) {
            long long w = w_edge.dst_index;
            if (w == -1 || w == v || w == u || ch_g->rank[w] != -1) continue;
            if (l_dist[w] > u_time + edge_travel_time(w_edge)) shortcuts++;
        }
    }

    int edges_removed = (int)(adj_r[v].size() + adj[v].size());

    // returns the difference in shortcuts and edges removed, allowing the node to be ranked
    // a higher number means that removing this node will create heaps of shortcuts compared to how many edges it has meaning that it is a hub of nodes
    return shortcuts - edges_removed;
}

static void ch_contract_node(std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, CHGraph *ch_g, long long v, long long rank, std::vector<double> &dist, std::vector<long long> &visited, std::vector<int> &hops, MinHeap &heap, std::vector<long long> &touched, int &touched_count) {
    for (size_t i = 0; i < adj_r[v].size(); i++) {
        AdjEdge  &u_edge = adj_r[v][i];
        long long u      = u_edge.dst_index;
        double    u_time = edge_travel_time(u_edge);

        if (u == -1 || u == v || ch_g->rank[u] != -1) continue;

        // per-u max_dist: only search as far as needed for the farthest w
        double max_dist = find_farthest_outgoing_travel_time(adj, ch_g, v, u, u_time);

        // run dijkstra with the bounding box of max_dist
        auto &l_dist = local_dijkstra(ch_g, adj, u, v, max_dist, dist, visited, hops, heap, touched, touched_count);

        for (size_t j = 0; j < adj[v].size(); j++) {
            AdjEdge  &w_edge = adj[v][j];
            long long w      = w_edge.dst_index;
            double    w_time = edge_travel_time(w_edge);

            if (w == -1 || w == v || w == u || ch_g->rank[w] != -1) continue;

            // if the new shortcut is faster than what is already there, add the new edge to the adjacency list
            if (l_dist[w] > u_time + w_time) {
                // get the distance to u and w
                double u_dist = u_edge.km_weight;
                double w_dist = w_edge.km_weight;

                // add the new shortcut edge to the adjacency list
                adjlist_add_edge(adj,   u, w, u_time + w_time, u_dist + w_dist, 0, RoadType::Unclassified);
                adjlist_add_edge(adj_r, w, u, u_time + w_time, u_dist + w_dist, 0, RoadType::Unclassified);
            }
        }
    }

    ch_g->rank[v] = rank;
}

CHGraph *ch_build(Graph *g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r) {
    // initialise variables
    clock_t t = clock();

    long long node_count = (long long)g->nodes.size();
    CHGraph *ch_g = ch_init(g);

    std::vector<double>    dist(node_count, DBL_MAX);
    std::vector<long long> visited(node_count, 0);
    std::vector<int>       hops(node_count, INT_MAX);
    std::vector<long long> touched(node_count);
    MinHeap heap, order_heap;
    int touched_count = 0;

    // initial scoring — push onto order_heap (lazy score)
    for (long long i = 0; i < node_count; i++) {
        int score = edge_difference(ch_g, adj, adj_r, i, dist, visited, hops, heap, touched, touched_count);
        order_heap.push({(double)score, i});
    }

    long long rank = 0;

    // run through heap
    while (!order_heap.empty()) {
        auto [score, v] = order_heap.top(); order_heap.pop();

        if (ch_g->rank[v] != -1) continue; // skip if already contracted

        // score the current node and push to heap
        int new_score = edge_difference(ch_g, adj, adj_r, v, dist, visited, hops, heap, touched, touched_count);

        // if the score is worse than the lazy score then push it back to the heap with its new score
        if ((double)new_score > score) {
            order_heap.push({(double)new_score, v});
            continue;
        }

        // if the score is still good or better then contract it
        ch_contract_node(adj, adj_r, ch_g, v, rank, dist, visited, hops, heap, touched, touched_count);
        rank++;
    }

    printf("Time to build contraction hierarchy: %fs\n", (double)(clock() - t) / CLOCKS_PER_SEC);
    return ch_g;
}

/*
Query a contraction hierarchy
*/
ResultPath ch_query(Graph *g, CHGraph *ch_g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, std::unordered_map<long long, long long> &map, long long src_id, long long dst_id) {
    clock_t t = clock();
    ResultPath result;

    // get source index from node id
    if (!map.count(src_id) || !map.count(dst_id)) return result;
    long long src_index = map.at(src_id);

    // get destination index from node id
    long long dst_index = map.at(dst_id);

    long long node_count = (long long)g->nodes.size();

    // allocate dist arrays, set all to DBL_MAX
    std::vector<double>    dist_f(node_count, DBL_MAX), dist_r(node_count, DBL_MAX);
    std::vector<double>    km_f(node_count, DBL_MAX),   km_r(node_count, DBL_MAX);

    // allocate prev arrays, set all to -1
    std::vector<long long> prev_f(node_count, -1), prev_r(node_count, -1);

    // allocate visited arrays, set all to 0
    std::vector<long long> vis_f(node_count, 0), vis_r(node_count, 0);

    // create heap_f and heap_r, push src and dst with distance 0
    MinHeap heap_f, heap_r;

    // push src onto heap_f and dst onto heap_r
    dist_f[src_index] = km_f[src_index] = 0; heap_f.push({0.0, src_index});
    dist_r[dst_index] = km_r[dst_index] = 0; heap_r.push({0.0, dst_index});

    long long best_meeting = -1;
    double    best_cost    = DBL_MAX;

    // alternate between forward and backward search each iteration
    while (!heap_f.empty() && !heap_r.empty()) {
        // expand on the side with the best distance
        bool fwd = heap_f.top().first <= heap_r.top().first;

        auto &dist_c    = fwd ? dist_f : dist_r;
        auto &km_c      = fwd ? km_f   : km_r;
        auto &prev_c    = fwd ? prev_f : prev_r;
        auto &vis_c     = fwd ? vis_f  : vis_r;
        auto &vis_o     = fwd ? vis_r  : vis_f;
        auto &heap_c    = fwd ? heap_f : heap_r;
        auto &neighbors = fwd ? adj    : adj_r;

        long long u = (fwd ? heap_f : heap_r).top().second;
        (fwd ? heap_f : heap_r).pop();

        // if visited[u] skip
        if (vis_c[u]) continue;
        vis_c[u] = 1;

        if (dist_c[u] >= best_cost) break;

        // update best_meeting and best_cost if it is the new best
        if (vis_o[u] && dist_f[u] + dist_r[u] < best_cost) {
            best_meeting = u;
            best_cost    = dist_f[u] + dist_r[u];
        }

        // for all neighbours of u
        for (auto &edge : neighbors[u]) {
            // get neighbour index (v)
            long long v = edge.dst_index;
            if (v == -1) continue;
            if (ch_g->rank[v] <= ch_g->rank[u]) continue;
            if (vis_c[v]) continue;

            // calculate alternative distance = dist[u] + edge weight
            double alt = dist_c[u] + edge_travel_time(edge);

            // if alternative < dist[v]
            if (alt < dist_c[v]) {
                dist_c[v] = alt;
                prev_c[v] = u;
                km_c[v]   = km_c[u] + edge.km_weight;
                heap_c.push({alt, v});
            }
        }
    }

    // check meeting point — same as bidirectional A*
    if (best_meeting == -1) return result;

    // reconstruct path using prev_f and prev_r
    std::vector<long long> path;

    // walk backwards from best_meeting to src
    for (long long cur = best_meeting; cur != -1; cur = prev_f[cur]) path.push_back(cur);

    // reverse so it goes src -> best_meeting
    std::reverse(path.begin(), path.end());

    // walk from best_meeting to dst, skip best_meeting (already in path)
    for (long long cur = prev_r[best_meeting]; cur != -1; cur = prev_r[cur]) path.push_back(cur);

    result.name                 = "CH-Query*";
    result.path_inx             = std::move(path);
    result.time_in_seconds      = best_cost;
    result.distance_in_metres   = km_f[best_meeting] + km_r[best_meeting];
    result.load_time_in_seconds = (double)(clock() - t) / CLOCKS_PER_SEC;

    return result;
}

// saves the contraction hierarchy to path
int ch_save(const char *path, Graph *g, std::vector<AdjList> &adj, std::vector<AdjList> &adj_r, CHGraph *ch_g) {
    FILE *f = fopen(path, "wb");
    if (!f) { printf("failed to open %s for writing (does the folder exist?)\n", path); return 0; }

    long long node_count = (long long)g->nodes.size();
    fwrite(&node_count,       sizeof(long long), 1,          f);
    fwrite(ch_g->rank.data(), sizeof(long long), node_count, f);

    for (long long i = 0; i < node_count; i++) { long long c = adj[i].size(); fwrite(&c, sizeof(long long), 1, f); }
    for (long long i = 0; i < node_count; i++) { fwrite(adj[i].data(), sizeof(AdjEdge), adj[i].size(),   f); }
    for (long long i = 0; i < node_count; i++) { long long c = adj_r[i].size(); fwrite(&c, sizeof(long long), 1, f); }
    for (long long i = 0; i < node_count; i++) { fwrite(adj_r[i].data(), sizeof(AdjEdge), adj_r[i].size(), f); }

    fclose(f);
    return 1;
}

static void fread_adjlist(FILE *f, std::vector<AdjList> &adj, long long node_count) {
    std::vector<long long> counts(node_count);
    for (long long i = 0; i < node_count; i++) fread(&counts[i], sizeof(long long), 1, f);
    for (long long i = 0; i < node_count; i++) {
        adj[i].resize(counts[i]);
        fread(adj[i].data(), sizeof(AdjEdge), counts[i], f);
    }
}

CHGraph *ch_load(const char *path, Graph *g, std::vector<AdjList> &out_adj, std::vector<AdjList> &out_adj_r) {
    FILE *f = fopen(path, "rb");
    if (!f) return nullptr;

    long long cached_count = 0;
    fread(&cached_count, sizeof(long long), 1, f);
    if (cached_count != (long long)g->nodes.size()) {
        printf("contraction cache %s is stale, rebuilding\n", path);
        fclose(f);
        return nullptr;
    }

    CHGraph *ch_g = new CHGraph();
    ch_g->g = g;
    ch_g->rank.resize(cached_count);
    fread(ch_g->rank.data(), sizeof(long long), cached_count, f);

    out_adj.resize(cached_count);
    out_adj_r.resize(cached_count);
    fread_adjlist(f, out_adj,   cached_count);
    fread_adjlist(f, out_adj_r, cached_count);

    fclose(f);
    return ch_g;
}