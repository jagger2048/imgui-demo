#include "filter_design.h"



inline std::vector<std::vector<float>>  FILTER_DESIGN::design() {
	std::cout << filter_type +" design:\n" << std::endl;
	auto iter = FILTER_TYPE.find(filter_type);
	if (iter == FILTER_TYPE.end()) {
		std::cout << "Invail filter type\n";
		std::vector<std::vector<float>> nl;
		return nl;
	}
	else {
		w0 = 2 * pi*f0 / fs;
		switch (iter->second) {
			case 1:		// lpf
				alpha = sin(w0) / (2 * Q);
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				b0 = (1 - cos(w0)) / 2;
				b1 = 1 - cos(w0);
				b2 = (1 - cos(w0)) / 2;
				break;
			case 2:		//'hpf':
				alpha = sin(w0) / (2 * Q);
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				b0 = (1 + cos(w0)) / 2;
				b1 = -(1 + cos(w0));
				b2 = (1 + cos(w0)) / 2;
				break;
			case 3://'apf':
				alpha = sin(w0) / (2 * Q);
				b0 = 1 - alpha;
				b1 = -2 * cos(w0);
				b2 = 1 + alpha;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 4://'bpf':// % Q = 1时是特殊情况
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0)); //% BW
				b0 = Q * alpha;
				b1 = 0;
				b2 = -Q * alpha;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 5://'notch':
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0)); //% BW
				b0 = 1;
				b1 = -2 * cos(w0);
				b2 = 1;
				a0 = 1 + alpha;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha;
				break;

			case 6://'peakingEQ': //% paramatric ， Boost 为中心频率的增益

				   //% %         BW = 2 * pi*B / fs;
				   //%         f0 = 1000;
				   //%         fs = 48e3;
				   //%         Boost = 3;
				   //%         BW = 1;
				   //%         w0 = 2 * pi*f0. / fs;
				A = pow(10, (Boost / 40));
				alpha = sin(w0)*sinh(log(2) / 2 * BW * w0 / sin(w0));
				//%         alpha = sin(w0) / (2 * Q); % 简化版，Q为 0.07~15
				//	% BW； 1 / Q = sinh(log(2) / 2 * BW * w0 / sin(w0)) % !!!
				//	%     %so Q = 1 / sinh(log(2) / 2 * BW * w0 / sin(w0)); % Q与BW成反比
				b0 = 1 + alpha * A;
				b1 = -2 * cos(w0);
				b2 = 1 - alpha * A;
				a0 = 1 + alpha / A;
				a1 = -2 * cos(w0);
				a2 = 1 - alpha / A;
				break;


			case 7://'lowShelf':
				A = pow(10, (Boost / 40));
				alpha = sin(w0) / 2 * sqrt((A + 1 / A)*(1 / S - 1) + 2);	// % S
																			//	% 1 / Q = sqrt((A + 1 / A)*(1 / S - 1) + 2), S = 1，Q = sqrt(2) / 2
				b0 = A * ((A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha);
				b1 = 2 * A*((A - 1) - (A + 1)*cos(w0));
				b2 = A * ((A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha);
				a0 = (A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
				a1 = -2 * ((A - 1) + (A + 1)*cos(w0));
				a2 = (A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha;
				break;

			case 8://'highShelf':
				A = pow(10, (Boost / 40.0));
				alpha = sin(w0) / 2.0 * sqrt((A + 1.0 / A)*(1.0 / S - 1) + 2);// % S
				b0 = A * ((A + 1) + (A - 1)*cos(w0) + 2 * sqrt(A)*alpha);
				b1 = -2 * A*((A - 1) + (A + 1)*cos(w0));
				b2 = A * ((A + 1) + (A - 1)*cos(w0) - 2 * sqrt(A)*alpha);
				a0 = (A + 1) - (A - 1)*cos(w0) + 2 * sqrt(A)*alpha;
				a1 = 2 * ((A - 1) - (A + 1)*cos(w0));
				a2 = (A + 1) - (A - 1)*cos(w0) - 2 * sqrt(A)*alpha;
			default:
				break;
		}
	}
	return	std::vector<std::vector<float>>{ {b0 / a0, b1 / a0, b2 / a0}, { a0 / a0, a1 / a0, a2 / a0 }};
	//return NULL;
}

