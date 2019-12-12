// Copyright 2016
// Author: Patrick Brosi


#include <string>

#include "shared/transitgraph/TransitGraph.h"
#include "topo/config/TopoConfig.h"
#include "topo/tests/TopologicalTest.h"
#include "util/Misc.h"
#include "util/Nullable.h"
#include "util/String.h"
#include "util/geo/output/GeoGraphJsonOutput.h"
#include "topo/tests/TopoTestUtil.h"

#define private public
#include "topo/mapconstructor/MapConstructor.h"

using util::approx;

// _____________________________________________________________________________
void TopologicalTest::run() {
  // ___________________________________________________________________________
  {
    //      2->     1
    //     a--> b <---|
    // c -----> d <---e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto cd = tg.addEdg(c, d, {{{0.0, 0.0}, {100.0, 0.0}}});
    auto ed = tg.addEdg(e, d, {{{200.0, 0.0}, {100, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    cd->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    ed->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    // util::geo::output::GeoGraphJsonOutput gout;
    // gout.print(tg, std::cout);
    // std::cout << std::flush;

    TEST(c->getAdjList().size(), !=, 0);
    a = c->getAdjList().front()->getOtherNd(c);
    for (auto edg : a->getAdjList()) {
      if (edg->getOtherNd(a) != c) d = edg->getOtherNd(a);
    }
    for (auto edg : d->getAdjList()) {
      if (edg->getOtherNd(d) != a) e = edg->getOtherNd(d);
    }

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRouteOcc(&l2).direction, ==, c);

    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    TEST(tg.getEdg(a, d)->pl().getRoutes().size(), ==, 1);
    TEST(tg.getEdg(a, d)->pl().getRouteOcc(&l2).direction, ==, 0);

    TEST(tg.getEdg(d, e)->pl().getRoutes().size(), ==, 2);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l2).direction, ==, d);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l1).direction, ==, 0);
  }

  // ___________________________________________________________________________
  {
    //      2->     1
    //     a--> b <---|
    // c <----- d <---e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto ed = tg.addEdg(e, d, {{{200.0, 0.0}, {100, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    ed->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    // util::geo::output::GeoGraphJsonOutput gout;
    // gout.print(tg, std::cout);
    // std::cout << std::flush;

    a = c->getAdjList().front()->getOtherNd(c);
    for (auto edg : a->getAdjList()) {
      if (edg->getOtherNd(a) != c) d = edg->getOtherNd(a);
    }
    for (auto edg : d->getAdjList()) {
      if (edg->getOtherNd(d) != a) e = edg->getOtherNd(d);
    }

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRouteOcc(&l2).direction, ==, c);

    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    TEST(tg.getEdg(a, d)->pl().getRoutes().size(), ==, 1);
    TEST(tg.getEdg(a, d)->pl().getRouteOcc(&l2).direction, ==, 0);

    TEST(tg.getEdg(d, e)->pl().getRoutes().size(), ==, 2);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l2).direction, ==, d);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l1).direction, ==, 0);
  }

  // ___________________________________________________________________________
  {
    //      2->     1
    //     a--> b <---|
    // c <----- d --->e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto de = tg.addEdg(d, e, {{{100.0, 0.0}, {200, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    de->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    a = c->getAdjList().front()->getOtherNd(c);
    for (auto edg : a->getAdjList()) {
      if (edg->getOtherNd(a) != c) d = edg->getOtherNd(a);
    }
    for (auto edg : d->getAdjList()) {
      if (edg->getOtherNd(d) != a) e = edg->getOtherNd(d);
    }

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRouteOcc(&l2).direction, ==, c);

    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    TEST(tg.getEdg(a, d)->pl().getRoutes().size(), ==, 1);
    TEST(tg.getEdg(a, d)->pl().getRouteOcc(&l2).direction, ==, 0);

    TEST(tg.getEdg(d, e)->pl().getRoutes().size(), ==, 2);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l2).direction, ==, d);
    TEST(tg.getEdg(d, e)->pl().getRouteOcc(&l1).direction, ==, 0);
  }

  // ___________________________________________________________________________
  {
    //             1
    //          b <---|
    // c -----> d <---e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto cd = tg.addEdg(d, c, {{{0.0, 0.0}, {100.0, 0.0}}});
    auto ed = tg.addEdg(e, d, {{{200, 0.0}, {100.0, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    cd->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    ed->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    cd = c->getAdjList().front();
    for (auto* nd : *tg.getNds()) {
      if (nd->getDeg() == 1 && nd != c) e = nd;
      if (nd->getDeg() == 2) d = nd;
    }
    ed = e->getAdjList().front();
    TEST(cd->pl().hasRoute(&l2) && cd->pl().getRouteOcc(&l2).direction, ==, c);
    TEST(ed->pl().hasRoute(&l2) && ed->pl().getRouteOcc(&l2).direction, !=, 0);
    TEST(ed->pl().getRouteOcc(&l2).direction, !=, e);
  }

  // ___________________________________________________________________________
  {
    //             1
    //          b <---|
    // c <----- d --->e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto de = tg.addEdg(d, e, {{{100.0, 0.0}, {200, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    de->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //    <-2          1,<-2
    // c ---------> d < --- e

    for (auto* nd : *tg.getNds()) {
      if (nd->getDeg() == 1 && nd != c) e = nd;
      if (nd->getDeg() == 2) d = nd;
    }

    TEST(tg.getNds()->size(), ==, 3);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto ro : e->getAdjList().front()->pl().getRoutes()) {
      if (ro.route == &l1) TEST(ro.direction, ==, 0);
      if (ro.route == &l2) TEST(ro.direction->pl().getGeom()->getX(), ==, approx(100));
    }
  }
  // ___________________________________________________________________________
  {
    //             1
    //          b <---|
    // c <----- d <---e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto ed = tg.addEdg(e, d, {{{200, 0.0}, {100.0, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    ed->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //    <-2          1,<-2
    // c ---------> d < --- e

    for (auto* nd : *tg.getNds()) {
      if (nd->getDeg() == 1 && nd != c) e = nd;
      if (nd->getDeg() == 2) d = nd;
    }

    TEST(tg.getNds()->size(), ==, 3);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto ro : e->getAdjList().front()->pl().getRoutes()) {
      if (ro.route == &l1) TEST(ro.direction, ==, 0);
      if (ro.route == &l2) TEST(ro.direction->pl().getGeom()->getX(), ==, approx(100));
    }
  }

  // ___________________________________________________________________________
  {
    //      2->     1
    //     a<-- b <---|
    // c <------ d --->e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{96.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ba = tg.addEdg(b, a, {{{95.0, 10.0}, {30.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto de = tg.addEdg(d, e, {{{100.0, 0.0}, {200, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {95, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ba->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    de->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();
    mc.removeNodeArtifacts();

    // util::geo::output::GeoGraphJsonOutput gout;
    // gout.print(tg, std::cout);
    // std::cout << std::flush;

    //    <-2   2        1,<-2
    // c ----a-----> d < --- e

    for (auto* nd : *tg.getNds()) {
      if (nd->getDeg() == 1 && nd != c) e = nd;
      if (nd->getDeg() == 2) d = nd;
    }

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto edg : a->getAdjList()) {
      if (edg->getOtherNd(a) == c) continue;

      TEST(edg->pl().getRoutes().size(), ==, 1);
      TEST(edg->pl().getRoutes().begin()->direction, ==, 0);
    }

    for (auto ro : e->getAdjList().front()->pl().getRoutes()) {
      if (ro.route == &l1) TEST(ro.direction, ==, 0);

      if (ro.route == &l2) {
        TEST(ro.direction->pl().getGeom()->getX(), <, 130);
        TEST(ro.direction->pl().getGeom()->getX(), >, 70);
      }
    }
  }

  // ___________________________________________________________________________
  {
    //     1
    // a ------> b
    // c ------> d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{0.0, 10.0}});
    auto b = tg.addNd({{50.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{50.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{0.0, 10.0}, {50.0, 10.0}}});
    auto cd = tg.addEdg(c, d, {{{0.0, 0.0}, {50.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    cd->pl().addRoute(&l2, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //     1, 2
    // a ------> b

    TEST(tg.getNds()->size(), ==, 2);
    TEST((*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto r : (*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) TEST(r.direction, ==, 0);
    }
  }

  // ___________________________________________________________________________
  {
    //     1
    // a ------> b
    // c <------ d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{0.0, 10.0}});
    auto b = tg.addNd({{50.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{50.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{0.0, 10.0}, {50.0, 10.0}}});
    auto cd = tg.addEdg(d, c, {{{50.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    cd->pl().addRoute(&l2, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //     1, 2
    // a ------> b

    TEST(tg.getNds()->size(), ==, 2);
    TEST((*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto r : (*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) TEST(r.direction, ==, 0);
    }
  }

  // ___________________________________________________________________________
  {
    //     1
    // a ------> b
    // c <------ d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{0.0, 10.0}});
    auto b = tg.addNd({{50.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{50.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{0.0, 10.0}, {50.0, 10.0}}});
    auto cd = tg.addEdg(d, c, {{{50.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    cd->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //     1, 2
    // a ------> b

    TEST(tg.getNds()->size(), ==, 2);
    TEST((*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto r : (*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) {
        TEST(r.direction, !=, 0);
        TEST(r.direction->pl().getGeom()->getX(), ==, approx(50));
      }
    }
  }

  // ___________________________________________________________________________
  {
    //     1
    // a ----------> b
    // c <---------- d
    //      2->
    // e <---------- f
    //    <-3,2->
    // g <---------- h
    //      3
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{0.0, 10.0}});
    auto b = tg.addNd({{50.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{50.0, 0.0}});

    auto e = tg.addNd({{0.0, 20.0}});
    auto f = tg.addNd({{50.0, 20.0}});
    auto g = tg.addNd({{0.0, 30.0}});
    auto h = tg.addNd({{50.0, 30.0}});

    auto ab = tg.addEdg(a, b, {{{0.0, 10.0}, {50.0, 10.0}}});
    auto cd = tg.addEdg(d, c, {{{50.0, 0.0}, {0.0, 0.0}}});

    auto fe = tg.addEdg(f, e, {{{50.0, 20.0}, {0.0, 20.0}}});
    auto hg = tg.addEdg(h, g, {{{50.0, 30.0}, {0.0, 30.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");
    shared::transitgraph::Route l3("3", "3", "green");
    shared::transitgraph::Route l4("4", "4", "black");

    ab->pl().addRoute(&l1, 0);
    cd->pl().addRoute(&l2, d);
    fe->pl().addRoute(&l3, e);
    fe->pl().addRoute(&l2, f);
    hg->pl().addRoute(&l3, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //  1, 2->, 3
    // a ------> b

    TEST(tg.getNds()->size(), ==, 2);
    TEST((*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes().size(), ==, 3);

    for (auto r : (*tg.getNds()->begin())->getAdjList().front()->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) {
        TEST(r.direction, !=, 0);
        TEST(r.direction->pl().getGeom()->getX(), >, 49);
        TEST(r.direction->pl().getGeom()->getX(), <, 51);
      }
      if (r.route == &l3) {
        TEST(r.direction, ==, 0);
      }
    }
  }

  // ___________________________________________________________________________
  {
    //      1
    //    a->b
    // c ------> d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{70.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {70.0, 10.0}}});
    auto cd = tg.addEdg(c, d, {{{0.0, 0.0}, {100.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    cd->pl().addRoute(&l2, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //    2   1,2    2
    // c ----a--->b----> d

    a = c->getAdjList().front()->getOtherNd(c);
    b = d->getAdjList().front()->getOtherNd(d);

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(d->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(tg.getEdg(a, b), !=, 0);
    TEST(tg.getEdg(a, b)->pl().getRoutes().size(), ==, 2);

    for (auto r : tg.getEdg(a, b)->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) TEST(r.direction, ==, 0);
    }
  }

  // ___________________________________________________________________________
  {
    //      1
    //     a->b
    // c <----- d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{70.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {70.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    dc->pl().addRoute(&l2, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    a = c->getAdjList().front()->getOtherNd(c);
    b = d->getAdjList().front()->getOtherNd(d);


    //    2   1,2    2
    // c ----a--->b----> d

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(d->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(tg.getEdg(a, b), !=, 0);
    TEST(tg.getEdg(a, b)->pl().getRoutes().size(), ==, 2);

    for (auto r : tg.getEdg(a, b)->pl().getRoutes()) {
      if (r.route == &l1) TEST(r.direction, ==, 0);
      if (r.route == &l2) TEST(r.direction, ==, 0);
    }
  }

  // ___________________________________________________________________________
  {
    //      1
    //     a--> b
    // c <----- d
    //     2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l1, 0);
    dc->pl().addRoute(&l2, 0);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    // util::geo::output::GeoGraphJsonOutput gout;
    // gout.print(tg, std::cout);
    // std::cout << std::flush;

    //    2     1,2
    // c ----a-----> d

    a = c->getAdjList().front()->getOtherNd(c);

    TEST(tg.getNds()->size(), ==, 3);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 2);

      for (auto r : e->pl().getRoutes()) {
        if (r.route == &l1) TEST(r.direction, ==, 0);
        if (r.route == &l2) TEST(r.direction, ==, 0);
      }
    }
  }

  // ___________________________________________________________________________
  {
    //      2->
    //     a--> b
    // c <----- d
    //     <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    //    <-2   2
    // c ----a-----> d

    a = c->getAdjList().front()->getOtherNd(c);

    TEST(tg.getNds()->size(), ==, 3);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 1);
      TEST(e->pl().getRoutes().begin()->direction, ==, 0);
    }
  }

  // ___________________________________________________________________________
  {
    //      2->     1
    //     a--> b <---|
    // c <----- d --->e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto de = tg.addEdg(d, e, {{{100.0, 0.0}, {200, 0.0}}});
    auto eb = tg.addEdg(e, b, {{{200.0, 0.0}, {100, 10.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    eb->pl().addRoute(&l1, 0);
    de->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    for (auto nd : *tg.getNds()) {
      if (nd->getDeg() == 1 && nd != c) e = nd;
    }

    //    <-2   2        1,<-2
    // c ----a-----> d < --- e

    a = c->getAdjList().front()->getOtherNd(c);

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 2);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 1);
      TEST(e->pl().getRoutes().begin()->direction, ==, 0);
    }

    for (auto ro : e->getAdjList().front()->pl().getRoutes()) {
      if (ro.route == &l1) TEST(ro.direction, ==, 0);
      if (ro.route == &l2) TEST(ro.direction->pl().getGeom()->getX(), ==, approx(100));
    }
  }

  // ___________________________________________________________________________
  {
    //      2->
    //     a--> b
    // c <----- d --->e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto de = tg.addEdg(d, e, {{{100.0, 0.0}, {200, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    de->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();


    a = c->getAdjList().front()->getOtherNd(c);

    //    <-2   2        <-2
    // c ----a-----> d < --- e

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 1);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 1);
      TEST(e->pl().getRoutes().begin()->direction, ==, 0);
    }

    TEST(e->getAdjList().front()->pl().getRoutes().begin()->direction->pl().getGeom()->getX(), ==, approx(100));
  }

  // ___________________________________________________________________________
  {
    //      2->
    //     a--> b
    // c <----- d <---e
    //     <-2    <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});
    auto e = tg.addNd({{200.0, 0.0}});

    auto ab = tg.addEdg(a, b, {{{30.0, 10.0}, {100.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});
    auto ed = tg.addEdg(e, d, {{{200.0, 0.0}, {100, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ab->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);
    ed->pl().addRoute(&l2, d);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();

    a = c->getAdjList().front()->getOtherNd(c);

    //    <-2   2        <-2
    // c ----a-----> d < --- e

    TEST(tg.getNds()->size(), ==, 4);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);
    TEST(e->getAdjList().front()->pl().getRoutes().size(), ==, 1);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 1);
      TEST(e->pl().getRoutes().begin()->direction, ==, 0);
    }

    TEST(e->getAdjList().front()->pl().getRoutes().begin()->direction->pl().getGeom()->getX(), ==, approx(100));
  }

  // ___________________________________________________________________________
  {
    //      2->
    //     a<-- b
    // c <----- d
    //     <-2
    shared::transitgraph::TransitGraph tg;
    auto a = tg.addNd({{30.0, 10.0}});
    auto b = tg.addNd({{100.0, 10.0}});
    auto c = tg.addNd({{0.0, 0.0}});
    auto d = tg.addNd({{100.0, 0.0}});

    auto ba = tg.addEdg(b, a, {{{100.0, 10.0}, {30.0, 10.0}}});
    auto dc = tg.addEdg(d, c, {{{100.0, 0.0}, {0.0, 0.0}}});

    shared::transitgraph::Route l1("1", "1", "red");
    shared::transitgraph::Route l2("2", "2", "blue");

    ba->pl().addRoute(&l2, b);
    dc->pl().addRoute(&l2, c);

    topo::config::TopoConfig cfg;
    cfg.maxAggrDistance = 50;
    cfg.minSegLength = 20;

    topo::MapConstructor mc(&cfg, &tg);
    mc.collapseShrdSegs();
    mc.removeEdgeArtifacts();


    //    <-2   2
    // c ----a-----> d

    // update the node variables, may have changed
    for (auto nd : *tg.getNds()) {
      if (nd->getAdjList().size() == 1) {
        if (nd->getAdjList().front()->pl().getRoutes().begin()->direction == nd) {
          c = nd;
        }
      }
    }

    a = c->getAdjList().front()->getOtherNd(c);

    TEST(a->pl().getGeom()->getX(), ==, approx(30));

    TEST(tg.getNds()->size(), ==, 3);
    TEST(c->getAdjList().front()->pl().getRoutes().size(), ==, 1);
    TEST(c->getAdjList().front()->pl().getRoutes().begin()->direction, ==, c);

    for (auto e : a->getAdjList()) {
      if (e->getOtherNd(a) == c) continue;
      TEST(e->pl().getRoutes().size(), ==, 1);
      TEST(e->pl().getRoutes().begin()->direction, ==, 0);
    }
  }
}
