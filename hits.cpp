/****************************************************
 * Cosmics
 *
 * @author Felix Touchte Codjo
 * @date March 30, 2025
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
	
	if (argc >= 1) { 
		// open file and read bank
		const char* filename = argv[1];
		
		hipo::reader  reader(filename);
		hipo::banklist banklist = reader.getBanks({"AHDC::adc", "AHDC::wf"});
		long unsigned int nEvent = 0;
		// loop over events
		while( reader.next(banklist)){
			//printf(" ======= EVENT %ld =========\n", nEvent);
			if (nEvent > 20000) { break;} // process only 20k events
			bool flag11 = false, flag21 = false, flag22 = false, flag31 = false, flag32 = false, flag41 = false, flag42 = false, flag51 = false; 	
			int nhits = 0;
			for(int col = 0; col < banklist[0].getRows(); col++){ // loop over columns of the bankname
				int layer = banklist[0].getInt("layer", col);
				if      (layer == 11) { flag11 = true; nhits++;}
				else if (layer == 21) { flag21 = true; nhits++;}
				else if (layer == 22) { flag22 = true; nhits++;}
				else if (layer == 31) { flag31 = true; nhits++;}
				else if (layer == 32) { flag32 = true; nhits++;}
				else if (layer == 41) { flag41 = true; nhits++;}
				else if (layer == 42) { flag42 = true; nhits++;}
				else if (layer == 51) { flag51 = true; nhits++;}
				else                  { } // do nothing
			}
			nEvent++;
			if (flag11 && flag21 && flag22 && flag31 && flag32 && flag41 && flag42 && flag51) {
			//if (nhits > 6) {
				printf(" ---> nEvent : %ld\n", nEvent);
			}
		}
	}
	else { 
		printf("Please, provide a filename...\n");
	}
}
