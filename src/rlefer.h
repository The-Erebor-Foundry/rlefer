#ifndef _RLEFER_HEADER_FILE_INCLUDE_
#define _RLEFER_HEADER_FILE_INCLUDE_

#include <vector>
#include <Rcpp.h>

namespace lefer {


double distance (double x1, double y1, double x2, double y2);
static int _grid_index_as_1d(int x, int y, int grid_width);


class FlowField {
private:
	Rcpp::NumericMatrix _flow_field;
	int _field_width;
public:
	FlowField(Rcpp::NumericMatrix flow_field, int field_width);
	int get_field_width();
	int get_flow_field_col(double x);
	int get_flow_field_row(double y);
	bool off_boundaries(double x, double y);
	double get_angle(double x, double y);
};

struct Point {
	double x;
	double y;
};



class Curve {
public:
	int _curve_id;
	std::vector<double> _x;
	std::vector<double> _y;
	std::vector<int> _direction;
	std::vector<int> _step_id;
	int _steps_taken;
public:
	Curve(int id, int n_steps);
	void insert_step(double x_coord, double y_coord, int direction_id);
};


struct DensityCell {
	std::vector<double> x;
	std::vector<double> y;
	int capacity;
	int space_used;
};


class DensityGrid {
private:
	std::vector<DensityCell> _grid;
	int _width;
	int _height;
	int _n_elements;
	double _d_sep;
public:
	DensityGrid(int flow_field_width, int flow_field_height, double d_sep, int cell_capacity);
	int get_density_col (double x);
	int get_density_row (double y);
	int get_density_index (double x, double y);
	int get_density_index (int col, int row);
	bool off_boundaries(double x, double y);
	void insert_coord(double x, double y);
	void insert_list_coords(Rcpp::List* curve);
	void insert_curve_coords(Curve* curve);
	bool is_valid_next_step(double x, double y);
};



class SeedPointsQueue {
public:
	std::vector<Point> _points;
	int _capacity;
	int _space_used;

public:
	SeedPointsQueue(int n_steps);
	bool is_empty();
	void insert_coord(double x, double y);
	void insert_point(Point p);
};



SeedPointsQueue collect_seedpoints (Rcpp::List* curve, double d_sep);



Rcpp::List draw_curve(int curve_id,
                double x_start,
                double y_start,
                int n_steps,
                double step_length,
                double d_sep,
                FlowField* flow_field,
                DensityGrid* density_grid);




} // namespace lefer


#endif
