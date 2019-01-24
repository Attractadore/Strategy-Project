#pragma once

#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

class SNode;

class SNodeGraph {
public:
  SNodeGraph(int width, int height, bool bWrapX, bool bWrapY);
  ~SNodeGraph();

  std::list<std::shared_ptr<SNode>> shortestPath(std::shared_ptr<SNode> src,
                                                 std::shared_ptr<SNode> dst);

  float heuristic(std::shared_ptr<SNode> src, std::shared_ptr<SNode> dst);

  const std::vector<std::shared_ptr<SNode>> &getTiles();

private:
  std::vector<std::shared_ptr<SNode>> tiles;

  std::list<std::shared_ptr<SNode>> adjacentNodes(std::shared_ptr<SNode> src);

  int width;
  int height;
  bool bWrapX;
  bool bWrapY;
};
