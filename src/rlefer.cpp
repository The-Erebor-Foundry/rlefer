// C Math Library
#include <math.h>

// C++ STD Libraries
#include <vector>

#include "rlefer.h"

// Rcpp header
#include <Rcpp.h>


using namespace Rcpp;

//' R wrapper around the C++ implementation for even spaced curves.
//'
//' @description
//' Use the function \code{rlefer::even_spaced_curves()} instead.
//'
//' @keywords internal
//'
//' @param x_start1 the x coordinate of the starting point from which the function will start to draw your curve.
//' @param y_start1 the y coordinate of the starting point from which the function will start to draw your curve.
//' @param n_curves1 the number of curves you want to draw.
//' @param n_steps1 the number of steps used to draw each curve.
//' @param min_steps_allowed1 the minimum number of steps allowed in each curve (see Details for more info).
//' @param step_length1 the length/distance taken in each step.
//' @param d_sep1 the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
//' @param flow_field1 a 2D matrix with double values, each double value represents an angle value.
//' @param flow_field_width1 the width of the input flow field.
//'
// [[Rcpp::export]]
SEXP even_spaced_curves_impl(SEXP x_start1,
                             SEXP y_start1,
                             SEXP n_curves1,
                             SEXP n_steps1,
                             SEXP min_steps_allowed1,
                             SEXP step_length1,
                             SEXP d_sep1,
                             SEXP flow_field1,
                             SEXP flow_field_width1) {


  double x_start = as<double>(x_start1);
  double y_start = as<double>(y_start1);
  int n_curves = as<int>(n_curves1);
  int n_steps = as<int>(n_steps1);
  int min_steps_allowed = as<int>(min_steps_allowed1);
  double step_length = as<double>(step_length1);
  double d_sep = as<double>(d_sep1);
  NumericMatrix flow_field = as<NumericMatrix>(flow_field1);
  int flow_field_width = as<int>(flow_field_width1);


  lefer::FlowField _flow_field = lefer::FlowField(flow_field, flow_field_width);
  lefer::DensityGrid density_grid = lefer::DensityGrid(
    flow_field_width, flow_field_width,
    d_sep,
    2000
  );


  std::vector<lefer::Curve> curves;
  curves.reserve(n_curves);
  double x = x_start;
  double y = y_start;
  int curve_array_index = 0;
  int curve_id = 0;
  lefer::Curve curve = draw_curve(
    curve_id,
    x, y,
    n_steps,
    step_length,
    d_sep,
    &_flow_field,
    &density_grid
  );

  curves.emplace_back(curve);
  density_grid.insert_curve_coords(&curve);
  curve_array_index++;


  while (curve_id < n_curves && curve_array_index < n_curves) {
    lefer::SeedPointsQueue queue = lefer::SeedPointsQueue(n_steps);
    if (curve_id >= curves.size()) {
      // There is no more curves to be analyzed in the queue
      break;
    }
    queue = collect_seedpoints(&curves.at(curve_id), d_sep);
    for (lefer::Point p: queue._points) {
      // check if it is valid given the current state
      if (density_grid.is_valid_next_step(p.x, p.y)) {
        // if it is, draw the curve from it
        lefer::Curve curve = draw_curve(
          curve_array_index,
          p.x, p.y,
          n_steps,
          step_length,
          d_sep,
          &_flow_field,
          &density_grid
        );

        if (curve._steps_taken < min_steps_allowed) {
          continue;
        }

        curves.emplace_back(curve);
        // insert this new curve into the density grid
        density_grid.insert_curve_coords(&curve);
        curve_array_index++;
      }
    }

    curve_id++;
  }


  return lefer::_curves_as_df(curves, n_steps);
}


//' R wrapper around the C++ implementation for non overlapping curves.
//'
//' @description
//' Use the function \code{rlefer::non_overlapping_curves()} instead.
//'
//' @keywords internal
//'
//' @param starting_points a list object, with the x and y coordinates of the starting points for each curve.
//' @param n_curves the number of curves you want to draw.
//' @param n_steps the number of steps used to draw each curve.
//' @param min_steps_allowed the minimum number of steps allowed in each curve (see Details for more info).
//' @param step_length the length/distance taken in each step.
//' @param d_sep the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
//' @param flow_field a 2D matrix with double values, each double value represents an angle value.
//' @param flow_field_width the width of the input flow field.
//'
// [[Rcpp::export]]
DataFrame non_overlapping_curves_impl(List starting_points,
                                      int n_curves,
                                      int n_steps,
                                      int min_steps_allowed,
                                      double step_length,
                                      double d_sep,
                                      NumericMatrix flow_field,
                                      int flow_field_width) {

  lefer::FlowField _flow_field = lefer::FlowField(flow_field, flow_field_width);
  lefer::DensityGrid density_grid = lefer::DensityGrid(
    flow_field_width, flow_field_width,
    d_sep,
    2000
  );


  std::vector<lefer::Curve> curves;
  curves.reserve(n_curves);
  int curve_id = 0;
  List::iterator it = starting_points.begin();
  for (; it != starting_points.end(); it++) {
    List point = *it;
    double x_start = as<double>(point["x"]);
    double y_start = as<double>(point["y"]);
    // Check if this starting point is valid given the current state
    if (density_grid.is_valid_next_step(x_start, y_start)) {
      // if it is, draw the curve from it
      lefer::Curve curve = draw_curve(
        curve_id,
        x_start, y_start,
        n_steps,
        step_length,
        d_sep,
        &_flow_field,
        &density_grid
      );

      if (curve._steps_taken < min_steps_allowed) {
        continue;
      }

      curves.emplace_back(curve);
      // insert this new curve into the density grid
      density_grid.insert_curve_coords(&curve);
      curve_id++;
    }
  }


  return lefer::_curves_as_df(curves, n_steps);
}







