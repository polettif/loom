// Copyright 2017, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include "octi/gridgraph/GridGraph.h"
#include "util/graph/Node.h"

using namespace octi::gridgraph;

// _____________________________________________________________________________
GridGraph::GridGraph(const util::geo::Box& bbox, double cellSize)
    : _bbox(bbox),
      _grid(cellSize, cellSize, bbox),
      Graph<NodePL, EdgePL>(true) {
  _c = {
      0,   // cost for 0 degree node traversal
      90,  // cost for 45 degree node traversal
      30,  // cost for 90 degree node traversal
      10,  // cost for 135 degree node traversal
      3,   // cost for vertical edge
      3,   // cost for horizontal edge
      3    // cost for diagonal edge
  };

  assert(_c.p_0 < _c.p_135);
  assert(_c.p_135 < _c.p_90);
  assert(_c.p_90 < _c.p_45);

  double c_0 = _c.p_45 - _c.p_135;
  double c_135 = _c.p_45;
  double c_90 = _c.p_45 - _c.p_135 + _c.p_90;

  double directNodeCost = 88888;

  // write nodes
  for (size_t x = 0; x < _grid.getXWidth(); x++) {
    for (size_t y = 0; y < _grid.getYHeight(); y++) {
      double xPos = bbox.min_corner().get<0>() + x * cellSize;
      double yPos = bbox.min_corner().get<1>() + y * cellSize;
      Node<NodePL, EdgePL>* n =
          new Node<NodePL, EdgePL>(NodePL(Point(xPos, yPos)));
      _grid.add(x, y, n);
      addNode(n);
      n->pl().setParent(n);

      Node<NodePL, EdgePL>* n1 =
          new Node<NodePL, EdgePL>(NodePL(Point(xPos - cellSize / 10.0, yPos)));
      n1->pl().setParent(n);
      addNode(n1);
      n->pl().setWestPort(n1);
      addEdge(n, n1, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n1->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n1, n, EdgePL(util::geo::PolyLine(*n1->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n2 = new Node<NodePL, EdgePL>(
          NodePL(Point(xPos - cellSize / 10.0, yPos - cellSize / 10.0)));
      addNode(n2);
      n2->pl().setParent(n);
      n->pl().setSouthWestPort(n2);
      addEdge(n, n2, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n2->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n2, n, EdgePL(util::geo::PolyLine(*n2->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n3 =
          new Node<NodePL, EdgePL>(NodePL(Point(xPos, yPos - cellSize / 10.0)));
      addNode(n3);
      n3->pl().setParent(n);
      n->pl().setSouthPort(n3);
      addEdge(n, n3, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n3->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n3, n, EdgePL(util::geo::PolyLine(*n3->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n4 =
          new Node<NodePL, EdgePL>(NodePL(Point(xPos + cellSize / 10.0, yPos)));
      addNode(n4);
      n4->pl().setParent(n);
      n->pl().setEastPort(n4);
      addEdge(n, n4, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n4->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n4, n, EdgePL(util::geo::PolyLine(*n4->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n5 = new Node<NodePL, EdgePL>(
          NodePL(Point(xPos + cellSize / 10.0, yPos + cellSize / 10.0)));
      n5->pl().setParent(n);
      addNode(n5);
      n->pl().setNorthEastPort(n5);
      addEdge(n, n5, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n5->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n5, n, EdgePL(util::geo::PolyLine(*n5->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n6 =
          new Node<NodePL, EdgePL>(NodePL(Point(xPos, yPos + cellSize / 10.0)));
      n6->pl().setParent(n);
      addNode(n6);
      n->pl().setNorthPort(n6);
      addEdge(n, n6, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n6->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n6, n, EdgePL(util::geo::PolyLine(*n6->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      Node<NodePL, EdgePL>* n7 = new Node<NodePL, EdgePL>(
          NodePL(Point(xPos - cellSize / 10.0, yPos + cellSize / 10.0)));
      n7->pl().setParent(n);
      addNode(n7);
      n->pl().setNorthWestPort(n7);
      addEdge(n, n7, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n7->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n7, n, EdgePL(util::geo::PolyLine(*n7->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));
      Node<NodePL, EdgePL>* n8 = new Node<NodePL, EdgePL>(
          NodePL(Point(xPos + cellSize / 10.0, yPos - cellSize / 10.0)));
      n8->pl().setParent(n);
      addNode(n8);
      n->pl().setSouthEastPort(n8);
      addEdge(n, n8, EdgePL(util::geo::PolyLine(*n->pl().getGeom(),
                                                *n8->pl().getGeom()),
                            directNodeCost, true));
      addEdge(n8, n, EdgePL(util::geo::PolyLine(*n8->pl().getGeom(),
                                                *n->pl().getGeom()),
                            directNodeCost, true));

      // in-node connections
      for (size_t i = 0; i < 8; i++) {
        for (size_t j = i + 1; j < 8; j++) {
          int d = (int)(i) - (int)(j);
          size_t deg = abs((((d + 4) % 8) + 8) % 8 - 4);
          double pen = c_0;

          if (deg == 1) continue;
          if (deg == 2) pen = c_90;
          if (deg == 3) pen = c_135;
          addEdge(
              n->pl().getPort(i), n->pl().getPort(j),
              EdgePL(util::geo::PolyLine(*n->pl().getPort(i)->pl().getGeom(),
                                         *n->pl().getPort(j)->pl().getGeom()),
                     pen, true));
          addEdge(
              n->pl().getPort(j), n->pl().getPort(i),
              EdgePL(util::geo::PolyLine(*n->pl().getPort(j)->pl().getGeom(),
                                         *n->pl().getPort(i)->pl().getGeom()),
                     pen, true));
        }
      }
    }
  }

  for (size_t x = 0; x < _grid.getXWidth(); x++) {
    for (size_t y = 0; y < _grid.getYHeight(); y++) {
      Node<NodePL, EdgePL>* center = getNode(x, y);
      if (center == 0) continue;

      Node<NodePL, EdgePL>* from = center->pl().getNorthPort();
      Node<NodePL, EdgePL>* toN = getNeighbor(x, y, 0);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getSouthPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getSouthPort();
      toN = getNeighbor(x, y, 4);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getNorthPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getWestPort();
      toN = getNeighbor(x, y, 6);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getEastPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getEastPort();
      toN = getNeighbor(x, y, 2);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getWestPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getSouthWestPort();
      toN = getNeighbor(x, y, 5);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getNorthEastPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getSouthEastPort();
      toN = getNeighbor(x, y, 3);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getNorthWestPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getNorthWestPort();
      toN = getNeighbor(x, y, 7);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getSouthEastPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }

      from = center->pl().getNorthEastPort();
      toN = getNeighbor(x, y, 1);
      if (from != 0 && toN != 0) {
        Node<NodePL, EdgePL>* to = toN->pl().getSouthWestPort();
        addEdge(from, to, EdgePL(util::geo::PolyLine(*from->pl().getGeom(),
                                                     *to->pl().getGeom()),
                                 0, false));
      }
    }
  }

  writeInitialCosts();
}

// _____________________________________________________________________________
Node<NodePL, EdgePL>* GridGraph::getNode(size_t x, size_t y) const {
  if (x >= _grid.getXWidth() || y >= _grid.getYHeight()) return 0;
  std::set<Node<NodePL, EdgePL>*> r;
  _grid.get(x, y, &r);

  if (r.size()) return *r.begin();
  return 0;
}

// _____________________________________________________________________________
std::pair<size_t, size_t> GridGraph::getNodeCoords(GridNode* n) const {
  auto r = _grid.getCells(n);
  assert(r.size() == 1);
  return *r.begin();
}

// _____________________________________________________________________________
Node<NodePL, EdgePL>* GridGraph::getNeighbor(size_t cx, size_t cy,
                                             size_t i) const {
  int8_t x = 1;
  if (i % 4 == 0) x = 0;
  if (i > 4) x = -1;

  int8_t y = 1;
  if (i == 2 || i == 6) y = 0;
  if (i == 3 || i == 4 || i == 5) y = -1;

  return getNode(cx + x, cy + y);
}

// _____________________________________________________________________________
void GridGraph::balanceNode(GridNode* n) {
  auto xy = getNodeCoords(n);
  size_t x = xy.first;
  size_t y = xy.second;

  for (size_t i = 0; i < 8; i++) {
    if (i == 1 || i == 3) {
      auto a = getNeighbor(x, y, (i + 7) % 8);
      auto b = getNeighbor(x, y, (i + 1) % 8);

      if (a && b) {
        size_t port = (i + 2) % 8;
        auto e =
            getEdge(a->pl().getPort(port), b->pl().getPort((port + 4) % 8));
        auto f =
            getEdge(b->pl().getPort((port + 4) % 8), a->pl().getPort(port));

        e->pl().setCost(e->pl().cost() + 99999);
        f->pl().setCost(f->pl().cost() + 99999);
      }
    }
  }
}

// _____________________________________________________________________________
void GridGraph::balanceEdge(GridNode* a, GridNode* b) {
  if (a == b) return;
  size_t dir = 0;
  for (; dir < 8; dir++) {
    if (getEdge(a->pl().getPort(dir), b->pl().getPort((dir + 4) % 8))) {
      break;
    }
  }

  auto xy = getNodeCoords(a);
  size_t x = xy.first;
  size_t y = xy.second;

  auto xyb = getNodeCoords(b);
  size_t xb = xyb.first;
  size_t yb = xyb.second;

  for (size_t i = 0; i < 8; i++) {
    auto e = getNEdge(a, getNeighbor(x, y, i));
    if (e) {
      e->pl().setCost(e->pl().cost() + 99999);
      getOtherEdge(e)->pl().setCost(getOtherEdge(e)->pl().cost() + 99999);
    }

    auto f = getNEdge(b, getNeighbor(xb, yb, i));
    if (f) {
      f->pl().setCost(f->pl().cost() + 99999);
      getOtherEdge(f)->pl().setCost(getOtherEdge(f)->pl().cost() + 99999);
    }
  }

  if (dir == 1 || dir == 3 || dir == 5 || dir == 7) {
    auto na = getNeighbor(x, y, (dir + 7) % 8);
    auto nb = getNeighbor(x, y, (dir + 1) % 8);

    if (na && nb) {
      auto e =
          getNEdge(na, nb);
      auto f =
          getNEdge(nb, na);

      e->pl().setCost(e->pl().cost() + 99999);
      f->pl().setCost(f->pl().cost() + 99999);
    }
  }

  return;

  double nearPenalty = 100;

  std::cerr << x << ", " << y << std::endl;

  if (dir == 0) {
    auto a = getNEdge(getNode(x - 1, y), getNode(x - 1, y + 1));
    auto b = getNEdge(getNode(x + 1, y), getNode(x + 1, y + 1));

    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() + nearPenalty);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() + nearPenalty);
    }
  } else if (dir == 4) {
    auto a = getNEdge(getNode(x - 1, y), getNode(x - 1, y - 1));
    auto b = getNEdge(getNode(x + 1, y), getNode(x + 1, y - 1));

    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() + nearPenalty);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() + nearPenalty);
    }
  }
  if (dir == 2) {
    auto a = getNEdge(getNode(x, y + 1), getNode(x + 1, y + 1));
    auto b = getNEdge(getNode(x, y - 1), getNode(x + 1, y - 1));

    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() + nearPenalty);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() + nearPenalty);
    }
  } else if (dir == 6) {
    auto a = getNEdge(getNode(x, y + 1), getNode(x - 1, y + 1));
    auto b = getNEdge(getNode(x, y - 1), getNode(x - 1, y - 1));

    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() + nearPenalty);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() + nearPenalty);
    }

    // diagonal
  } else if (dir == 1) {
    auto a = getNEdge(getNode(x - 1, y), getNode(x, y + 1));
    auto b = getNEdge(getNode(x, y + 1), getNode(x + 1, y + 2));
    auto c = getNEdge(getNode(x, y - 1), getNode(x + 1, y));
    auto d = getNEdge(getNode(x + 1, y), getNode(x + 2, y + 1));
    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty / 2);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty / 2);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (c) {
      c->pl().setCost(c->pl().cost() + nearPenalty / 2);
      getOtherEdge(c)->pl().setCost(getOtherEdge(c)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (d) {
      d->pl().setCost(d->pl().cost() + nearPenalty / 2);
      getOtherEdge(d)->pl().setCost(getOtherEdge(d)->pl().cost() +
                                    nearPenalty / 2);
    }
  } else if (dir == 5) {
    auto a = getNEdge(getNode(x - 1, y), getNode(x, y + 1));
    auto b = getNEdge(getNode(x - 2, y - 1), getNode(x - 1, y));
    auto c = getNEdge(getNode(x, y - 1), getNode(x + 1, y));
    auto d = getNEdge(getNode(x - 1, y - 2), getNode(x, y - 1));
    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty / 2);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty / 2);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (c) {
      c->pl().setCost(c->pl().cost() + nearPenalty / 2);
      getOtherEdge(c)->pl().setCost(getOtherEdge(c)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (d) {
      d->pl().setCost(d->pl().cost() + nearPenalty / 2);
      getOtherEdge(d)->pl().setCost(getOtherEdge(d)->pl().cost() +
                                    nearPenalty / 2);
    }
  } else if (dir == 3) {
    auto a = getNEdge(getNode(x - 1, y), getNode(x, y - 1));
    auto b = getNEdge(getNode(x, y - 1), getNode(x + 1, y - 2));
    auto c = getNEdge(getNode(x, y + 1), getNode(x + 1, y));
    auto d = getNEdge(getNode(x + 1, y), getNode(x + 2, y - 1));
    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty / 2);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty / 2);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (c) {
      c->pl().setCost(c->pl().cost() + nearPenalty / 2);
      getOtherEdge(c)->pl().setCost(getOtherEdge(c)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (d) {
      d->pl().setCost(d->pl().cost() + nearPenalty / 2);
      getOtherEdge(d)->pl().setCost(getOtherEdge(d)->pl().cost() +
                                    nearPenalty / 2);
    }
  } else if (dir == 7) {
    auto a = getNEdge(getNode(x, y - 1), getNode(x - 1, y));
    auto b = getNEdge(getNode(x - 1, y), getNode(x - 2, y + 1));
    auto c = getNEdge(getNode(x + 1, y), getNode(x, y + 1));
    auto d = getNEdge(getNode(x, y + 1), getNode(x - 1, y + 2));
    if (a) {
      a->pl().setCost(a->pl().cost() + nearPenalty / 2);
      getOtherEdge(a)->pl().setCost(getOtherEdge(a)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (b) {
      b->pl().setCost(b->pl().cost() + nearPenalty / 2);
      getOtherEdge(b)->pl().setCost(getOtherEdge(b)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (c) {
      c->pl().setCost(c->pl().cost() + nearPenalty / 2);
      getOtherEdge(c)->pl().setCost(getOtherEdge(c)->pl().cost() +
                                    nearPenalty / 2);
    }
    if (d) {
      d->pl().setCost(d->pl().cost() + nearPenalty / 2);
      getOtherEdge(d)->pl().setCost(getOtherEdge(d)->pl().cost() +
                                    nearPenalty / 2);
    }
  }

  std::cerr << dir << std::endl;
}

// _____________________________________________________________________________
GridEdge* GridGraph::getOtherEdge(GridEdge* e) {
  return getEdge(e->getTo(), e->getFrom());
}

// _____________________________________________________________________________
GridEdge* GridGraph::getNEdge(GridNode* a, GridNode* b) {
  for (size_t i = 0; i < 8; i++) {
    if (a->pl().getPort(i) && b->pl().getPort((i + 4) % 8)) {
      auto e = getEdge(a->pl().getPort(i), b->pl().getPort((i + 4) % 8));
      if (e) return e;
    }
  }

  return 0;
}

// _____________________________________________________________________________
void GridGraph::topoPenalty(GridNode* n, CombNode* origNode, CombEdge* e) {
  auto xy = getNodeCoords(n);
  size_t x = xy.first;
  size_t y = xy.second;
  assert(getNode(x, y) == n);

  if (origNode->pl().getParent()->pl().getStops().size())
    std::cerr << "Checking station "
              << origNode->pl().getParent()->pl().getStops().front().name
              << std::endl;

  size_t origEdgeNumber = origNode->getAdjList().size();
  assert(origNode->pl().getOrderedEdges().size() == origEdgeNumber);
  size_t optimDistance = (8 / origEdgeNumber) - 1;

  if (!origNode->pl().hasOrderedEdge(e)) {
    std::cerr << "Warning: tried to balance edge " << e << " in node "
              << origNode << ", but the edge does not appear there."
              << std::endl;
    return;
  }

  std::cerr << std::endl;
  std::cerr << "Orig edge number = " << origEdgeNumber << ", optim distance is "
            << optimDistance << std::endl;

  CombEdge* outgoing[8];

  bool found = false;

  // if some outgoing edge is taken, dont put new edge next to it
  for (size_t i = 0; i < 8; i++) {
    auto port = n->pl().getPort(i);
    auto neigh = getNeighbor(x, y, i);

    if (neigh &&
        getEdge(port, neigh->pl().getPort((i + 4) % 8))
                ->pl()
                .getResEdges()
                .size() > 0) {
      outgoing[i] = *getEdge(port, neigh->pl().getPort((i + 4) % 8))
                         ->pl()
                         .getResEdges()
                         .begin();
      found = true;
      if (!origNode->pl().hasOrderedEdge(outgoing[i])) {
        std::cerr << "Warning: tried to balance edge " << outgoing[i]
                  << " in node " << origNode
                  << ", but the edge does not appear there." << std::endl;
        return;
      }
    } else {
      outgoing[i] = 0;
    }
  }

  std::cerr << "Edge distribution: ";
  if (origNode->pl().getParent()->pl().getStops().size())
    std::cerr << "in "
              << origNode->pl().getParent()->pl().getStops().front().name;
  std::cerr << " ";
  for (size_t i = 0; i < 8; i++) {
    std::cerr << outgoing[i] << ", ";
  }
  std::cerr << std::endl;

  std::cerr << "Ordered edges in orig node: ";
  for (auto e : origNode->pl().getOrderedEdges()) {
    if (e.first->getOtherNode(origNode)
            ->pl()
            .getParent()
            ->pl()
            .getStops()
            .size()) {
      std::cerr << e.first << "(to "
                << e.first->getOtherNode(origNode)
                       ->pl()
                       .getParent()
                       ->pl()
                       .getStops()
                       .front()
                       .name
                << "), ";
    } else {
      std::cerr << e.first << ", ";
    }
  }
  std::cerr << std::endl;

  if (!found) return;

  double addC[8] = {0, 0, 0, 0, 0, 0, 0, 0};

  for (size_t i = 0; i < 8; i++) {
    if (!outgoing[i]) continue;

    // this is the number of edges that will occur between the currently checked
    // edge and the inserted edge,
    // positive if in clockwise direction, negative if in counter-clockwise
    // direction
    int d = origNode->pl().distBetween(outgoing[i], e) - 1;

    // dd and ddd are the optimal distances between outgoing[i] and e, based on
    // the total number
    // of edges in this node
    int dd = ((((d + 1) + d) % 8) * optimDistance) % 8;
    int ddd = (6 - dd) % 8;

    std::cerr << "Distance between the inserted edge (" << e << ") and edge at "
              << i << " (" << outgoing[i] << ") is " << d
              << ", optim distance between them is +" << dd << " and -" << ddd
              << std::endl;

    for (int j = 0; j <= dd + 1; j++) {
      addC[(i + j) % 8] += 20 * (dd + 1 - j);
    }

    for (int j = 0; j <= ddd + 1; j++) {
      addC[(i + (8 - j)) % 8] += 20 * (ddd + 1 - j);
    }

    if (d > 0) {
      for (int j = 1; j <= d; j++) {
        addC[(i + j) % 8] += 99999;
      }
    } else if (d < 0) {
      for (int j = 1; j <= abs(d); j++) {
        addC[(i + (8 - j)) % 8] += 99999;
      }
    }
  }

  // topological blocking
  for (size_t i = 0; i < 8; i++) {
    if (!outgoing[i]) continue;

    for (size_t j = i + 1; j < i + 8; j++) {
      if (!outgoing[j % 8]) continue;
      if (outgoing[j % 8] == outgoing[i]) break;

      std::cerr << i << " vs " << (j % 8) << std::endl;
      int da = origNode->pl().distBetween(outgoing[i], e);
      int db = origNode->pl().distBetween(outgoing[j % 8], e);

      std::cerr << "da = " << da << " vs "
                << "db = " << db << std::endl;

      if (db < da) {
        // edge does not lie in this segment, block it!

        for (size_t x = i + 1; x < j; x++) {
          addC[x % 8] = 999999;
        }
      }
    }

    break;
  }

  std::cerr << "Cost distribution: ";
  for (size_t i = 0; i < 8; i++) {
    std::cerr << addC[i] << ", ";
  }
  std::cerr << std::endl;

  for (size_t i = 0; i < 8; i++) {
    auto port = n->pl().getPort(i);
    auto neigh = getNeighbor(x, y, i);

    if (!neigh) continue;
    getEdge(port, neigh->pl().getPort((i + 4) % 8))
        ->pl()
        .setCost(

            getEdge(port, neigh->pl().getPort((i + 4) % 8))->pl().cost() +
            addC[i]);
    getEdge(neigh->pl().getPort((i + 4) % 8), port)
        ->pl()
        .setCost(

            getEdge(neigh->pl().getPort((i + 4) % 8), port)->pl().cost() +
            addC[i]);
  }
}

// _____________________________________________________________________________
std::set<util::graph::Edge<octi::graph::CombNodePL, octi::graph::CombEdgePL>*>
GridGraph::getResEdges(Node<NodePL, EdgePL>* n) const {
  std::set<util::graph::Edge<octi::graph::CombNodePL, octi::graph::CombEdgePL>*>
      ret;

  for (size_t i = 0; i < 8; i++) {
    auto port = n->pl().getPort(i);
    for (auto e : port->getAdjList()) {
      ret.insert(e->pl().getResEdges().begin(), e->pl().getResEdges().end());
    }
  }

  return ret;
}

// _____________________________________________________________________________
void GridGraph::writeInitialCosts() {
  for (size_t x = 0; x < _grid.getXWidth(); x++) {
    for (size_t y = 0; y < _grid.getYHeight(); y++) {
      auto n = getNode(x, y);

      for (size_t i = 0; i < 8; i++) {
        auto port = n->pl().getPort(i);
        auto neigh = getNeighbor(x, y, i);
        if (!neigh || !port) continue;
        auto e = getEdge(port, neigh->pl().getPort((i + 4) % 8));

        if (i % 4 == 0) e->pl().setCost(_c.verticalCost);
        if ((i + 2) % 4 == 0) e->pl().setCost(_c.horizontalCost);
        if (i % 2) e->pl().setCost(_c.diagonalCost);
      }
    }
  }
}

// _____________________________________________________________________________
std::priority_queue<Candidate> GridGraph::getNearestCandidatesFor(
    const util::geo::Point& p, double maxD) const {
  std::priority_queue<Candidate> ret;
  std::set<Node<NodePL, EdgePL>*> neigh;
  util::geo::Box b(util::geo::Point(p.get<0>() - maxD, p.get<1>() - maxD),
                   util::geo::Point(p.get<0>() + maxD, p.get<1>() + maxD));
  _grid.get(b, &neigh);

  for (auto n : neigh) {
    double d = util::geo::dist(n->pl().getGeom(), p);
    if (d < maxD) {
      ret.push(Candidate(n, d));
    }
  }

  return ret;
}

// _____________________________________________________________________________
const Grid<Node<NodePL, EdgePL>*, Point>& GridGraph::getGrid() const {
  return _grid;
}
