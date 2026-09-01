#include <algorithm>
#include <cstddef>
#include <map>
#include <vector>

using Graph = std::map<int, std::vector<int>>;

std::vector<int> shortestPath(const Graph &graph, int start, int goal) {
  std::map<int, int> parent = {{start, start}};
  std::vector<int> frontier = {start};

  for (std::size_t cursor = 0; cursor < frontier.size(); ++cursor) {
    const int current = frontier[cursor];
    if (current == goal) {
      break;
    }

    const auto neighbors = graph.find(current);
    if (neighbors == graph.end()) {
      continue;
    }

    for (const int neighbor : neighbors->second) {
      if (parent.find(neighbor) != parent.end()) {
        continue;
      }

      parent[neighbor] = current;
      frontier.push_back(neighbor);
    }
  }

  if (parent.find(goal) == parent.end()) {
    return {};
  }

  std::vector<int> path;
  for (int node = goal;; node = parent[node]) {
    path.push_back(node);
    if (node == start) {
      break;
    }
  }

  std::reverse(path.begin(), path.end());
  return path;
}

int main() {
  const Graph transitNetwork = {{1, {2, 3}}, {2, {4}}, {3, {4, 5}},
                                {4, {6}},    {5, {6}}, {6, {}}};

  const std::vector<int> route = shortestPath(transitNetwork, 1, 6);
  const std::vector<int> expectedRoute = {1, 2, 4, 6};
  return route == expectedRoute ? 0 : 1;
}