namespace lefer {


Curve draw_curve(int curve_id,
                 double x_start,
                 double y_start,
                 int n_steps,
                 double step_length,
                 double d_sep,
                 FlowField* flow_field,
                 DensityGrid* density_grid) {

  Curve curve = Curve(curve_id, n_steps);
  curve.insert_step(x_start, y_start, 0);
  double x = x_start;
  double y = y_start;
  int i = 1;
  // Draw curve from right to left
  while (i < (n_steps / 2)) {
    if (flow_field->off_boundaries(x, y)) {
      break;
    }

    double angle = flow_field->get_angle(x, y);
    double x_step = step_length * cos(angle);
    double y_step = step_length * sin(angle);
    x = x - x_step;
    y = y - y_step;

    if (!density_grid->is_valid_next_step(x, y)) {
      break;
    }

    curve.insert_step(x, y, 0);
    i++;
  }

  x = x_start;
  y = y_start;
  // Draw curve from left to right
  while (i < n_steps) {
    if (flow_field->off_boundaries(x, y)) {
      break;
    }

    double angle = flow_field->get_angle(x, y);
    double x_step = step_length * cos(angle);
    double y_step = step_length * sin(angle);
    x = x + x_step;
    y = y + y_step;

    if (!density_grid->is_valid_next_step(x, y)) {
      break;
    }

    curve.insert_step(x, y, 1);
    i++;
  }

  return curve;
}
















// Utilitaries =======================================================

double distance (double x1, double y1, double x2, double y2) {
	double s1 = pow(x2 - x1, 2.0);
	double s2 = pow(y2 - y1, 2.0);
	return sqrt(s1 + s2);
}


DataFrame _curves_as_df(std::vector<lefer::Curve> &curves, int n_steps) {
  int n_curves = curves.size();
  NumericVector x(n_curves * n_steps);
  NumericVector y(n_curves * n_steps);
  NumericVector direction_ids(n_curves * n_steps);
  NumericVector step_ids(n_curves * n_steps);
  NumericVector steps_taken(n_curves * n_steps);
  NumericVector curve_ids(n_curves * n_steps);

  int row_index = 0;
  for (int curve_id = 0; curve_id < n_curves; curve_id++) {
    lefer::Curve curve = curves[curve_id];
    Rprintf("[INFO]: New curve %d\n", curve._curve_id);
    for (int i = 0; i < curve._steps_taken; i++) {
      Rprintf("[INFO]: Row index %d\n", row_index);
      curve_ids[row_index] = curve._curve_id;
      steps_taken[row_index] = curve._steps_taken;
      x[row_index] = curve._x[i];
      y[row_index] = curve._y[i];
      direction_ids[row_index] = curve._direction[i];
      step_ids[row_index] = curve._step_id[i];

      row_index++;
    }
  }


  return DataFrame::create(
    Named("curve_id") = curve_ids,
    Named("steps_taken") = steps_taken,
    Named("x") = x,
    Named("y") = y,
    Named("direction_id") = direction_ids,
    Named("step_ids") = step_ids
  );
}











// FlowField class =======================================================

FlowField::FlowField(NumericMatrix flow_field, int field_width) {
	_flow_field = flow_field;
	_field_width = field_width;
}


int FlowField::get_field_width() {
	return _field_width;
}


int FlowField::get_flow_field_col(double x) {
	return (int) x;
}

int FlowField::get_flow_field_row(double y) {
	return (int) y;
}

bool FlowField::off_boundaries(double x, double y) {
	return (
	x <= 0 ||
	y <= 0 ||
	x >= _field_width ||
	y >= _field_width
	);
}


double FlowField::get_angle(double x, double y) {
	int xi = get_flow_field_col(x);
	int yi = get_flow_field_row(y);
	return _flow_field(xi, yi);
}












// Curve class =======================================================

Curve::Curve(int id, int n_steps) {
	_curve_id = id;
	_steps_taken = 0;
	_x.reserve(n_steps);
	_y.reserve(n_steps);
	_direction.reserve(n_steps);
	_step_id.reserve(n_steps);
}

void Curve::insert_step(double x_coord, double y_coord, int direction_id) {
	_x.emplace_back(x_coord);
	_y.emplace_back(y_coord);
	_direction.emplace_back(direction_id);
	_step_id.emplace_back(_steps_taken);
	_steps_taken++;
}









// DensityGrid class ============================================================================


DensityGrid::DensityGrid(int flow_field_width, int flow_field_height, double d_sep, int cell_capacity) {
	int grid_width = (int)(flow_field_width / d_sep);
	int grid_height = (int)(flow_field_height / d_sep);
	_d_sep = d_sep;
	_width = grid_width;
	_height = grid_height;
	_n_elements = grid_width * grid_height;
	_grid.reserve(grid_width * grid_height);

	for (int i = 0; i < _n_elements; i++) {
		_grid[i].x.reserve(cell_capacity);
		_grid[i].y.reserve(cell_capacity);
		_grid[i].capacity = cell_capacity;
		_grid[i].space_used = 0;
	}
}

int DensityGrid::get_density_col (double x) {
	double c = (x / _d_sep);
	return (int) c;
}

int DensityGrid::get_density_row (double y) {
	double r = (y / _d_sep);
	return (int) r;
}

int DensityGrid::get_density_index (double x, double y) {
	int col = get_density_col(x);
	int row = get_density_row(y);
	return col + _width * row;
}

int DensityGrid::get_density_index (int col, int row) {
	return col + _width * row;
}

bool DensityGrid::off_boundaries(double x, double y) {
	int c = get_density_col(x);
	int r = get_density_row(y);
	return (
	c <= 0 ||
	r <= 0 ||
	c >= _width ||
	r >= _height
	);
}

void DensityGrid::insert_coord(double x, double y) {
	if (off_boundaries(x, y)) {
		return;
	}

	int density_index = get_density_index(x, y);
	int space_used = _grid[density_index].space_used;
	int capacity = _grid[density_index].capacity;

	if ((space_used + 1) < capacity) {
		_grid[density_index].x.emplace_back(x);
		_grid[density_index].y.emplace_back(y);
		_grid[density_index].space_used++;
	}
}

void DensityGrid::insert_curve_coords(Curve* curve) {
	int steps_taken = curve->_steps_taken;
	for (int i = 0; i < steps_taken; i++) {
		insert_coord(curve->_x.at(i), curve->_y.at(i));
	}
}

bool DensityGrid::is_valid_next_step(double x, double y) {
	if (off_boundaries(x, y)) {
		return 0;
	}

	int density_col = get_density_col(x);
	int density_row = get_density_row(y);
	int start_row = (density_row - 1) > 0 ? density_row - 1 : 0;
	int end_row = (density_row + 1) < _width ? density_row + 1 : density_row;
	int start_col = (density_col - 1) > 0 ? density_col - 1 : 0;
	int end_col = (density_col + 1) < _height ? density_col + 1 : density_col;

	// Subtracting a very small amount from D_TEST, just to account for the lost of float precision
	// that happens during the calculations below, specially in the distance calc
	double d_test = _d_sep - (0.01 * _d_sep);
	for (int c = start_col; c <= end_col; c++) {
		for (int r = start_row; r <= end_row; r++) {
			int density_index = get_density_index(c, r);
			int n_elements = _grid[density_index].space_used;
			if (n_elements == 0) {
				continue;
			}

			for (int i = 0; i < n_elements; i++) {
				double x2 = _grid[density_index].x.at(i);
				double y2 = _grid[density_index].y.at(i);
				double dist = distance(x, y, x2, y2);
				if (dist <= d_test) {
					return 0;
				}
			}
		}
	}

	return 1;
}


















// SeedPointsQueue class =========================================================================

SeedPointsQueue::SeedPointsQueue(int n_steps) {
	_capacity = n_steps * 2;
	_space_used = 0;
	_points.reserve(n_steps * 2);
}

bool SeedPointsQueue::is_empty() {
	return _space_used == 0;
}

void SeedPointsQueue::insert_coord(double x, double y) {
	Point p = {x, y};
	_points.emplace_back(p);
	_space_used++;
}

void SeedPointsQueue::insert_point(Point p) {
	_points.emplace_back(p);
	_space_used++;
}



SeedPointsQueue collect_seedpoints (Curve* curve, double d_sep) {
	int steps_taken = curve->_steps_taken;
	SeedPointsQueue queue = SeedPointsQueue(steps_taken);
	if (steps_taken == 0) {
		return queue;
	}

	for (int i = 0; i < steps_taken - 1; i++) {
		double x = curve->_x.at(i);
		double y = curve->_y.at(i);
		double angle = atan2(curve->_y.at(i + 1) - y, curve->_x.at(i + 1) - x);

		double angle_left = angle + (M_PI / 2);
		double angle_right = angle - (M_PI / 2);

		Point left_point = {
			x + (d_sep * cos(angle_left)),
			y + (d_sep * sin(angle_left))
		};
		Point right_point = {
			x + (d_sep * cos(angle_right)),
			y + (d_sep * sin(angle_right))
		};

		queue.insert_point(left_point);
		queue.insert_point(right_point);
	}

	return queue;
}


} // namespace lefer
