
even_spaced_curves <- function(x_start,
                                y_start,
                                n_curves,
                                n_steps,
                                min_steps_allowed,
                                step_length,
                                d_sep,
                                flow_field,
                                flow_field_width) {

  even_spaced_curves_impl(
        x_start,
        y_start,
        n_curves,
        n_steps,
        min_steps_allowed,
        step_length,
        d_sep,
        flow_field,
        flow_field_width
  )
}



non_overlapping_curves <- function(starting_points,
                               n_steps,
                               min_steps_allowed,
                               step_length,
                               d_sep,
                               flow_field,
                               flow_field_width) {

  n_curves <- length(starting_points)
  non_overlapping_curves_impl(
    starting_points,
    n_curves,
    n_steps,
    min_steps_allowed,
    step_length,
    d_sep,
    flow_field,
    flow_field_width
  )
}
