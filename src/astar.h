#ifndef ASTAR_H
#define ASTAR_H

#include <queue>
#include <vector>

struct Node {
  int x;
  int y;

  bool operator==(const Node &other) const {
    return x == other.x && y == other.y;
  }
};

struct Result {
  std::vector<Node> path;
  int nodesExpanded = 0;
};

struct Workspace {
  std::vector<int> g_cost;
  std::vector<int> visit_id;
  std::vector<Node> parent;

  int current_search_id = 1;

  Workspace(int size) {
    g_cost.resize(size);
    visit_id.resize(size, 0);
    parent.resize(size);
  }
};

class AStar {
private:
  int width;
  int height;

  std::vector<int> grid;

  struct PQItem {
    int x;
    int y;
    int f;
  };

  struct Compare {
    bool operator()(const PQItem &a, const PQItem &b) const {
      return a.f > b.f;
    }
  };

  inline int index(int x, int y) const { return y * width + x; }

  int heuristic(int x1, int y1, int x2, int y2) const;

  std::vector<Node> reconstructPath(const Node &start, const Node &goal,
                                    Workspace &ws);

public:
  AStar(const std::vector<std::vector<int>> &grid);

  Result solve(const Node &start, const Node &goal, Workspace &ws);
};

#endif