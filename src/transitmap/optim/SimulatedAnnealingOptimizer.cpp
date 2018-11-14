// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <algorithm>
#include <unordered_map>
#include "transitmap/optim/SimulatedAnnealingOptimizer.h"
#include "util/log/Log.h"

using namespace transitmapper;
using namespace optim;
using namespace transitmapper::graph;
using transitmapper::optim::SimulatedAnnealingOptimizer;

// _____________________________________________________________________________
int SimulatedAnnealingOptimizer::optimizeComp(const std::set<OptNode*>& g,
                                          HierarchOrderingConfig* hc) const {
  OptOrderingConfig cur, null;

  // fixed order list of optim graph edges
  std::vector<OptEdge*> edges;

  for (auto n : g)
    for (auto e : n->getAdjList())
      if (n == e->getFrom()) edges.push_back(e);

  // this guarantees that all the orderings are sorted!
  initialConfig(g, &null, true);

  // this is the starting ordering, which is random
  initialConfig(g, &cur, false);

  size_t iters = 0;
  size_t last = 0;

  size_t k = 0;

  size_t ABORT_AFTER_UNCH = 5000;

  while (true) {
    iters++;

    double temp = 1000.0 / iters;

    if (iters - last == 10000) {
      LOG(DEBUG) << "@ " << iters << ", temp = " << temp << ", last change was at " << k << " iters.";
      last = iters;
    }

    int i = rand() % edges.size();

    double oldScore = getScore(edges[i], cur);
    auto old = cur[edges[i]];
    cur[edges[i]] = null[edges[i]];

    int c = rand() % util::factorial(edges[i]->pl().getCardinality());

    for (int j = 0; j < c; j++)
        std::next_permutation(cur[edges[i]].begin(), cur[edges[i]].end());

    double s = getScore(edges[i], cur);

    double r = rand() / (RAND_MAX + 1.0);
    double e = exp(-(1.0 * (s - oldScore)) / temp);

    if (s < oldScore) {
      // found a better solution
      k = iters;
    } else if (s != oldScore && e > r) {
      // take this value, despite not bringing any local gain
      k = iters;
    } else {
      // reverting
      cur[edges[i]] = old;
    }

    if (iters - k > ABORT_AFTER_UNCH) break;
  }

  double curScore = _optScorer.getCrossingScore(g, cur);
  if (_cfg->splittingOpt) curScore += _optScorer.getSplittingScore(g, cur);

  LOG(DEBUG) << "Stopped after " << iters << " iterations. Final target = " << curScore;

  writeHierarch(&cur, hc);
  return iters;
}

// _____________________________________________________________________________
double SimulatedAnnealingOptimizer::getScore(OptEdge* e,
                                             OptOrderingConfig& cur) const {
  double curScore = _optScorer.getCrossingScore(e, cur);
  if (_cfg->splittingOpt) curScore += _optScorer.getSplittingScore(e, cur);
  return curScore;
}