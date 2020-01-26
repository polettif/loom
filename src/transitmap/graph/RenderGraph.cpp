// Copyright 2016, University of Freiburg,
// Chair of Algorithms and Data Structures.
// Authors: Patrick Brosi <brosi@informatik.uni-freiburg.de>

#include <set>
#include <string>
#include "json/json.hpp"
#include "shared/linegraph/Line.h"
#include "transitmap/graph/OrderCfg.h"
#include "transitmap/graph/RenderGraph.h"
#include "util/Misc.h"
#include "util/geo/BezierCurve.h"
#include "util/geo/Geo.h"

// we still need boost geometry for the polygon buffering, which is no
// yet implemented in util/Geo.h
#include <boost/geometry.hpp>
#include <boost/geometry/strategies/transform/matrix_transformers.hpp>

namespace bgeo = boost::geometry;

using util::geo::Point;
using util::geo::DPoint;
using util::geo::Box;
using util::geo::dist;
using transitmapper::graph::RenderGraph;
using shared::linegraph::Line;
using shared::linegraph::LineEdge;
using shared::linegraph::LineNode;
using shared::linegraph::LineOcc;
using shared::linegraph::NodeFront;
using shared::linegraph::Partner;
using transitmapper::graph::OrderCfg;
using shared::linegraph::InnerGeom;
using util::geo::BezierCurve;
using util::geo::PolyLine;
using util::geo::Polygon;
using util::geo::MultiLine;

// _____________________________________________________________________________
const OrderCfg& RenderGraph::getConfig() const { return _config; }

// _____________________________________________________________________________
void RenderGraph::setConfig(const OrderCfg& c) { _config = c; }

// _____________________________________________________________________________
size_t RenderGraph::numEdgs() const { return 0; }

// _____________________________________________________________________________
std::vector<InnerGeom> RenderGraph::innerGeoms(const LineNode* n,
                                               const OrderCfg& c,
                                               double prec) const {
  std::vector<InnerGeom> ret;
  std::map<const Line*, std::set<const shared::linegraph::NodeFront*>>
      processed;

  for (size_t i = 0; i < n->pl().fronts().size(); ++i) {
    const shared::linegraph::NodeFront& nf = n->pl().fronts()[i];

    if (!c.count(nf.edge)) {
      std::cout << "No ordering for edge " << nf.edge << " found!" << std::endl;
      assert(false);
    }
    const std::vector<size_t>* ordering = &c.find(nf.edge)->second;

    for (size_t j : *ordering) {
      const LineOcc& lineOcc = nf.edge->pl().lineOccAtPos(j);
      Partner o(&nf, nf.edge, lineOcc.line);

      std::vector<Partner> partners = getPartners(n, &nf, lineOcc);

      for (const Partner& p : partners) {
        if (processed[lineOcc.line].find(p.front) !=
            processed[lineOcc.line].end()) {
          continue;
        }

        auto is = getInnerStraightLine(n, c, o, p);
        if (is.geom.getLength() == 0) continue;

        if (prec > 0) {
          ret.push_back(getInnerBezier(n, c, o, p, prec));
        } else {
          ret.push_back(is);
        }
      }

      // handle lines where this node is the terminus
      if (partners.size() == 0) {
        auto is = getTerminusStraightLine(n, c, o);
        if (is.geom.getLength() > 0) {
          if (prec > 0) {
            ret.push_back(getTerminusBezier(n, c, o, prec));
          } else {
            ret.push_back(is);
          }
        }
      }

      processed[lineOcc.line].insert(&nf);
    }
  }

  return ret;
}

