#include "stdafx.h"
#include "CppUnitTest.h"
#include <fstream>
#include <memory>
#include "DataLoader.h"
#include "TIDataSet.h"
#include <codecvt>
#include "Predecon.h"
#include "DataWriter.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace Microsoft
{
	namespace VisualStudio
	{
		namespace CppUnitTestFramework
		{
			template<> static std::wstring ToString<Point>(const Point &t)
			{
				std::stringstream ss;
				ss << "id: " << t.id << " " << "[" << t[0];
				for (int i = 1; i < t.size(); i++)
					ss << " " << t[i];
				ss << "]";
				return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(ss.str().c_str());
			}
		}
	}
}


namespace Testing
{		
	TEST_CLASS(UnitTest1)
	{
	public:
		std::string path = "../test_data/";

		
		TEST_METHOD(fileOpen)
		{
			std::ifstream file(path + "test.txt");
			Assert::IsTrue(file.is_open());
			file.close();
		}

		TEST_METHOD(loadData)
		{
			std::vector<Point>* ds = DataLoader(path + "test.txt").load();
			Assert::AreEqual(3, (int)ds->size());
			Assert::AreEqual(2, (int)ds->at(0).size());
			Assert::AreEqual(4.2, ds->at(1)[1]);
		}

		TEST_METHOD(euclideanDist)
		{
			Point rel1(2, 0);
			rel1.push_back(1);
			rel1.push_back(2);
			Point rel2(2, 0);
			rel2.push_back(3);
			rel2.push_back(4);
			Assert::AreEqual(sqrt(8.0), measures::euclideanDistance(rel1, rel2));
		}

		TEST_METHOD(manhattanDist)
		{
			Point rel1(2, 0);
			rel1.push_back(1);
			rel1.push_back(2);
			Point rel2(2, 0);
			rel2.push_back(3);
			rel2.push_back(4);
			Assert::AreEqual(4.0, measures::manhattanDistance(rel1, rel2));
		}

		TEST_METHOD(sortTIData)
		{
			std::vector<Point>* ds = DataLoader(path + "test.txt").load();
			Point rel(2, 0);
			rel.push_back(4.2);
			rel.push_back(5.2);
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Assert::AreEqual(1, ti.sortedData[0]->id);
			Assert::AreEqual(2, ti.sortedData[1]->id);
			Assert::AreEqual(0, ti.sortedData[2]->id);
		}
		
		TEST_METHOD(tICalcNeighbourhoods)
		{
			std::vector<Point>* ds = DataLoader(path + "test2.txt").load();
			Point rel = ds->at(6); // 3 1
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Predecon<TIDataSet> predecon(&ti, measures::euclideanDistance, 2.3, 3, -1.0, 999999999);
			auto nbh = predecon.neighbourhoods[6];
			Point* n1 = ti.sortedData[3]; // 2 3
			Point* n2 = ti.sortedData[1]; // 2 1
			Point* n3 = ti.sortedData[2]; // 3 2
			Point* n4 = ti.sortedData[0]; // 3 1

			Assert::AreEqual(4, (int)nbh.size());
			Assert::IsFalse(std::find(nbh.begin(), nbh.end(), n1) == nbh.end());
			Assert::IsFalse(std::find(nbh.begin(), nbh.end(), n2) == nbh.end());
			Assert::IsFalse(std::find(nbh.begin(), nbh.end(), n3) == nbh.end());
			Assert::IsFalse(std::find(nbh.begin(), nbh.end(), n4) == nbh.end());
		}


		TEST_METHOD(tICalcVariances)
		{
			std::vector<Point>* ds = DataLoader(path + "test2.txt").load();
			Point rel = ds->at(6); // 3 1
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Predecon<TIDataSet> predecon(&ti, measures::euclideanDistance, 2.3, 3, 1, 1);
			std::vector<double> variances = predecon.allVariances[6];

			Assert::AreEqual((int)variances.size(), 2);
			Assert::AreEqual(variances[0], 0.5);
			Assert::AreEqual(variances[1], 1.25);
		}

		TEST_METHOD(tICalcPDims)
		{
			std::vector<Point>* ds = DataLoader(path + "test2.txt").load();
			Point rel = ds->at(6); // 3 1
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Predecon<TIDataSet> predecon(&ti, measures::euclideanDistance, 2.3, 3, 1, 1);

			Assert::AreEqual(predecon.pDims[6], 1);
		}

		TEST_METHOD(tICalcPrefVectors)
		{
			std::vector<Point>* ds = DataLoader(path + "test2.txt").load();
			Point rel = ds->at(6); // 3 1
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Predecon<TIDataSet> predecon(&ti, measures::euclideanDistance, 2.3, 3, 1, 1);

			Assert::AreNotEqual(predecon.prefVectors[6][0], 1.0);
			Assert::AreEqual(predecon.prefVectors[6][1], 1.0);
		}

		TEST_METHOD(predecon)
		{
			std::vector<Point>* ds = DataLoader(path + "test2.txt").load();
			Point rel = ds->at(6); // 3 1
			TIDataSet ti(ds, measures::euclideanDistance, rel);
			Predecon<TIDataSet> predecon(&ti, measures::euclideanDistance, 1.1, 3, -1.0, 999999999);
			predecon.compute();
			DataWriter("out.txt").write(ds);
		}

	};
}