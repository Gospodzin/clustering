#include "referenceSelectors.h"

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