// _____________________________________________________________________________
InnerGeom RenderGraph::getInnerBezier(const LineNode* n, const OrderCfg& cf,
                                      const Partner& partnerFrom,
                                      const Partner& partnerTo,
                                      double prec) const {
  double EPSI = 0.001;
  InnerGeom ret = getInnerStraightLine(n, cf, partnerFrom, partnerTo);
  DPoint p = ret.geom.getLine().front();
  DPoint pp = ret.geom.getLine().back();
  double d = util::geo::dist(p, pp);

  DPoint b = p;
  DPoint c = pp;
  std::pair<double, double> slopeA, slopeB;

  if (util::geo::len(*partnerFrom.front->edge->pl().getGeom()) <= 5) return ret;
  if (util::geo::len(*partnerTo.front->edge->pl().getGeom()) <= 5) return ret;

  if (d <= 5) return ret;

  if (partnerFrom.front->edge->getTo() == n) {
    slopeA =
        PolyLine<double>(*partnerFrom.front->edge->pl().getGeom())
            .getSlopeBetweenDists(
                util::geo::len(*partnerFrom.front->edge->pl().getGeom()) - 5,
                util::geo::len(*partnerFrom.front->edge->pl().getGeom()));
  } else {
    slopeA = PolyLine<double>(*partnerFrom.front->edge->pl().getGeom())
                 .getSlopeBetweenDists(5, 0);
  }

  if (partnerTo.front->edge->getTo() == n) {
    slopeB = PolyLine<double>(*partnerTo.front->edge->pl().getGeom())
                 .getSlopeBetweenDists(
                     util::geo::len(*partnerTo.front->edge->pl().getGeom()) - 5,
                     util::geo::len(*partnerTo.front->edge->pl().getGeom()));
  } else {
    slopeB = PolyLine<double>(*partnerTo.front->edge->pl().getGeom())
                 .getSlopeBetweenDists(5, 0);
  }

  double da = 1;
  double db = 1;

  DPoint pa =
      DPoint(p.getX() - slopeA.first * d * 2, p.getY() - slopeA.second * d * 2);
  DPoint pb = DPoint(pp.getX() - slopeB.first * d * 2,
                     pp.getY() - slopeB.second * d * 2);
  DPoint ppa =
      DPoint(p.getX() + slopeA.first * d * 2, p.getY() + slopeA.second * d * 2);
  DPoint ppb = DPoint(pp.getX() + slopeB.first * d * 2,
                      pp.getY() + slopeB.second * d * 2);

  if (d > EPSI && util::geo::intersects(pa, ppa, pb, ppb)) {
    DPoint isect = util::geo::intersection(pa, ppa, pb, ppb);

    if (!std::isnan(isect.getX()) && !std::isnan(isect.getY())) {
      double degAng = util::geo::innerProd(isect, pa, ppb);
      double ang = cos(degAng / (180 / M_PI));

      ang = ang * ang;

      if (std::isnan(ang)) ang = 1;

      if (std::max(partnerFrom.edge->pl().getLines().size(),
                   partnerTo.edge->pl().getLines().size()) > 1) {
        double fac =
            fabs((double)((int)partnerFrom.edge->pl().getLines().size() -
                          (int)partnerTo.edge->pl().getLines().size())) /
            (double)(std::max(partnerFrom.edge->pl().getLines().size(),
                              partnerTo.edge->pl().getLines().size()) -
                     1);

        fac = pow(fac, 2);
        ang = pow(ang, fac);
      }

      double dar = util::geo::dist(isect, p);
      double dbr = util::geo::dist(isect, pp);

      double avg = (dar + dbr) / 2;

      da = (dar * 2 * (1 - ang) + avg * 2 * ang) / 2;
      db = (dbr * 2 * (1 - ang) + avg * 2 * ang) / 2;
    }
  }

  if ((da + db) < EPSI || (da + db) > d * 2) {
    da = 1;
    db = 1;
  }

  b = DPoint(p.getX() + slopeA.first * d * (da / (da + db)),
             p.getY() + slopeA.second * d * (da / (da + db)));
  c = DPoint(pp.getX() + slopeB.first * d * (db / (da + db)),
             pp.getY() + slopeB.second * d * (db / (da + db)));

  BezierCurve<double> bc(p, b, c, pp);
  ret.geom = bc.render(prec);

  return ret;
}

// _____________________________________________________________________________
InnerGeom RenderGraph::getTerminusStraightLine(
    const LineNode* n, const OrderCfg& c, const Partner& partnerFrom) const {
  DPoint p = linePosOn(*partnerFrom.front, partnerFrom.line, c, false);
  DPoint pp = linePosOn(*partnerFrom.front, partnerFrom.line, c, true);

  size_t s = partnerFrom.edge->pl().linePosUnder(
      partnerFrom.line, c.find(partnerFrom.edge)->second);
  size_t ss = partnerFrom.edge->pl().linePosUnder(
      partnerFrom.line, c.find(partnerFrom.edge)->second);

  return InnerGeom(PolyLine<double>(p, pp), partnerFrom, Partner(), s, ss);
}

// _____________________________________________________________________________
InnerGeom RenderGraph::getInnerStraightLine(const LineNode* n,
                                            const OrderCfg& c,
                                            const Partner& partnerFrom,
                                            const Partner& partnerTo) const {
  DPoint p = linePosOn(*partnerFrom.front, partnerFrom.line, c, false);
  DPoint pp = linePosOn(*partnerTo.front, partnerTo.line, c, false);

  size_t s = partnerFrom.edge->pl().linePosUnder(
      partnerFrom.line, c.find(partnerFrom.edge)->second);
  size_t ss = partnerTo.edge->pl().linePosUnder(partnerTo.line,
                                                c.find(partnerTo.edge)->second);

  return InnerGeom(PolyLine<double>(p, pp), partnerFrom, partnerTo, s, ss);
}

