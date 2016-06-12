#include "referenceSelectors.h"
#include <algorithm>
#include "utils.h"

Point referenceSelectors::max(std::vector<Point>& data) {
	Point reference = data[0];
	for (Point& p : data)
		for (int i = 0; i < (int)p.size(); ++i)
			if (p[i] > reference[i])
				reference[i] = p[i];

	return reference;
}

Point referenceSelectors::min(std::vector<Point>& data) {
	Point reference = data[0];
	for (Point& p : data)
		for (int i = 0; i < (int)p.size(); ++i)
			if (p[i] < reference[i])
				reference[i] = p[i];

	return reference;
}

referenceSelectors::ReferenceSelector referenceSelectors::getMinMax(int k, measures::MeasureId measureId) {
	return [=](std::vector<Point>& data)->Point{
		measures::Measure measure = measures::getMeasure(measureId);

		std::vector<Point> candidates;
		for (int i = 0; i < k; ++i) {
			Point reference(data.front().size());
			std::vector<int> signs(data.front().size());
			for (int i = 0; i < signs.size(); ++i)
				signs[i] = rand() % 2 == 0 ? 1 : -1;
			for (Point& p : data)
				for (int i = 0; i < (int)p.size(); ++i)
					if (p[i] * signs[i] > reference[i] * signs[i])
						reference[i] = p[i];
			candidates.emplace_back(reference);
		}

		Point reference = *std::max_element(candidates.begin(), candidates.end(), [&](Point& r1, Point& r2){
			return utils::calcM(r1, data, measure) < utils::calcM(r2, data, measure);
		});

		return reference; };
}