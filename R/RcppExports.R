# Generated by using Rcpp::compileAttributes() -> do not edit by hand
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#' R wrapper around the C++ implementation for even spaced curves.
#'
#' @description
#' Use the function \code{rlefer::even_spaced_curves()} instead.
#'
#' @keywords internal
#'
#' @param x_start1 the x coordinate of the starting point from which the function will start to draw your curve.
#' @param y_start1 the y coordinate of the starting point from which the function will start to draw your curve.
#' @param n_curves1 the number of curves you want to draw.
#' @param n_steps1 the number of steps used to draw each curve.
#' @param min_steps_allowed1 the minimum number of steps allowed in each curve (see Details for more info).
#' @param step_length1 the length/distance taken in each step.
#' @param d_sep1 the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
#' @param flow_field1 a 2D matrix with double values, each double value represents an angle value.
#' @param flow_field_width1 the width of the input flow field.
#'
even_spaced_curves_impl <- function(x_start1, y_start1, n_curves1, n_steps1, min_steps_allowed1, step_length1, d_sep1, flow_field1, flow_field_width1) {
    .Call(`_rlefer_even_spaced_curves_impl`, x_start1, y_start1, n_curves1, n_steps1, min_steps_allowed1, step_length1, d_sep1, flow_field1, flow_field_width1)
}

#' R wrapper around the C++ implementation for non overlapping curves.
#'
#' @description
#' Use the function \code{rlefer::non_overlapping_curves()} instead.
#'
#' @keywords internal
#'
#' @param starting_points a list object, with the x and y coordinates of the starting points for each curve.
#' @param n_curves the number of curves you want to draw.
#' @param n_steps the number of steps used to draw each curve.
#' @param min_steps_allowed the minimum number of steps allowed in each curve (see Details for more info).
#' @param step_length the length/distance taken in each step.
#' @param d_sep the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
#' @param flow_field a 2D matrix with double values, each double value represents an angle value.
#' @param flow_field_width the width of the input flow field.
#'
non_overlapping_curves_impl <- function(starting_points, n_curves, n_steps, min_steps_allowed, step_length, d_sep, flow_field, flow_field_width) {
    .Call(`_rlefer_non_overlapping_curves_impl`, starting_points, n_curves, n_steps, min_steps_allowed, step_length, d_sep, flow_field, flow_field_width)
}

