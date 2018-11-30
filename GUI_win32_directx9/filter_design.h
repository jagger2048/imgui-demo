#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
const std::map<std::string, int> FILTER_TYPE{ { "lpf",1 },{ "hpf",2 } ,{ "apf",3 },{ "bpf",4 },{ "notch",5 } ,{ "peakingEQ",6 },{ "lowShelf",7 },{ "highShelf",8 } };
class FILTER_DESIGN
{
	public:
		std::string filter_type = "lpf";
		float f0 = 1000;
		float fs = 48e3;
		float Q = 0.707;
		float Boost = 1;
		float BW = 1;					// % var2 = B, in Hz
		float S = 1;
		double pi = 3.14159265;
		double w0 = 0;
		float a0, a1, a2, b0, b1, b2;
		double alpha = 1;
		double A = 1;
		void show() {
			std::cout << "test" << std::endl;
		};
	std::vector<std::vector<float>>  design();
	FILTER_DESIGN() {};
	virtual ~FILTER_DESIGN() {};
};
