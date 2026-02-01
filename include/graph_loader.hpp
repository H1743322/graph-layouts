#include "graph.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

inline void loadSotch(Graph& g, const std::string& path) {
    std::clog << "Load SRC\n";
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }

    int numNodes, numEdges;
    file >> numNodes >> numEdges;
    g.nodes.reserve(numNodes);
    g.adj.reserve(numNodes);

    std::string line;
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (line.empty())
            continue;
        std::istringstream ss(line);

        int nodeId;
        ss >> nodeId;

        int weight, neighbor;

        if (!(ss >> weight >> neighbor))
            continue;

        while (ss >> weight >> neighbor) {
            g.addEdge(nodeId, neighbor, weight);
        }
    }
    file.close();
}
inline void loadMtx(Graph& g, const std::string& path) {
    std::clog << "Load MTX\n";

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("failed to open file");
    }
    std::string str;
    while (std::getline(file, str)) {

        if (str.empty())
            continue;
        if (!str.starts_with("%")) {
            break;
        }
    }
    std::istringstream ss(str);
    int rows = 0, cols = 0, nnz;

    ss >> rows >> cols >> nnz;

    g.nodes.reserve(rows);
    g.adj.reserve(cols);

    while (std::getline(file, str)) {
        if (str.empty() || str.starts_with("%")) {
            continue;
        }
        std::istringstream ss(str);
        int r, c;
        double v = 1.0f;
        if (!(ss >> r >> c)) {
            continue;
        }
        g.addEdge(r - 1, c - 1, v);
    }
    file.close();
}

// Build a grid graph
inline void buildGrid(Graph& g, int mx, int my) {
    for (int y = 0; y < my; ++y) {
        for (int x = 0; x < mx; ++x) {
            int node = y * mx + x;
            if (x + 1 < mx)
                g.addEdge(node, node + 1);
            if (y + 1 < my)
                g.addEdge(node, node + mx);
        }
    }
}

void buildSierpinskiRec(Graph& g, int a, int b, int c, int depth, int& nextNodeId);
inline void buildSierpinski(Graph& g, int depth) {
    int node_id = 2;
    buildSierpinskiRec(g, 0, 1, 2, depth, node_id);
}
inline void buildSierpinskiRec(Graph& g, int a, int b, int c, int depth, int& nextNodeId) {
    if (depth == 0) {
        g.addEdge(a, b);
        g.addEdge(b, c);
        g.addEdge(c, a);
        return;
    }

    int ab = ++nextNodeId;
    int bc = ++nextNodeId;
    int ca = ++nextNodeId;

    buildSierpinskiRec(g, a, ab, ca, depth - 1, nextNodeId);
    buildSierpinskiRec(g, ab, b, bc, depth - 1, nextNodeId);
    buildSierpinskiRec(g, ca, bc, c, depth - 1, nextNodeId);
}

inline void buildTorus(Graph& g, int n, int m) {

    for (int i = 0; i < n * m; ++i) {
        g.addNode(i);
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            int current = i * m + j;
            int right = i * m + (j + 1) % m;
            int down = ((i + 1) % n) * m + j;

            g.addEdge(current, right);
            g.addEdge(current, down);
        }
    }
}
