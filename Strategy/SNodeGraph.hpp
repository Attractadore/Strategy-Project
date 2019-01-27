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

  std::vector<std::shared_ptr<SNode>> shortestPath(std::shared_ptr<SNode> src,
                                                   std::shared_ptr<SNode> dst);

  float heuristic(std::shared_ptr<SNode> src, std::shared_ptr<SNode> dst);

  const std::vector<std::shared_ptr<SNode>> &getTiles();

  std::shared_ptr<SNode> getTileAt(int x, int y);

private:
  std::vector<std::shared_ptr<SNode>> m_tiles;

  std::list<std::shared_ptr<SNode>> adjacentNodes(std::shared_ptr<SNode> src);

  int m_width;
  int m_height;
  bool m_bWrapX;
  bool m_bWrapY;
};
