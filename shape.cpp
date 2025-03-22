/****************************************************
 * Shape recognition
 *
 * @author Felix Touchte Codjo
 * @date March 22, 2025
 * *************************************************/

#include "reader.h"

#include <string>
#include <cstdio>
#include <vector>
#include <cmath>

#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TString.h"
#include "TLegend.h"
#include "TMultiGraph.h"
#include "Math/PdfFuncMathCore.h"

bool is_recognized (std::vector<double> samples, std::vector<double> vx, std::string title, bool verbose = false) {  // vx : corresponding x axis values
	int Npts = samples.size();
	if ((Npts < 1) || ((int) vx.size() != Npts)){
		return false;
	}
	// compute the derived function : df/dx
	std::vector<double> dsamples(Npts, 0.0);
	for (int i = 1; i < Npts; i++) {
		if (vx[i] == vx[i-1]) {
			return false;
		}
		double slope = (samples[i] - samples[i-1])/(vx[i] - vx[i-1]); // devided by 1
		dsamples[i-1] = slope;
	}
	dsamples[Npts-1] = 0.0;
	// find the number of zeros
	int nzeros = 0;
	bool flag_skip_next_itr = false;
	for (int i = 2; i < Npts-1; i++) { // bin 0, 1 and Npts-1 are ignored
		if (flag_skip_next_itr) { 
			flag_skip_next_itr = false; 
			continue;
		}
		if (dsamples[i]*dsamples[i-1] < 0) {
			if (dsamples[i+1]*dsamples[i-2] < 0) { // strong condition
				nzeros++;
				//printf("zero found in bins [%.3lf , %.3lf]\n", vx[i-1], vx[i]);
			}
		}
		else if (dsamples[i]*dsamples[i-1] == 0) {
			if (dsamples[i+1]*dsamples[i-2] < 0) { // strong condition
				nzeros++;
				//printf("zero found in bins [%.3lf , %.3lf]\n", vx[i-1], vx[i]);
			}
			flag_skip_next_itr = true; // useful to not count the same zero two times 
		}
		else {
			// do nothing
		}
	}
	//printf("nzeros : %d\n", nzeros);
	/**************************************************************
	 * at this stage we can ignore all signal for which nzeros > 1
	 * ***********************************************************/

	// Estimate the pedestal ("adcOffset") of the signal with the fisrt bin (convenient with AHDC signals)
	double pedestal = samples[0];
	std::vector<double> samplesCorr(Npts, 0.0);
	for (int i = 0; i < Npts; i++) {
		samplesCorr[i] = std::max(samples[i] - pedestal, 0.0);
	}
	// Determine the peak (not really adcMax)
	double adc_peak = samplesCorr[0];
	//double time_peak = vx[0];
	int bin_peak = 0;
	for (int i = 0; i < Npts; i++) {
		if (adc_peak < samplesCorr[i]) {
			adc_peak = samplesCorr[i];
			//time_peak = vx[i];
			bin_peak = i;
		}
	}
	//printf("time_peak : %.3lf, adc_peak : %.3lf\n", time_peak, adc_peak);
	/**************************************************************
	 * possibility to add a cut on adc_peak
	 * ***********************************************************/
	/*// simple ToT finder
	int bin_tot1 = 0;
	for (int i = 0; i < bin_peak; i++) {
		if (samplesCorr[i] >= 0.5*adc_peak) { // first pass above tot
			bin_tot1 = i;
			break;
		}
	}
	int bin_tot2 = bin_peak;
	for (int i = bin_peak; i < Npts; i++) {
		if (samplesCorr[i] <= 0.5*adc_peak) { // fisrt pass below tot
			bin_tot2 = i;
			break;
		}
	}
	double tot = vx[bin_tot2] - vx[bin_tot1];*/
	//printf("tot : %.3lf\n", tot);
	/********* Mode AHDC *******************/
	float threshold = 0.5*adc_peak;
	// leadingEdgeTime
	int binRise = 0;
	for (int bin = 0; bin < bin_peak; bin++){
		if (samplesCorr[bin] < threshold)
			binRise = bin;  // last pass below threshold and before adcMax
	} // at this stage : binRise < leadingEdgeTime/samplingTime <= binRise + 1 // leadingEdgeTime is determined by assuming a linear fit between binRise and binRise + 1
	float slopeRise = 0;
	if (binRise + 1 <= Npts-1)
		slopeRise = samplesCorr[binRise+1] - samplesCorr[binRise];
	float fittedBinRise = (slopeRise == 0) ? binRise : binRise + (threshold - samplesCorr[binRise])/slopeRise;
	float bin_tot1 = fittedBinRise; // binOffset is determined in wavefromCorrection() // must be the same for all time ? // or must be defined using fittedBinRise*sparseSample

	// trailingEdgeTime
	int binFall = bin_peak;
	for (int bin = bin_peak; bin < Npts; bin++){
		if (samplesCorr[bin] > threshold){
			binFall = bin;
		}
		else {
			binFall = bin;
			break; // first pass below the threshold
		}
	} // at this stage : binFall - 1 <= timeRiseCFA/samplingTime < binFall // trailingEdgeTime is determined by assuming a linear fit between binFall - 1 and binFall
	float slopeFall = 0;
	if (binFall - 1 >= 0)
		slopeFall = samplesCorr[binFall] - samplesCorr[binFall-1];
	float fittedBinFall = (slopeFall == 0) ? binFall : binFall-1 + (threshold - samplesCorr[binFall-1])/slopeFall;
	float bin_tot2 = fittedBinFall;

	// timeOverThreshold
	double tot = bin_tot2 - bin_tot1;
	/**************************************************************
	 * possibility to add a cut on tot
	 * ***********************************************************/

	// Visualization
	bool criteria = (nzeros  <= 1) && (tot >= 7) && (adc_peak >= 200);
	
	if (verbose && criteria) {
		TCanvas* canvas1 = new TCanvas(title.c_str(),title.c_str(),1400, 800);
		TMultiGraph  *mg  = new TMultiGraph();
		TLegend* legend = new TLegend(0.7, 0.7, 0.95, 0.95);
		TGraph * g = new TGraph(Npts);
		TGraph * dg = new TGraph(Npts);
		for (int i = 0; i < Npts; i++) {
			g->SetPoint(i, vx[i], samples[i]);
			dg->SetPoint(i, vx[i], dsamples[i]);
		}
		g->SetLineColor(2);
		dg->SetLineColor(3);
		mg->SetTitle(TString::Format("%s, nzeros : %d, ToT : %.4lf, adc_peak : %.4lf", title.c_str(), nzeros, tot, adc_peak).Data());
		mg->Add(g);
		mg->Add(dg);
		mg->Draw("APL");
		legend->AddEntry(g,"f(x)","l");
		legend->AddEntry(dg,"df/dx","l");
		legend->Draw();
		canvas1->Print(title.c_str());
	}
	// output
	return criteria;
}

