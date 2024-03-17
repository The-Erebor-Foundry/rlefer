
even_spaced_curves <- function(x_start,
                                y_start,
                                n_curves,
                                n_steps,
                                min_steps_allowed,
                                step_length,
                                d_sep,
                                flow_field) {

  flow_field_width <- dim(flow_field)[1]
  df <- even_spaced_curves_impl(
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

  df |>
    tibble::as_tibble() |>
    dplyr::filter(
      curve_id > 0,
      x > 0,
      y > 0,
      steps_taken > 0,
      step_ids > 0,
      direction_id > 0,
    )
}



non_overlapping_curves <- function(starting_points,
                               n_steps,
                               min_steps_allowed,
                               step_length,
                               d_sep,
                               flow_field) {

  flow_field_width <- dim(flow_field)[1]
  n_curves <- length(starting_points)
  df <- non_overlapping_curves_impl(
    starting_points,
    n_curves,
    n_steps,
    min_steps_allowed,
    step_length,
    d_sep,
    flow_field,
    flow_field_width
  )

  df |>
    tibble::as_tibble() |>
    dplyr::filter(
      curve_id > 0,
      x > 0,
      y > 0,
      steps_taken > 0,
      step_ids > 0,
      direction_id > 0,
    )
}
