#pragma once
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <vector>

struct Edge;
struct Node {
    int id;
    float x = 0, y = 0;
    float dx = 0, dy = 0;
};

struct NodeAdj {
    int dst;
    float weight = 1.0f;
};

class Graph {

  public:
    std::vector<Node> nodes;
    std::vector<std::vector<NodeAdj>> adj;
    std::unordered_map<int, size_t> idToIndex;
    bool directed = false;

    Graph(bool directed = false) : directed(directed) {}

    size_t getEdgeCount();
    size_t addNode(const int id);
    void addEdge(const int src, const int dest, float weight = 1.0f);
    std::vector<int> getNeighbords(const int n);
    void print();
    void randomizePos(float w, float h);
    void gridLayout(float width, float height, int cols = 0);
    void resetForces();

    void dijkstra(int src, std::vector<float>& dist);
    std::vector<std::vector<float>> computeAllPairsShortestPaths();
    void clear() {
        nodes.clear();
        adj.clear();
        idToIndex.clear();
    }
};
