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
	//hipo::banklist banklist = reader.getBanks({"AHDC::adc","AHDC::wf"});
	hipo::banklist banklist = reader.getBanks({"AHDC::adc"});
	long unsigned int nEvent = 0;

	TH1D* hist1d_time = new TH1D("hist1d_time", "hist1d_time", 100, 0, 5000);	
	TH1D* hist1d_leadingEdgeTime = new TH1D("hist1d_leadingEdgeTime", "hist1d_leadingEdgeTime", 100, 0, 5000);	
	// loop over events
	while( reader.next(banklist)){
		//printf(" ======= EVENT %ld =========\n", nEvent);
		if (nEvent > 20000) { break;} // process only 20k events
		for(int col = 0; col < banklist[0].getRows(); col++){ // loop over columns of AHDC::wf 
			// AHDC::wf --> samples
			//int layer = banklist[1].getInt("layer", col);
			//int component = banklist[1].getInt("component", col);
			int time = banklist[0].getInt("time", col);
			//long timestamp = banklist[1].getLong("timestamp", col);
			//printf("%d  ", time);
			//printf("%ld  ", timestamp);
			// AHDC::adc  --> decoded outputs
			//double adcMax = banklist[0].getInt("ADC", col);
			//double integral = banklist[0].getInt("integral", col);
			//double adcOffset = banklist[0].getInt("adcOffset", col);
			//double timeMax = banklist[0].getInt("time", col);
			//double leadingEdgeTime = banklist[0].getFloat("leadingEdgeTime", col)/50.0;
			//double timeOT = banklist[0].getInt("timeOverThreshold", col);
			//double timeCFD = banklist[0].getInt("constantFractionTime", col);
			hist1d_time->Fill(time);
			//hist1d_leadingEdgeTime->Fill(leadingEdgeTime);
		}
		nEvent++;
	}
	TCanvas* canvas1 = new TCanvas("c1","c1 title",1200, 800);
	gStyle->SetOptStat("nemruo");
	//canvas1->Divide(2,1);
	//canvas1->cd(1);
	hist1d_time->GetXaxis()->SetTitle("time");
        hist1d_time->GetXaxis()->SetTitleSize(0.05);
        hist1d_time->GetYaxis()->SetTitle("count");
        hist1d_time->GetYaxis()->SetTitleSize(0.05);
        hist1d_time->Draw();
	/*canvas1->cd(2);
	hist1d_leadingEdgeTime->GetXaxis()->SetTitle("leadingEdgeTime");
        hist1d_leadingEdgeTime->GetXaxis()->SetTitleSize(0.05);
        hist1d_leadingEdgeTime->GetYaxis()->SetTitle("count");
        hist1d_leadingEdgeTime->GetYaxis()->SetTitleSize(0.05);
        hist1d_leadingEdgeTime->Draw();
	canvas1->Print("./time.pdf");*/
	delete hist1d_time;
	delete canvas1;
}