// _____________________________________________________________________________
InnerGeom RenderGraph::getTerminusBezier(const LineNode* n, const OrderCfg& cf,
                                         const Partner& partnerFrom,
                                         double prec) const {
  InnerGeom ret = getTerminusStraightLine(n, cf, partnerFrom);
  DPoint p = ret.geom.getLine().front();
  DPoint pp = ret.geom.getLine().back();
  double d = util::geo::dist(p, pp) / 2;

  DPoint b = p;
  DPoint c = pp;
  std::pair<double, double> slopeA;

  assert(util::geo::len(*partnerFrom.front->edge->pl().getGeom()) > 5);

  if (partnerFrom.front->edge->getTo() == n) {
    slopeA =
        PolyLine<double>(*partnerFrom.front->edge->pl().getGeom())
            .getSlopeBetweenDists(
                util::geo::len(*partnerFrom.front->edge->pl().getGeom()) - 5,
                util::geo::len(*partnerFrom.front->edge->pl().getGeom()));
  } else {
    slopeA = PolyLine<double>(*partnerFrom.front->edge->pl().getGeom())
                 .getSlopeBetweenDists(5, 0);
  }

  b = DPoint(p.getX() + slopeA.first * d, p.getY() + slopeA.second * d);
  c = DPoint(pp.getX(), pp.getY());

  BezierCurve<double> bc(p, b, c, pp);
  ret.geom = bc.render(prec);

  return ret;
}

// _____________________________________________________________________________
Polygon<double> RenderGraph::getConvexFrontHull(
    const LineNode* n, double d, bool rectangulize,
    bool simpleRenderForTwoEdgeNodes) const {
  double cd = d;

  typedef bgeo::model::point<double, 2, bgeo::cs::cartesian> BoostPoint;
  typedef bgeo::model::linestring<BoostPoint> BoostLine;
  typedef bgeo::model::polygon<BoostPoint> BoostPoly;
  typedef bgeo::model::multi_polygon<BoostPoly> BoostMultiPoly;

  BoostMultiPoly ret;
  double pointsPerCircle = 36;
  bgeo::strategy::buffer::distance_symmetric<double> distanceStrat(d);
  bgeo::strategy::buffer::join_round joinStrat(pointsPerCircle);
  bgeo::strategy::buffer::end_round endStrat(pointsPerCircle);
  bgeo::strategy::buffer::point_circle circleStrat(pointsPerCircle);
  bgeo::strategy::buffer::side_straight sideStrat;

  if (!simpleRenderForTwoEdgeNodes || n->pl().fronts().size() != 2) {
    MultiLine<double> l;
    for (auto& nf : n->pl().fronts()) {
      l.push_back(nf.origGeom
                      .getSegment((cd / 2) / nf.origGeom.getLength(),
                                  (nf.origGeom.getLength() - cd / 2) /
                                      nf.origGeom.getLength())
                      .getLine());
    }

    Polygon<double> hull = util::geo::convexHull(l);

    if (rectangulize && getMaxLineNum(n) > 1) {
      MultiLine<double> ll;
      for (auto& nf : n->pl().fronts()) {
        ll.push_back(nf.origGeom.getLine());
      }
      Polygon<double> env = util::geo::convexHull(
          util::geo::shrink(util::geo::getOrientedEnvelopeAvg(ll), cd / 2));

      double incr = (util::geo::area(env) / util::geo::area(hull)) - 1;
      if (ll.size() < 5 || incr < 0.5) {
        hull = env;
      }
    }

    BoostPoly hullBgeo;
    for (const auto& p : hull.getOuter())
      hullBgeo.outer().push_back({p.getX(), p.getY()});
    hullBgeo.outer().push_back(
        {hull.getOuter().front().getX(), hull.getOuter().front().getY()});

    // boost geometry expects polygons in clockwise fashion
    bgeo::correct(hullBgeo);

    bgeo::buffer(hullBgeo, ret, distanceStrat, sideStrat, joinStrat, endStrat,
                 circleStrat);

  } else {
    // for two main dirs, take average
    std::vector<const PolyLine<double>*> pols;

    PolyLine<double> a = n->pl().fronts()[0].origGeom.getSegment(
        (cd / 2) / n->pl().fronts()[0].origGeom.getLength(),
        (n->pl().fronts()[0].origGeom.getLength() - cd / 2) /
            n->pl().fronts()[0].origGeom.getLength());

    PolyLine<double> b = n->pl().fronts()[1].origGeom.getSegment(
        (cd / 2) / n->pl().fronts()[1].origGeom.getLength(),
        (n->pl().fronts()[1].origGeom.getLength() - cd / 2) /
            n->pl().fronts()[1].origGeom.getLength());

    assert(a.getLine().size() > 1);
    assert(b.getLine().size() > 1);

    if (dist(a.getLine()[0], b.getLine()[0]) >
        dist(a.getLine()[1], b.getLine()[0])) {
      a.reverse();
    }

    pols.push_back(&a);
    pols.push_back(&b);

    auto avg = PolyLine<double>::average(pols).getLine();

    BoostLine lineBgeo;
    for (const auto& p : avg) {
      lineBgeo.push_back({p.getX(), p.getY()});
    }

    bgeo::buffer(lineBgeo, ret, distanceStrat, sideStrat, joinStrat, endStrat,
                 circleStrat);
  }

  assert(ret.size() > 0);

  Polygon<double> retPoly;
  for (const auto& p : ret[0].outer()) {
    retPoly.getOuter().push_back({p.get<0>(), p.get<1>()});
  }

  return retPoly;
}

