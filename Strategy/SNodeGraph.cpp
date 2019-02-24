#include "SNodeGraph.hpp"

#include "SBuilding.hpp"
#include "SNode.hpp"
#include "SSprite.hpp"
#include "SUnit.hpp"

#include <algorithm>
#include <iostream>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include <cmath>

template <typename T>
T lerp(T v, T vmin, T vmax, T tmin, T tmax)
{
  //  std::cout << "v " << v << " vmin " << vmin << " vmax " << vmax << " tmin " << tmin << " tmax " << tmax << std::endl;
  //  std::cout << "v " << v << " (v - vmin)/(vmax - vmin) " << ((v - vmin) / (vmax - vmin)) << " (tmax - tmin) " << (tmax - tmin) << std::endl;
  return tmin + ((v - vmin) / (vmax - vmin)) * (tmax - tmin);
}

SNodeGraph::SNodeGraph(int p_width, int p_height)
{
  m_width = p_width;
  m_height = p_height;

  generateMap();
}

void SNodeGraph::generateMap()
{

  std::shared_ptr<SSprite> grasslandSprite = std::make_shared<SSprite>("./assets/art/grassland.png", 1, 60, 1);
  std::shared_ptr<SSprite> hillSprite = std::make_shared<SSprite>("./assets/art/hill.png", 1, 60, 1);
  std::shared_ptr<SSprite> mountainSprite = std::make_shared<SSprite>("./assets/art/mountain.png", 1, 60, 1);
  std::shared_ptr<SSprite> waterSprite = std::make_shared<SSprite>("./assets/art/water.png", 1, 60, 1);
  std::shared_ptr<SSprite> oceanSprite = std::make_shared<SSprite>("./assets/art/ocean.png", 1, 60, 1);

  SNode grassland{};
  grassland.setSprite(grasslandSprite);
  grassland.m_movementCost = 1;
  grassland.m_armorModifier = 0;
  grassland.m_accuracyModifier = 0;
  grassland.bPassable = true;

  SNode hill{};
  hill.setSprite(hillSprite);
  hill.m_movementCost = 2;
  hill.m_armorModifier = 1;
  hill.m_accuracyModifier = -0.1f;
  hill.bPassable = true;

  SNode mountain{};
  mountain.setSprite(mountainSprite);
  mountain.m_movementCost = 0;
  mountain.m_armorModifier = 0;
  mountain.m_accuracyModifier = 0;
  mountain.bPassable = false;

  SNode water{};
  water.setSprite(waterSprite);
  water.m_movementCost = 3;
  water.m_armorModifier = -1;
  water.m_accuracyModifier = 0;
  water.bPassable = true;

  SNode ocean{};
  ocean.setSprite(oceanSprite);
  ocean.m_movementCost = 0;
  ocean.m_armorModifier = 0;
  ocean.m_accuracyModifier = 0;
  ocean.bPassable = false;

  int cellSize = 4;

  int fw = m_width / cellSize;
  int fh = m_height / cellSize;

  std::vector<float> firstDist(fw * fh);
  float c_x = (fw - 1) / 2.0f;
  float c_y = (fh - 1) / 2.0f;
  float c_d = std::sqrt(c_x * c_x + c_y * c_y);
  for (int i = 0; i < fw; i++)
  {
    for (int j = 0; j < fh; j++)
    {
      float dx = c_x - i;
      float dy = c_y - j;
      float d = std::sqrt(dx * dx + dy * dy);
      firstDist[i + j * fw] = 1 - lerp(d, 0.0f, c_d, 0.0f, 1.0f);
      //      std::cout << firstDist[i + j * fw] << " ";
    }
    //    std::cout << std::endl;
  }

  //  for (int y = 0; y < fh; y++)
  //  {
  //    for (int x = 0; x < fw; x++)
  //    {
  //      std::cout << firstDist[x + y * fw] << " ";
  //    }
  //    std::cout << std::endl;
  //  }

  float maxDelta = 0.3f;
  std::uniform_real_distribution<float> deltaDist(-maxDelta, maxDelta);
  std::mt19937 gen;
  gen.seed(std::random_device()());

  std::vector<float> secondDist(m_width * m_height);

  for (int i = 0; i < fw; i++)
  {
    for (int j = 0; j < fh; j++)
    {
      float s = firstDist[i + j * fw];
      for (int x = 0; x < cellSize; x++)
      {
        for (int y = 0; y < cellSize; y++)
        {
          //          std::cout << (i * cellSize + x) << ", " << ((j)*cellSize + y) << std::endl;
          secondDist[i * cellSize + x + (j * cellSize + y) * m_width] = s + deltaDist(gen);
        }
      }
    }
  }

  //  for (int y = 0; y < m_height; y++)
  //  {
  //    for (int x = 0; x < m_width; x++)
  //    {
  //      std::cout << secondDist[x + y * m_width] << " ";
  //    }
  //    std::cout << std::endl;
  //  }

  m_tiles = std::vector<std::shared_ptr<SNode>>(m_width * m_height);
  for (int i = 0; i < m_width; i++)
  {
    for (int j = 0; j < m_height; j++)
    {
      float v = secondDist[i + j * m_width];
      SNode* tileType;
      if (v <= 0.2f)
      {
        tileType = &ocean;
      }
      else if (v <= 0.3f)
      {
        tileType = &water;
      }
      else if (v <= 0.75f)
      {
        tileType = &grassland;
      }
      else if (v <= 0.95f)
      {
        tileType = &hill;
      }
      else
      {
        tileType = &mountain;
      }

      auto newNode = std::make_shared<SNode>(*tileType);
      newNode->setPos(i, j);
      m_tiles[i + j * m_width] = newNode;
    }
  }
}

