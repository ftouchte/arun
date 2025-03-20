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
		printf("Please, provide a filename...");
		return 0;
	}

	// open file and read bank
	const char* filename = argv[1];
	hipo::reader  reader(filename);
	hipo::banklist banklist = reader.getBanks({"AHDC::adc","AHDC::wf"});
	long unsigned int nEvent = 0;
	
	TH1D* hist1d_rms1 = new TH1D("hist1d_rms1", "RMS signals in Layer 1", 100, 0, 500);
	TH1D* hist1d_rms2 = new TH1D("hist1d_rms2", "RMS signals in Layer 2", 100, 0, 500);
	TH1D* hist1d_rms3 = new TH1D("hist1d_rms3", "RMS signals in Layer 3", 100, 0, 500);
	TH1D* hist1d_rms4 = new TH1D("hist1d_rms4", "RMS signals in Layer 4", 100, 0, 500);
	TH1D* hist1d_rms5 = new TH1D("hist1d_rms5", "RMS signals in Layer 5", 100, 0, 500);
	TH1D* hist1d_rms6 = new TH1D("hist1d_rms6", "RMS signals in Layer 6", 100, 0, 500);
	TH1D* hist1d_rms7 = new TH1D("hist1d_rms7", "RMS signals in Layer 7", 100, 0, 500);
	TH1D* hist1d_rms8 = new TH1D("hist1d_rms8", "RMS signals in Layer 8", 100, 0, 500);

	// loop over events
	while( reader.next(banklist)){
		//printf(" ======= EVENT %ld =========\n", nEvent);
		for(int col = 0; col < banklist[1].getRows(); col++){ // loop over columns of AHDC::wf 
			// AHDC::wf --> samples
			int layer = banklist[1].getInt("layer", col);
			//int component = banklist[1].getInt("component", col);
			// find the end the waveform (in case of Zero Suppress)
			int nsamples = 50;
			for (int bin = 50; bin >= 1; bin--){
				char buffer[50];
				sprintf(buffer, "s%d", bin); 
				short value = banklist[1].getInt(buffer,col);
				if (value != 0) { 
					nsamples = bin;
					break;
				}
			}
			double rms = 0.0;
			for (int bin = 1; bin <= nsamples; bin++){
				char buffer[50];
				sprintf(buffer, "s%d", bin); 
				short value = banklist[1].getInt(buffer,col);
				rms += value*value;
			}
			rms = sqrt(rms/nsamples);
			if (layer == 51) {
				//printf("   > layer : %2.0d , wire : %2.0d, nsamples : %2.0d , rms : %lf\n", layer, component, nsamples, rms);
			       	hist1d_rms8->Fill(rms);	
			}
			else if (layer == 42) {
				hist1d_rms7->Fill(rms);
			}
			else if (layer == 41) {
				hist1d_rms6->Fill(rms);
			}
			else if (layer == 32) {
				hist1d_rms5->Fill(rms);
			}
			else if (layer == 31) {
				hist1d_rms4->Fill(rms);
			}
			else if (layer == 22) {
				hist1d_rms3->Fill(rms);
			}
			else if (layer == 21) {
				hist1d_rms2->Fill(rms);
			}
			else if (layer == 11) {
				hist1d_rms1->Fill(rms);
			}
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
		nEvent++;
	}
	TCanvas* canvas1 = new TCanvas("c1","c1 title",1400, 800);
	canvas1->Divide(4,2);
	gStyle->SetOptStat("nemruo");
	canvas1->cd(1);
	hist1d_rms1->GetXaxis()->SetTitle("RMS");
        hist1d_rms1->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms1->GetYaxis()->SetTitle("Count");
        hist1d_rms1->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms1->Draw();
	canvas1->cd(2);
	hist1d_rms2->GetXaxis()->SetTitle("RMS");
        hist1d_rms2->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms2->GetYaxis()->SetTitle("Count");
        hist1d_rms2->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms2->Draw();
	canvas1->cd(3);
	hist1d_rms3->GetXaxis()->SetTitle("RMS");
        hist1d_rms3->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms3->GetYaxis()->SetTitle("Count");
        hist1d_rms3->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms3->Draw();
	canvas1->cd(4);
	hist1d_rms4->GetXaxis()->SetTitle("RMS");
        hist1d_rms4->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms4->GetYaxis()->SetTitle("Count");
        hist1d_rms4->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms4->Draw();
	canvas1->cd(5);
	hist1d_rms5->GetXaxis()->SetTitle("RMS");
        hist1d_rms5->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms5->GetYaxis()->SetTitle("Count");
        hist1d_rms5->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms4->Draw();
	canvas1->cd(6);
	hist1d_rms6->GetXaxis()->SetTitle("RMS");
        hist1d_rms6->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms6->GetYaxis()->SetTitle("Count");
        hist1d_rms6->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms6->Draw();
	canvas1->cd(7);
	hist1d_rms7->GetXaxis()->SetTitle("RMS");
        hist1d_rms7->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms7->GetYaxis()->SetTitle("Count");
        hist1d_rms7->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms7->Draw();
	canvas1->cd(8);
	hist1d_rms8->GetXaxis()->SetTitle("RMS");
        hist1d_rms8->GetXaxis()->SetTitleSize(0.05);
        hist1d_rms8->GetYaxis()->SetTitle("Count");
        hist1d_rms8->GetYaxis()->SetTitleSize(0.05);
        hist1d_rms8->Draw();
	
	canvas1->Print("./rms.pdf");
	delete hist1d_rms1;
	delete hist1d_rms2;
	delete hist1d_rms3;
	delete hist1d_rms4;
	delete hist1d_rms5;
	delete hist1d_rms6;
	delete hist1d_rms7;
	delete hist1d_rms8;
	delete canvas1;

}
