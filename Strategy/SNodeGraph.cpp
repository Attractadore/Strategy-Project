#include "SNodeGraph.hpp"

#include "SBuilding.hpp"
#include "SNode.hpp"
#include "SSprite.hpp"
#include "SUnit.hpp"

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include <cmath>

SNodeGraph::SNodeGraph(int p_width, int p_height) {
  m_width = p_width;
  m_height = p_height;

  std::shared_ptr<SSprite> defaultNodeSprite =
      std::make_shared<SSprite>("./assets/art/tile.png", 1, 60, 1);

  SNode defaultNode{};
  defaultNode.setSprite(defaultNodeSprite);
  defaultNode.setMovementCost(1);

  m_tiles = std::vector<std::shared_ptr<SNode>>(m_width * m_height);
  for (int i = 0; i < m_width; i++) {
    for (int j = 0; j < m_height; j++) {
      auto newNode = std::make_shared<SNode>(defaultNode);
      newNode->setPos(i, j);
      m_tiles[i + j * m_width] = newNode;
    }
  }
}

SNodeGraph::~SNodeGraph() {}

std::vector<std::shared_ptr<SNode>>
SNodeGraph::shortestPath(std::shared_ptr<SNode> src,
                         std::shared_ptr<SNode> dst) {

  if (src == dst) {
    return {};
  }

  std::unordered_set<std::shared_ptr<SNode>> closedSet;
  std::unordered_set<std::shared_ptr<SNode>> openSet = {src};

  std::unordered_map<std::shared_ptr<SNode>, std::shared_ptr<SNode>> cameFrom;

  std::unordered_map<std::shared_ptr<SNode>, float> pathCost;
  pathCost[src] = 0.0f;

  std::unordered_map<std::shared_ptr<SNode>, float> passThroughCost;
  passThroughCost[src] = heuristic(src, dst);

  std::vector<std::shared_ptr<SNode>> finalPath;

  while (!openSet.empty()) {
    std::shared_ptr<SNode> currentNode;
    float minScore = std::numeric_limits<float>::infinity();
    for (auto &n : openSet) {
      float nCost = passThroughCost[n];
      if (nCost < minScore) {
        currentNode = n;
        minScore = nCost;
      }
    }

    if (currentNode == dst) {
      finalPath = {currentNode};
      while (cameFrom.find(currentNode) != cameFrom.end()) {
        currentNode = cameFrom[currentNode];
        finalPath.push_back(currentNode);
      }
      break;
    }

    openSet.erase(currentNode);
    closedSet.insert(currentNode);

    std::list<std::shared_ptr<SNode>> adj = adjacentNodes(currentNode);
    for (auto &a : adj) {
      if (closedSet.find(a) != closedSet.end()) {
        continue;
      }
      float cumPathCost = pathCost[currentNode] + 1;
      if (openSet.find(a) == openSet.end()) {
        openSet.insert(a);
        pathCost[a] = std::numeric_limits<float>::infinity();
      }
      if (cumPathCost < pathCost[a]) {
        cameFrom[a] = currentNode;
        pathCost[a] = cumPathCost;
        passThroughCost[a] = cumPathCost + heuristic(a, dst);
      }
    }
  }

  finalPath.pop_back();
  std::reverse(finalPath.begin(), finalPath.end());

  return finalPath;
}

float SNodeGraph::heuristic(std::shared_ptr<SNode> src,
                            std::shared_ptr<SNode> dst) {
  std::pair<int, int> srcPos = src->getPos();
  std::pair<int, int> dstPos = dst->getPos();
  int deltaX = dstPos.first - srcPos.first;
  int deltaY = dstPos.second - srcPos.second;
  return std::sqrt(static_cast<float>(deltaX * deltaX + deltaY * deltaY));
}

std::list<std::shared_ptr<SNode>>
SNodeGraph::adjacentNodes(std::shared_ptr<SNode> src) {
  std::list<std::shared_ptr<SNode>> adj = {};
  std::pair<int, int> srcPos = src->getPos();
  int srcX = srcPos.first;
  int srcY = srcPos.second;
  for (int i = srcX - 1; i <= srcX + 1; i++) {
    for (int j = srcY - 1; j <= srcY + 1; j++) {
      if (!(i == srcX and j == srcY)) {
        bool bjinb = (j >= 0 and j <= m_height - 1);
        bool biinb = (i >= 0 and i <= m_width - 1);
        if (biinb and bjinb) {
          adj.push_back(m_tiles[i + j * m_width]);
        }
      }
    }
  }
  return adj;
}

const std::vector<std::shared_ptr<SNode>> &SNodeGraph::getTiles() {
  return m_tiles;
}

std::shared_ptr<SNode> SNodeGraph::getTileAt(int x, int y) {
  if (x < 0 or x >= m_width or y < 0 or y >= m_height) {
    return nullptr;
  }
  return m_tiles[x + y * m_width];
}