SNodeGraph::~SNodeGraph()
{
}

std::vector<std::shared_ptr<SNode>>
SNodeGraph::shortestPath(std::shared_ptr<SNode> src,
                         std::shared_ptr<SNode> dst)
{

  if (src == dst)
  {
    return {};
  }

  std::unordered_set<std::shared_ptr<SNode>> closedSet;
  std::unordered_set<std::shared_ptr<SNode>> openSet = { src };

  std::unordered_map<std::shared_ptr<SNode>, std::shared_ptr<SNode>> cameFrom;

  std::unordered_map<std::shared_ptr<SNode>, float> pathCost;
  pathCost[src] = 0.0f;

  std::unordered_map<std::shared_ptr<SNode>, float> passThroughCost;
  passThroughCost[src] = heuristic(src, dst);

  std::vector<std::shared_ptr<SNode>> finalPath;

  while (!openSet.empty())
  {
    std::shared_ptr<SNode> currentNode;
    float minScore = std::numeric_limits<float>::infinity();
    for (auto& n : openSet)
    {
      float nCost = passThroughCost[n];
      if (nCost < minScore)
      {
        currentNode = n;
        minScore = nCost;
      }
    }

    if (currentNode == dst)
    {
      finalPath = { currentNode };
      while (cameFrom.find(currentNode) != cameFrom.end())
      {
        currentNode = cameFrom[currentNode];
        finalPath.push_back(currentNode);
      }
      break;
    }

    openSet.erase(currentNode);
    closedSet.insert(currentNode);

    std::list<std::shared_ptr<SNode>> adj = adjacentNodes(currentNode);
    for (auto& a : adj)
    {
      if (closedSet.find(a) != closedSet.end())
      {
        continue;
      }
      float cumPathCost = pathCost[currentNode] + 1;
      if (openSet.find(a) == openSet.end())
      {
        openSet.insert(a);
        pathCost[a] = std::numeric_limits<float>::infinity();
      }
      if (cumPathCost < pathCost[a])
      {
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
                            std::shared_ptr<SNode> dst)
{
  std::pair<int, int> srcPos = src->getPos();
  std::pair<int, int> dstPos = dst->getPos();
  int deltaX = dstPos.first - srcPos.first;
  int deltaY = dstPos.second - srcPos.second;
  return std::sqrt(static_cast<float>(deltaX * deltaX + deltaY * deltaY));
}

std::list<std::shared_ptr<SNode>>
SNodeGraph::adjacentNodes(std::shared_ptr<SNode> src)
{
  std::list<std::shared_ptr<SNode>> adj = {};
  std::pair<int, int> srcPos = src->getPos();
  int srcX = srcPos.first;
  int srcY = srcPos.second;
  for (int i = srcX - 1; i <= srcX + 1; i++)
  {
    for (int j = srcY - 1; j <= srcY + 1; j++)
    {
      if (!(i == srcX and j == srcY))
      {
        bool bjinb = (j >= 0 and j <= m_height - 1);
        bool biinb = (i >= 0 and i <= m_width - 1);
        if (biinb and bjinb)
        {
          adj.push_back(m_tiles[i + j * m_width]);
        }
      }
    }
  }
  return adj;
}

const std::vector<std::shared_ptr<SNode>>& SNodeGraph::getTiles()
{
  return m_tiles;
}

std::shared_ptr<SNode> SNodeGraph::getTileAt(int x, int y)
{
  if (x < 0 or x >= m_width or y < 0 or y >= m_height)
  {
    return nullptr;
  }
  return m_tiles[x + y * m_width];
}
