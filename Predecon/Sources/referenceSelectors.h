#pragma once
#include "Point.h"
#include <functional>
#include "measures.h"

namespace referenceSelectors {
	typedef  std::function< Point(std::vector<Point>&) >  ReferenceSelector;

	Point max(std::vector<Point>& data);
	Point min(std::vector<Point>& data);
	ReferenceSelector getMinMax(int k, measures::MeasureId measureId);
}