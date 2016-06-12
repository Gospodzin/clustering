#pragma once
#include <numeric>
#include <algorithm>
#include <vector>
#include <set>

#include "Data.h"
#include "DataSet.h"
#include "utils.h"

struct VaFileDataSet : DataSet {
private:
	typedef int PageId;
	typedef int SegId;
	typedef std::vector<PageId> Seg;

	struct SegBound {
		int beg;
		int end;
	};
	std::vector<SegBound> segBounds;
	std::vector<std::vector<SegBound> > adjacentSegs;

	struct SegWithId {
        SegWithId(Seg seg, SegId segId) : seg(seg), segId(segId) {}
        Seg seg;
		SegId segId;
        bool operator < (const SegWithId &other) const { return segId < other.segId; }
	};


	struct Record {
		Point* p;
		SegId segId;
	};

	std::vector<Record> vaFile;
	std::vector<PointId> positions;

	std::vector<int> divDims;
	std::vector<double> deviations;
	std::vector<int> sortedAttr;
	int dims;
	double eps;
	Point min;
	Point max;
	
	Seg getSeg(Point& p) {
		Seg seg(dims);
		for(int i = 0; i < dims; ++i) seg[i] = (p[divDims[i]] - min[divDims[i]]) / eps;

		return seg;
	}

	void calcDeviations() {
		TS("Calc deviations...");
		deviations = utils::calcMeanDeviations(*data);
		sortedAttr.resize(dimensions());
		for(int i = 0; i < dimensions(); ++i) sortedAttr[i] = i;
		std::sort(sortedAttr.begin(), sortedAttr.end(), [&](int a, int b) -> bool { return deviations[a]>deviations[b]; });
		TP("Deviations calculated");
	}

	
	typedef std::shared_ptr<void> TreePtr;
	typedef std::map<PageId, TreePtr> Node;
	struct Leaf { Leaf(SegId segId) : segId(segId) {}; std::vector<Point*> points; SegId segId; };
	typedef std::shared_ptr<Node> NodePtr;
	typedef std::shared_ptr<Leaf> LeafPtr;

	int getPageId(const Point& p, int dim) const { return (p.at(dim) - min.at(dim)) / eps; }
	int pageExists(Node& node, PageId pageId) { return node.find(pageId) != node.end(); }

	void collectSegments(Node& node, int level, Seg seg, std::set<SegWithId>& segs) {
		for(auto& page : node) {
			Seg nextSeg = seg; nextSeg.emplace_back(page.first);
            if(level == dims - 1) segs.emplace(SegWithId(nextSeg, std::static_pointer_cast<Leaf>(page.second)->segId));
			else collectSegments(*std::static_pointer_cast<Node>(page.second), level + 1, nextSeg, segs);
		}
	}

	void setAdjacentSegs(TreePtr& tree, std::set<SegWithId> segs, std::vector<std::vector<SegId> >& adjacentSegsById) {
		for(SegWithId seg : segs) 
			setAdjacentSegs(*std::static_pointer_cast<Node>(tree), 0, seg.seg, seg.segId, adjacentSegsById[seg.segId]);
	}

	void setAdjacentSegs(Node& node, int level, Seg& seg, SegId& segId, std::vector<SegId>& adjacentSegs) {
		if(level == dims - 1) {
			auto pageIt = node.find(seg[level] - 1); if(pageIt != node.end()) adjacentSegs.emplace_back(std::static_pointer_cast<Leaf>(pageIt->second)->segId);
			pageIt = node.find(seg[level]); if(pageIt != node.end()) adjacentSegs.emplace_back(std::static_pointer_cast<Leaf>(pageIt->second)->segId);
			pageIt = node.find(seg[level] + 1); if(pageIt != node.end()) adjacentSegs.emplace_back(std::static_pointer_cast<Leaf>(pageIt->second)->segId);
		}
		else {
			auto pageIt = node.find(seg[level] - 1); if(pageIt != node.end()) setAdjacentSegs(*std::static_pointer_cast<Node>(pageIt->second), level + 1, seg, segId, adjacentSegs);
			pageIt = node.find(seg[level]); if(pageIt != node.end()) setAdjacentSegs(*std::static_pointer_cast<Node>(pageIt->second), level + 1, seg, segId, adjacentSegs);
			pageIt = node.find(seg[level] + 1); if(pageIt != node.end()) setAdjacentSegs(*std::static_pointer_cast<Node>(pageIt->second), level + 1, seg, segId, adjacentSegs);
		}
	}

	void setAdjacentSegs2(TreePtr& tree, std::vector<std::vector<SegId> >& adjacentSegsById) {
		std::vector<TreePtr> potentials;
		auto node = std::static_pointer_cast<Node>(tree);
		auto subTreeIt = node->begin();
		auto next = node->begin(); ++next;
		while(subTreeIt != node->end()) {
			if(subTreeIt != node->begin()) {
				auto prev = subTreeIt; --prev;
				if(subTreeIt->first - 1 == prev->first)
					potentials.emplace_back(prev->second);
			}
			if(next != node->end()) {
				if(subTreeIt->first + 1 == next->first)
					potentials.emplace_back(next->second);
			}
			potentials.emplace_back(subTreeIt->second);
			setAdjacentSegs2(*std::static_pointer_cast<Node>(subTreeIt->second), 0, potentials);
			potentials.clear();
		}
	}
	 
