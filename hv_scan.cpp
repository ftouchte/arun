/**************************************
 *
 * @author Felix Touchte Codjo
 * @date March 24, 2025
 * ***********************************/

#include <cstdio>
#include <vector>
#include <cmath>

#include <cairommconfig.h>
#include <cairomm/context.h>
#include <cairomm/surface.h>

#include "fCanvas.h"

int main(int argc, char const *argv[]) {
	const char *filename = "hv_scan.txt";
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file\n");
		return 1;
	}
	std::vector<double> vec_id, vec_run, vec_hv, vec_nwfs;
	double id, run, hv, nwfs;
	int Npts = 0;
	while (fscanf(file, "%lf %lf %lf %lf\n", &id, &run, &hv, &nwfs ) == 4) {
		Npts++;
		vec_id.push_back(id);
		vec_run.push_back(run);
		vec_hv.push_back(hv);
		vec_nwfs.push_back(nwfs);
		printf("%5.0lf %5.0lf %5.0lf %5.0lf\n", id, run, hv, nwfs);
	}
	if (Npts < 1) { return -1;}
	double xmin = vec_hv[0];
	double xmax = vec_hv[vec_hv.size()-1];
	double ymin = vec_nwfs[0], ymax = vec_nwfs[0];
	for (int i = 0; i < (int) Npts; i++){
		ymin = (ymin < vec_nwfs[i]) ? ymin : vec_nwfs[i];
		ymax = (ymax > vec_nwfs[i]) ? ymax : vec_nwfs[i];
	}
	// Define cairo object
	int width = 1400;
	int height = 800;
	auto surface = Cairo::PdfSurface::create("hv_scan.pdf", width, height);
	auto cr = Cairo::Context::create(surface);
	// Create a canvas in cr
	fCanvas canvas(width, height, xmin, xmax, ymin, ymax);
	int window_size = std::min(width,height);
	//canvas.set_top_margin(0.05*window_size);
	//canvas.set_bottom_margin(0.20*window_size);
	//canvas.set_left_margin(0.05*window_size);
	//canvas.set_right_margin(0.15*window_size);
	canvas.set_frame_line_width(0.005);
	//canvas.set_stick_width(0.002);
	//canvas.set_label_size(0.5);
	//canvas.set_title_size(0.6);
	//canvas.set_x_start(-80);
	//canvas.set_x_end(80);
	//canvas.set_y_start(-80);
	//canvas.set_y_end(80);
	canvas.define_coord_system(cr);
	canvas.do_not_draw_secondary_stick();
	canvas.draw_title(cr, "HV scan : March 13, 2025");
	canvas.draw_xtitle(cr, "High Voltage (V)");
	canvas.draw_ytitle(cr, "Nb. signals");
	
	// x coord to width
	auto x2w = [canvas] (double x) {
		return canvas.x2w(x);
	};
	// y coord to height
	auto y2h = [canvas] (double y) {
		return canvas.y2h(y);
	};

	int seff = canvas.get_seff();
	int heff = canvas.get_heff();
	int weff = canvas.get_weff();
	double x_start = canvas.get_x_start();
	double x_end = canvas.get_x_end();
	double y_start = canvas.get_y_start();
	double y_end = canvas.get_y_end();
	// Draw points
	//cr->set_line_width(0.01*seff);
	int marker_size = 8;
	printf("Marker_size : %d %d %d %d %lf \n", marker_size, weff, heff, seff, y_end-y_start);
	//cr->move_to(x2w(vec_hv[0]),y2h(vec_hv[0]));
	for (int i = 0; i < Npts; i++) {
		// draw a line between points i and i-1
		cr->set_source_rgb(0.0, 0.0, 1.0);
		cr->move_to(x2w(vec_hv[i]) + marker_size, y2h(vec_nwfs[i]));
		cr->arc(x2w(vec_hv[i]), y2h(vec_nwfs[i]), marker_size, 0, 2*M_PI); 
		cr->fill();
		// draw label : ahdv hv
		cr->set_source_rgb(1.0, 0.0, 0.0);
		cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::NORMAL);
		cr->set_font_size(0.8*canvas.get_label_size());
		Cairo::TextExtents te;
		char buffer[50];
		sprintf(buffer, "%d", (int) vec_run[i]);
		cr->get_text_extents(buffer, te);
		cr->move_to(x2w(vec_hv[i]) + 12, y2h(vec_nwfs[i]) - 12 );
		cr->show_text(buffer);
	}
	/*cr->set_source_rgb(0.0, 1.0, 0.0);
	cr->move_to(x2w(vec_hv[0]), y2h(vec_nwfs[0]));
	for (int i = 1; i < Npts; i++) {
		cr->line_to(x2w(vec_hv[i]), y2h(vec_nwfs[i]));
	}
	cr->stroke();*/
	
	// add legend
	/*cr->set_source_rgb(1.0, 0.0, 0.0);
	cr->select_font_face("@cairo:sans-serif",Cairo::ToyFontFace::Slant::NORMAL,Cairo::ToyFontFace::Weight::BOLD);
	cr->set_font_size(1.0*canvas.get_label_size());
	Cairo::TextExtents te;
	char buffer[50] = "numeric";
	cr->get_text_extents(buffer, te); 
	cr->move_to(0.5*weff - 0.5*te.width, -0.95*heff + te.height);
	cr->show_text(buffer);*/

	canvas.draw_frame(cr);	
	cr->show_page();
}
