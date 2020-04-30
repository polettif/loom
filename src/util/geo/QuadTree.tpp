// _____________________________________________________________________________
template <typename V, typename T>
QuadTree<V, T>::QuadTree(size_t d, const Box<T>& bbox) : _maxDepth(d) {
  _nds.push_back(QuadNode<T>{0, 0, bbox});
}

// _____________________________________________________________________________
template <typename V, typename T>
void QuadTree<V, T>::insert(const V& val, const Point<T>& pos) {
  if (!intersects(pos, _nds[0].bbox)) return;
  int64_t valId = _vals.size();
  _vals.push_back(QuadValue<V, T>{val, pos, -1});
  _vals[valId].nextValue = -1;
  insert(valId, 0);
}

// _____________________________________________________________________________
template <typename V, typename T>
void QuadTree<V, T>::insert(int64_t vid, int64_t nid) {
  if (!intersects(_vals[vid].point, _nds[nid].bbox)) return;

  if (shouldSplit(_nds[nid], _vals[vid])) {
    split(nid);
  }

  if (_nds[nid].numEls == -1) {
    // insert into fitting subtree
    for (size_t i = 0; i < 4; i++) insert(vid, _nds[nid].childs + i);
  } else {
    if (_nds[nid].numEls == 0) {
      _nds[nid].childs = vid;
    } else {
      _vals[vid].nextValue = _nds[nid].childs;
      _nds[nid].childs = vid;
    }
    _nds[nid].numEls++;
  }
}

// _____________________________________________________________________________
template <typename V, typename T>
void QuadTree<V, T>::split(size_t nid) {
  const auto& box = _nds[nid].bbox;
  T w = (box.getUpperRight().getX() - box.getLowerLeft().getX()) / T(2);

  int64_t curEl = _nds[nid].childs;

  _nds[nid].numEls = -1;           // the node is now a leaf node
  _nds[nid].childs = _nds.size();  // the nodes quadrant block starts here

  // box at 0, 0
  _nds.push_back(QuadNode<T>{
      0, 0,
      Box<T>(box.getLowerLeft(), Point<T>(box.getLowerLeft().getX() + w,
                                          box.getLowerLeft().getY() + w))});
  // box at 0, 1
  _nds.push_back(QuadNode<T>{
      0, 0,
      Box<T>(Point<T>(box.getLowerLeft().getX() + w, box.getLowerLeft().getY()),
             Point<T>(box.getUpperRight().getX(),
                      box.getLowerLeft().getY() + w))});
  // box at 1,0
  _nds.push_back(QuadNode<T>{
      0, 0,
      Box<T>(Point<T>(box.getLowerLeft().getX(), box.getLowerLeft().getY() + w),
             Point<T>(box.getLowerLeft().getX() + w,
                      box.getUpperRight().getY()))});
  // box at 1,1
  _nds.push_back(QuadNode<T>{0, 0,
                             Box<T>(Point<T>(box.getLowerLeft().getX() + w,
                                             box.getLowerLeft().getY() + w),
                                    box.getUpperRight())});

  while (curEl > -1) {
    _vals[curEl].nextValue = -1;
    insert(curEl, nid);
    curEl = _vals[curEl].nextValue;
  }
}

// _____________________________________________________________________________
template <typename V, typename T>
bool QuadTree<V, T>::shouldSplit(const QuadNode<T>& nd,
                                 const QuadValue<V, T>& newVal) const {
  // if (nd.numEls + 1 > 4) return true;
  if (nd.numEls + 1 > 1) return true;
  return false;
}

// _____________________________________________________________________________
template <typename V, typename T>
size_t QuadTree<V, T>::size() const {
  return _vals.size();
}

// _____________________________________________________________________________
template <typename V, typename T>
const QuadNode<T>& QuadTree<V, T>::getNd(size_t nid) const {
  return _nds[nid];
}

// _____________________________________________________________________________
template <typename V, typename T>
const std::vector<QuadNode<T>>& QuadTree<V, T>::getNds() const {
  return _nds;
}
