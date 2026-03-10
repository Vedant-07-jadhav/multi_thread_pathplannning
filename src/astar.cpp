#include "astar.h"
#include <algorithm>
#include <cmath>

AStar::AStar(const std::vector<std::vector<int>> &inputGrid) {
  height = inputGrid.size();
  width = inputGrid[0].size();

  grid.resize(width * height);

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++)
      grid[index(x, y)] = inputGrid[y][x];
}

int AStar::heuristic(int x1, int y1, int x2, int y2) const {
  return std::abs(x1 - x2) + std::abs(y1 - y2);
}

std::vector<Node> AStar::reconstructPath(const Node &start, const Node &goal,
                                         Workspace &ws) {
  std::vector<Node> path;

  Node current = goal;

  while (!(current == start)) {
    path.push_back(current);
    current = ws.parent[index(current.x, current.y)];
  }

  path.push_back(start);

  std::reverse(path.begin(), path.end());

  return path;
}

Result AStar::solve(const Node &start, const Node &goal, Workspace &ws) {
  ws.current_search_id++;

  int nodesExpanded = 0;

  std::priority_queue<PQItem, std::vector<PQItem>, Compare> open;

  int start_idx = index(start.x, start.y);

  ws.visit_id[start_idx] = ws.current_search_id;
  ws.g_cost[start_idx] = 0;
  ws.parent[start_idx] = start;

  open.push({start.x, start.y, heuristic(start.x, start.y, goal.x, goal.y)});

  int dx[4] = {1, -1, 0, 0};
  int dy[4] = {0, 0, 1, -1};

  while (!open.empty()) {
    PQItem top = open.top();
    open.pop();

    int x = top.x;
    int y = top.y;

    nodesExpanded++;

    if (x == goal.x && y == goal.y) {
      Result r;
      r.path = reconstructPath(start, goal, ws);
      r.nodesExpanded = nodesExpanded;
      return r;
    }

    int current_idx = index(x, y);

    for (int i = 0; i < 4; i++) {
      int nx = x + dx[i];
      int ny = y + dy[i];

      if (nx < 0 || nx >= width || ny < 0 || ny >= height)
        continue;

      int nidx = index(nx, ny);

      if (grid[nidx] == 1)
        continue;

      int tentative_g = ws.g_cost[current_idx] + 1;

      if (ws.visit_id[nidx] != ws.current_search_id ||
          tentative_g < ws.g_cost[nidx]) {
        ws.visit_id[nidx] = ws.current_search_id;

        ws.g_cost[nidx] = tentative_g;

        ws.parent[nidx] = {x, y};

        int f = tentative_g + heuristic(nx, ny, goal.x, goal.y);

        open.push({nx, ny, f});
      }
    }
  }

  Result r;
  r.nodesExpanded = nodesExpanded;
  return r;
}