void test1() {
	printf("===== Test 1 =====\n");
	int Npts = 50;
	std::vector<double> samples(Npts, 0.0);
	std::vector<double> vx(Npts, 0.0);
	for (int i = 0; i < Npts; i++) {
		vx[i] = i;
		samples[i] = ROOT::Math::landau_pdf(vx[i], 3, 10.0); // x, width, central value
	}
	if (is_recognized(samples, vx, "test_landau.pdf", true)) {
		printf("\033[32m > Congratulation, this a signal !!!!!\n\033[0m");
	}
	else {
		printf("\033[31m > Signal not recongized :-( \n\033[0m");
	}
};

void test2() {
	printf("===== Test 2 =====\n");
	int Npts = 50;
	std::vector<double> samples(Npts, 0.0);
	std::vector<double> vx(Npts, 0.0);
	for (int i = 0; i < Npts; i++) {
		vx[i] = (2*M_PI*i)/Npts;
		samples[i] = sin(vx[i]);
	}
	if (is_recognized(samples, vx, "test_sinus.pdf", true)) {
		printf("\033[32m > Congratulation, this a signal !!!!!\n\033[0m");
	}
	else {
		printf("\033[31m > Signal not recongized :-( \n\033[0m");
	}
};

int main(int argc, char const *argv[]){
	test1();
	test2();
	
	if (argc < 2) {
		printf("Please, provide a filename...\n");
		return 0;
	}

	// open file and read bank
	const char* filename = argv[1];
	hipo::reader  reader(filename);
	hipo::banklist banklist = reader.getBanks({"AHDC::adc","AHDC::wf"});
	long unsigned int nEvent = 0;
	long unsigned int nSignals = 0;
	// loop over events
	while( reader.next(banklist)){
		if (nEvent % 1000 == 0) {
			printf("Begin EVENT %ld\n", nEvent);
		}
		if (nEvent > 10000) { break;} // process only 20k events
		for(int col = 0; col < banklist[1].getRows(); col++){ // loop over columns of AHDC::wf 
			int layer = banklist[1].getInt("layer", col);
			int component = banklist[1].getInt("component", col);
			std::vector<double> samples;
			for (int bin = 1; bin <= 50; bin++){
				char buffer[50];
				sprintf(buffer, "s%d", bin); 
				short value = banklist[1].getInt(buffer,col);
				samples.push_back((double) value);
			}
			std::vector<double> vx((int) samples.size(), 0.0);
			for (int i = 0; i < (int) samples.size(); i++) {
				vx[i] = i;
			}

			char buffer[50];
			sprintf(buffer, "./output/cosmics_%ld_%d_%d.png", nEvent+1, layer, component); 
			if (is_recognized(samples, vx, buffer, true)) {
				nSignals++;
				printf("Event : %4ld, layer : %d, component : %d\n", nEvent+1, layer, component);
			}
		}
		nEvent++;
	}
	printf("nSignals : %ld\n", nSignals);
	return 0;
}

