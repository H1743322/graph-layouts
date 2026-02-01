#include "graph.hpp"
#include <iostream>
#include <queue>
#include <random>
#include <vector>

size_t Graph::addNode(int id) {
    auto it = idToIndex.find(id);
    if (it != idToIndex.end())
        return it->second;

    size_t index = nodes.size();
    nodes.emplace_back(Node{id});

    adj.emplace_back();
    idToIndex[id] = index;

    return index;
}

void Graph::addEdge(int src, int dest, float weight) {
    int src_idx = addNode(src);
    int dest_idx = addNode(dest);

    adj[src_idx].emplace_back(NodeAdj{dest_idx, weight});
    if (!directed) {
        adj[dest_idx].emplace_back(NodeAdj{src_idx, weight});
    }
}

std::vector<int> Graph::getNeighbords(const int n) {
    std::vector<int> neighbord;
    int idx = idToIndex.at(n);
    for (const auto& n_i : adj[idx]) {
        int node = nodes[n_i.dst].id;
        neighbord.push_back(node);
    }
    return neighbord;
}
size_t Graph::getEdgeCount() {
    size_t count = 0;
    for (const auto& n : adj) {
        count += n.size();
    }
    return count;
}

void Graph::print() {
    std::cout << "Graph Summary: " << nodes.size() << " nodes, " << getEdgeCount() << " edges\n\n";

    for (size_t i = 0; i < nodes.size(); ++i) {
        const Node& n = nodes[i];
        std::cout << "Node " << n.id << " ( " << n.x << ", " << n.y << ") -> ";

        if (adj[i].empty()) {
            std::cout << "(none)";
        } else {
            for (const auto& n_a : adj[i]) {
                auto& dst = nodes[n_a.dst];
                std::cout << dst.id << " ";
            }
        }

        std::cout << "\n";
    }

    std::cout << std::endl;
}

// FIXME: move to layout
void Graph::randomizePos(float w, float h) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_x(-w / 2.0f, w / 2.0f);
    std::uniform_real_distribution<float> dist_y(-h / 2.0f, h / 2.0f);
    for (auto& n : nodes) {
        n.x = dist_x(gen);
        n.y = dist_y(gen);
    }
}

// FIXME: move to layout
void Graph::gridLayout(float width, float height, int cols) {
    int N = nodes.size();
    if (N == 0)
        return;

    if (cols <= 0) {
        cols = static_cast<int>(std::ceil(std::sqrt(N)));
    }
    int rows = static_cast<int>(std::ceil(static_cast<float>(N) / cols));

    float cell_width = width / cols;
    float cell_height = height / rows;

    for (int i = 0; i < N; ++i) {
        int r = i / cols;
        int c = i % cols;

        nodes[i].x = c * cell_width - width / 2 + cell_width / 2;
        nodes[i].y = r * cell_height - height / 2 + cell_height / 2;
    }
}
void Graph::resetForces() {
    for (auto& n : nodes) {
        n.dx = 0.0f;
        n.dy = 0.0f;
    }
}

void Graph::dijkstra(int src, std::vector<float>& dist) {
    std::fill(dist.begin(), dist.end(), std::numeric_limits<float>::infinity());
    dist[src] = 0.0f;

    using PQItem = std::pair<float, size_t>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<>> pq;
    pq.push({0.0f, src});

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u])
            continue;

        for (const auto& neighbor : adj[u]) {
            size_t v = neighbor.dst;
            float alt = dist[u] + neighbor.weight;

            if (alt < dist[v]) {
                dist[v] = alt;
                pq.push({alt, v});
            }
        }
    }
}

std::vector<std::vector<float>> Graph::computeAllPairsShortestPaths() {
    size_t V = nodes.size();
    std::vector<std::vector<float>> allPairs(V,
                                             std::vector<float>(V, std::numeric_limits<float>::infinity()));

    std::vector<float> dist(V);
    dist.resize(V);
    for (size_t i = 0; i < V; ++i) {
        dijkstra(i, dist);
        std::copy(dist.begin(), dist.end(), allPairs[i].begin());
    }

    return allPairs;
}
