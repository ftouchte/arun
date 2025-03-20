/****************************************************
 * Study of the outer layer noise (L. 42 and 51)
 *
 * @author Felix Touchte Codjo
 * @date March 19, 2025
 * *************************************************/

#include "reader.h"

#include <string>
#include <cstdio>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TString.h"


int main(int argc, char const *argv[]){
	
	if (argc < 2) {
		printf("Please, provide a filename...\n");
		return 0;
	}

	// open file and read bank
	const char* filename = argv[1];
	hipo::reader  reader(filename);
	hipo::banklist banklist = reader.getBanks({"AHDC::adc","AHDC::wf"});
	long unsigned int nEvent = 0;
	
	long unsigned int nEvent_full = 0;
	long unsigned int nEvent_semi = 0;
	long unsigned int nEvent_semi_semi = 0;

	// loop over events
	while( reader.next(banklist)){
		//printf(" ======= EVENT %ld =========\n", nEvent);
		if (nEvent > 20000) { break;} // process only 20k events
		int nhit_51 = 0;
		int nhit_42 = 0;
		for(int col = 0; col < banklist[1].getRows(); col++){ // loop over columns of AHDC::wf 
			// AHDC::wf --> samples
			int layer = banklist[1].getInt("layer", col);
			//int component = banklist[1].getInt("component", col);
			// find the end the waveform (in case of Zero Suppress)
			/*int nsamples = 50;
			for (int bin = 1; bin <= nsamples; bin++){
				char buffer[50];
				sprintf(buffer, "s%d", bin); 
				short value = banklist[1].getInt(buffer,col);
				rms += value*value;
			}*/
			if (layer == 51) {
				nhit_51++;
			}
			else if (layer == 42) {
				nhit_42++;
			}
			/*else if (layer == 41) {
			}
			else if (layer == 32) {
			}
			else if (layer == 31) {
			}
			else if (layer == 22) {
			}
			else if (layer == 21) {
			}
			else if (layer == 11) {
			}*/
			else {
				// do nothing
			}
			// AHDC::adc  --> decoded outputs
			/*double adcMax = banklist[0].getInt("ADC", col);
			double integral = banklist[0].getInt("integral", col);
			double adcOffset = banklist[0].getInt("adcOffset", col);
			double timeMax = banklist[0].getInt("time", col);
			double timeRiseCFA = banklist[0].getInt("leadingEdgeTime", col);
			double timeOT = banklist[0].getInt("timeOverThreshold", col);
			double timeCFD = banklist[0].getInt("constantFractionTime", col);*/
		}
		int nhit = nhit_51 + nhit_42;
		if (nhit > 150) { // 99 + 87 == 186
			nEvent_full++;
			printf("\033[31m   > nEvent : %4.0ld, nhit : %3.0d, nhit_51 : %3.0d, nhit_42 : %3.0d\n\033[0m", nEvent+1, nhit, nhit_51, nhit_42);
		}
		else if ((nhit > 80) && (nhit <= 150)) {
			nEvent_semi++;
			printf("\033[33m   > nEvent : %4.0ld, nhit : %3.0d, nhit_51 : %3.0d, nhit_42 : %3.0d\n\033[0m", nEvent+1, nhit, nhit_51, nhit_42);
		}
		else if ((nhit > 20) && (nhit <= 80)) {
			nEvent_semi_semi++;
			printf("\033[32m   > nEvent : %4.0ld, nhit : %3.0d, nhit_51 : %3.0d, nhit_42 : %3.0d\n\033[0m", nEvent+1, nhit, nhit_51, nhit_42);
		}
		else {
			// do nothing
		}
		nEvent++;
	}
	printf("\033[31m nEvent_full       : %ld\n\033[0m", nEvent_full);
	printf("\033[33m nEvent_semi       : %ld\n\033[0m", nEvent_semi);
	printf("\033[32m nEvent_semi_semi  : %ld\n\033[0m", nEvent_semi_semi);
}