	void setAdjacentSegs2(Node& node, int level, std::vector<TreePtr>& potentials) {
		std::vector<TreePtr> nextPotentials;
		if(level == dims - 1) {
		}
		else {
			auto subTreeIt = node.begin();
			while(subTreeIt != node.end()) {
				for(auto potential : potentials) {
				}
				setAdjacentSegs2(*std::static_pointer_cast<Node>(subTreeIt->second), level + 1, potentials);
			}
		}
	}

	void builSegTree(TreePtr tree, std::vector<Record>& vaFile) {
		int nextSegId = 0;

		for(Point& p : *data) {
			NodePtr node = std::static_pointer_cast<Node>(tree);
			for(int i = 0; i < dims - 1; ++i) {
				PageId pageId = getPageId(p, divDims[i]);
				if(!pageExists(*node, pageId)) node->emplace(pageId, NodePtr(new Node()));
				node = std::static_pointer_cast<Node>(node->at(pageId));
			}

			PageId pageId = getPageId(p, divDims[dims - 1]);
			if(!pageExists(*node, pageId)) { node->emplace(pageId, LeafPtr(new Leaf(nextSegId++))); }
			LeafPtr leaf = std::static_pointer_cast<Leaf>(node->at(pageId));

			leaf->points.push_back(&p);
			vaFile.push_back({&p, leaf->segId});
		}
	}

    void findSegsBounds(std::vector<SegBound>& segBounds) {
        segBounds[vaFile[0].segId].beg = 0;
		for(int i = 1, last = vaFile[0].segId; i < vaFile.size(); ++i) if(vaFile[i].segId != last) { segBounds[last].end = i; last = vaFile[i].segId; segBounds[last].beg = i; }
		segBounds[vaFile.back().segId].end = vaFile.size();
	}
	
public:
	VaFileDataSet(std::vector<Point>* data, Params params) : DataSet(data, params), dims(params.n > dimensions() ? dimensions() : params.n), eps(params.eps) {
		TS("Creating VaFile...");

		TS("init...");
		// init;
		calcDeviations();
		for(int i = 0; i < dims; ++i) divDims.emplace_back(sortedAttr[i]);

		min = referenceSelectors::min(*data);
		max = referenceSelectors::max(*data);
		TP("...init");

		TS("build seg tree...");
		// build seg tree
		TreePtr tree(new Node());
		builSegTree(tree, vaFile);
		TP("...build seg tree");

		TS("collect segs...");
		// collect segs
		std::set<SegWithId> segs;
		collectSegments(*std::static_pointer_cast<Node>(tree), 0, Seg(), segs);
		TP("...collect segs");

		TS("set adjacent segs...");
		// set adjacent segs
		std::vector<std::vector<SegId> > adjacentSegsById(segs.size());
		setAdjacentSegs(tree, segs, adjacentSegsById);
		TP("...set adjacent segs");

		TS("sort by seg id...");
		// sort by seg id
		std::sort(vaFile.begin(), vaFile.end(), [](Record& r1, Record& r2) -> bool { return r1.segId < r2.segId; });
		TP("...sort by seg id");

		TS("find segs bounds...");
		// find segs bounds
		segBounds.resize(segs.size());
        findSegsBounds(segBounds);
		TP("...find segs bounds");

		TS("set adjacent segs bounds...");
		// set adjacent segs bounds
		adjacentSegs.resize(segs.size());
		for(int id = 0; id < segs.size(); ++id) for(auto& segId : adjacentSegsById[id]) adjacentSegs[id].emplace_back(segBounds[segId]);
		TP("...set adjacent segs bounds");

		TS("sort segs by highest dev dim...");
		// sort segs by highest dev dim
		for(int i = 0; i < segBounds.size(); ++i) std::sort(vaFile.begin() + segBounds[i].beg, vaFile.begin() + segBounds[i].end, [&](Record& r1, Record& r2) -> bool { return r1.p->at(divDims[0]) < r2.p->at(divDims[0]); });
		TP("...sort segs by highest dev dim");

		TS("set vafile position by point id...");
		// set vafile position by point id
		positions.resize(vaFile.size());
		for(int i = 0; i < vaFile.size(); ++i) positions[vaFile[i].p->id] = i;
		TP("...set vafile position by point id");

		TP("VaFile created");
	}

	std::vector<Point*> regionQuery(const Point& target, const double& eps, const std::vector<int>& attrs = {}) const {
		std::vector<Point*> neighbours;

		int pos = positions[target.id];
		SegId targetSegId = vaFile[pos].segId;
		PageId targetPage = getPageId(target, divDims[0]);
		for(auto segBound : adjacentSegs[targetSegId]) {
			PageId page = getPageId(*vaFile[segBound.beg].p, divDims[0]);
			if(page == targetPage)
				for(int i = segBound.beg; i < segBound.end; ++i) {
					if(distance(target, *vaFile[i].p, {}) <= eps)
						neighbours.emplace_back(vaFile[i].p);
				}
			else if(page < targetPage)
				for(int i = segBound.end - 1; i >= segBound.beg; --i) {
					if(target[divDims[0]] - vaFile[i].p->at(divDims[0]) > eps) break;
					if(distance(target, *vaFile[i].p, {}) <= eps)
						neighbours.emplace_back(vaFile[i].p);
				}
			else if(page > targetPage)
				for(int i = segBound.beg; i < segBound.end; ++i) {
					if(target[divDims[0]] - vaFile[i].p->at(divDims[0]) > eps) break;
					if(distance(target, *vaFile[i].p, {}) <= eps)
						neighbours.emplace_back(vaFile[i].p);
				}
		}

		return neighbours;
	}

};