// _____________________________________________________________________________
Polygon<double> RenderGraph::getStationHull(const LineNode* n, double d,
                                            bool simple) const {
  if (n->pl().fronts().size() == 0) return Polygon<double>();
  return getConvexFrontHull(n, d, true, simple);
}

// _____________________________________________________________________________
double RenderGraph::getTotalWidth(const LineEdge* e) const {
  return _defWidth * e->pl().getLines().size() +
         _defSpacing * (e->pl().getLines().size() - 1);
}

// _____________________________________________________________________________
size_t RenderGraph::getConnCardinality(const LineNode* n) {
  size_t ret = 0;
  std::map<const Line*, std::set<const shared::linegraph::NodeFront*>>
      processed;

  for (const auto& nf : n->pl().fronts()) {
    for (size_t j = 0; j < nf.edge->pl().getLines().size(); j++) {
      const auto& lineOcc = nf.edge->pl().lineOccAtPos(j);

      std::vector<Partner> partners = getPartners(n, &nf, lineOcc);

      for (const Partner& p : partners) {
        if (processed[lineOcc.line].find(p.front) !=
            processed[lineOcc.line].end()) {
          continue;
        }
        ret++;
      }

      processed[lineOcc.line].insert(&nf);
    }
  }

  return ret;
}

// _____________________________________________________________________________
double RenderGraph::getWidth(const shared::linegraph::LineEdge* e) const {
  return _defWidth;
}

// _____________________________________________________________________________
double RenderGraph::getSpacing(const shared::linegraph::LineEdge* e) const {
  return _defSpacing;
}

// _____________________________________________________________________________
double RenderGraph::getMaxNdFrontWidth(
    const shared::linegraph::LineNode* n) const {
  double ret = 0;
  for (const auto& g : n->pl().fronts()) {
    if (getTotalWidth(g.edge) > ret) ret = getTotalWidth(g.edge);
  }
  return ret;
}

// _____________________________________________________________________________
size_t RenderGraph::getMaxNdFrontCard(
    const shared::linegraph::LineNode* n) const {
  size_t ret = 0;
  for (const auto& g : n->pl().fronts()) {
    if (g.edge->pl().getLines().size() > ret)
      ret = g.edge->pl().getLines().size();
  }
  return ret;
}

// _____________________________________________________________________________
DPoint RenderGraph::linePosOn(const NodeFront& nf, const Line* r,
                              const OrderCfg& c, bool origGeom) const {
  assert(c.find(nf.edge) != c.end());

  size_t p = nf.edge->pl().linePosUnder(r, c.find(nf.edge)->second);
  return linePosOn(nf, nf.edge, p, nf.n == nf.edge->getTo(), origGeom);
}

// _____________________________________________________________________________
DPoint RenderGraph::linePosOn(const NodeFront& nf, const LineEdge* e,
                              size_t pos, bool inv, bool origG) const {
  double p;
  if (!inv) {
    p = (getWidth(e) + getSpacing(e)) * pos + getWidth(e) / 2;
  } else {
    p = (getWidth(e) + getSpacing(e)) * (e->pl().getLines().size() - 1 - pos) +
        getWidth(e) / 2;
  }
  // use interpolate here directly for speed
  if (origG) {
    return nf.origGeom.interpolate(nf.origGeom.getLine().front(),
                                   nf.origGeom.getLine().back(), p);
  } else {
    return nf.geom.interpolate(nf.geom.getLine().front(),
                               nf.geom.getLine().back(), p);
  }
}