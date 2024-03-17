
utils::globalVariables(".data")

#' Draws multiple evenly-spaced and non-overlapping curves in a flow field.
#'
#' @description
#' Draws multiple evenly-spaced and non-overlapping curves in a flow field,
#' using the Jobard and Lefer (1997) algorithm.
#'
#' @details
#' You can use this function to draw multiple curves in a flow field.
#' Each curve will be non-overlapping and evenly-space between it's
#' neighbors.
#'
#' In essence, this function takes a single starting point (\code{x_start} and \code{y_start}) in the flow field,
#' and it starts to draw a initial curve in the flow field. After that, the function starts a loop process,
#' to draw \code{n_curves - 1} curves from this initial curve. In other words, all the curves that are drawn
#' into the flow field are derived from this initial curve.
#'
#' Is
#' worth noting that each starting point
#' represents the "middle point" of the curve, because the function draws a curve
#' in both directions. That is why we have the \code{direction_id} column in the
#' output of this function. It indicates what direction the function was
#' following when drawing the current section (or "step") of the current
#' curve.
#'
#' In each step of the way, the function will check if the current curve that is being
#' drawn is too close to it's neighbors, by calculating it's distance to the existing curves
#' around it. If the current curve is getting too close to a neighbor curve, then,
#' the function will stop drawing the current curve, and will start to draw the
#' next curve in the queue.
#'
#' If the function starts to draw a new curve, but the starting point of this new curve is already
#' too close to other existing curves, then, the function completely drops this curve (i.e.
#' it "gives up" on drawing this curve), and jumps to the next curve in the queue.
#'
#' Also, if the function draws a new curve, but this curve have less than
#' \code{min_allowed_steps} steps, then, this curve is also completely dropped.
#' This avoids getting a high number of curves that are too short.
#'
#' In other words, it is not guaranteed that this function will draw exactly \code{n_curves} curves
#' into the field, because, it might not have enough space for \code{n_curves} curves, considering your current settings.
#' So, the function
#' will attempt to draw as many curves as possible. As long as they are not overlapping
#' each other, and they are not too close to other neighbouring curves, the function will
#' continue to draw curves into the field.
#'
#' For more details about how the algorithm works, check: \url{https://pedro-faria.netlify.app/posts/2024/2024-02-19-flow-even/en/}
#'
#' @param x_start the x coordinate of the starting point from which the function will start to draw your curve.
#' @param y_start the y coordinate of the starting point from which the function will start to draw your curve.
#' @param n_curves the number of curves you want to draw.
#' @param n_steps the number of steps used to draw each curve.
#' @param min_steps_allowed the minimum number of steps allowed in each curve (see Details for more info).
#' @param step_length the length/distance taken in each step.
#' @param d_sep the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
#' @param flow_field a 2D matrix with double values, each double value represents an angle value.
#'
#' @return
#' This function returns a \code{tibble} object with 6 columns:
#' \itemize{
#' \item{\code{curve_id}: the ID of the curve.}
#' \item{\code{x}: the x coordinates of each point that represents the curve.}
#' \item{\code{y}: the y coordinates of each point that represents the curve.}
#' \item{\code{direction_id}: which direction that the algorithm was following when drawing the current point (0 means from left to right, 1 means from right to left).}
#' \item{\code{step_id}: the ID (or the number) of the current step.}
#' \item{\code{steps_taken}: the number of steps taken to draw the current curve.}
#' }
#'
#' @references
#' Jobard, Bruno, and Wilfrid Lefer. 1997. “Creating Evenly-Spaced Streamlines of Arbitrary Density.” In Visualization in Scientific Computing ’97, edited by Wilfrid Lefer and Michel Grave, 43–55. Vienna: Springer Vienna.
#'
#'
#' @examples
#' library(ambient)
#' set.seed(50)
#' flow_field <- noise_perlin(c(240, 240))
#' # The coordinates x = 45 and y = 24 are used as the starting point:
#' curves <- even_spaced_curves(
#'   45, 24,
#'   100,
#'   30,
#'   5,
#'   0.01*240,
#'   0.5,
#'   flow_field
#' )
#' @export
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
      .data$curve_id > 0,
      .data$x > 0,
      .data$y > 0,
      .data$steps_taken > 0,
      .data$step_ids > 0,
      .data$direction_id > 0,
    )
}










#' Draws multiple non-overlapping curves in a flow field.
#'
#' @description
#' Draws multiple non-overlapping curves in a flow field,
#' using the Jobard and Lefer (1997) algorithm.
#'
#' @param starting_points a list object, with the x and y coordinates of the starting points for each curve.
#' @param n_steps the number of steps used to draw each curve.
#' @param min_steps_allowed the minimum number of steps allowed in each curve (see Details for more info).
#' @param step_length the length/distance taken in each step.
#' @param d_sep the "separation distance", i.e., the amount of distance that each curve must be from neighbouring curves.
#' @param flow_field a 2D matrix with double values, each double value represents an angle value.
#'
#' @details
#' If you want to draw curves that are not only "non-overlapping",
#' but also, that are "evenly-spaced" between each other, you should
#' use the \code{even_spaced_curves()} function instead.
#' But if you care only if the curves are non-overlapping
#' between each other, then, this function is for you.
#'
#' You can use this function to draw multiple curves in a flow field.
#' Each curve will be non-overlapping between it's
#' neighbors.
#'
#' In essence, this function takes list of starting points for each curve you want to draw.
#' The function will attempt to draw a curve from each starting point.
#'
#' Is
#' worth noting that each starting point
#' represents the "middle point" of the curve, because the function draws a curve
#' in both directions. That is why we have the \code{direction_id} column in the
#' output of this function. It indicates what direction the function was
#' following when drawing the current section (or "step") of the current
#' curve.
#'
#' In each step of the way, the function will check if the current curve that is being
#' drawn is too close to it's neighbors, by calculating it's distance to the existing curves
#' around it. If the current curve is getting too close to a neighbor curve, then,
#' the function will stop drawing the current curve, and will start to draw the
#' next curve in the queue.
#'
#' If the function starts to draw a new curve, but the starting point of this new curve is already
#' too close to other existing curves, then, the function completely drops this curve (i.e.
#' it "gives up" on drawing this curve), and jumps to the next curve in the queue.
#'
#' Also, if the function draws a new curve, but this curve have less than
#' \code{min_allowed_steps} steps, then, this curve is also completely dropped.
#' This avoids getting a high number of curves that are too short.
#'
#' In other words, it is not guaranteed that this function will draw exactly \code{n_curves} curves
#' into the field, because, it might not have enough space for \code{n_curves} curves, considering your current settings.
#' So, the function
#' will attempt to draw as many curves as possible. As long as they are not overlapping
#' each other, and they are not too close to other neighbouring curves, the function will
#' continue to draw curves into the field.
#'
#' For more details about how the algorithm works, check: \url{https://pedro-faria.netlify.app/posts/2024/2024-02-19-flow-even/en/}
#'
#'
#' @return
#' This function returns a \code{tibble} object with 6 columns:
#' \itemize{
#' \item{\code{curve_id}: the ID of the curve.}
#' \item{\code{x}: the x coordinates of each point that represents the curve.}
#' \item{\code{y}: the y coordinates of each point that represents the curve.}
#' \item{\code{direction_id}: which direction that the algorithm was following when drawing the current point (0 means from left to right, 1 means from right to left).}
#' \item{\code{step_id}: the ID (or the number) of the current step.}
#' \item{\code{steps_taken}: the number of steps taken to draw the current curve.}
#' }
#'
#' @references
#' Jobard, Bruno, and Wilfrid Lefer. 1997. “Creating Evenly-Spaced Streamlines of Arbitrary Density.” In Visualization in Scientific Computing ’97, edited by Wilfrid Lefer and Michel Grave, 43–55. Vienna: Springer Vienna.
#'
#' @examples
#' library(ambient)
#' set.seed(50)
#' flow_field <- noise_perlin(c(240, 240))
#' set.seed(80)
#' xs <- runif(100)
#' set.seed(90)
#' ys <- runif(100)
#' xs <- xs * 240
#' ys <- ys * 240
#' starting_points <- list()
#' for(i in seq_len(100)) {
#'   starting_points[[i]] <- list(x = xs[i], y = ys[i])
#' }
#'
#' curves <- non_overlapping_curves(
#'   starting_points,
#'   30,
#'   5,
#'   0.01*240,
#'   0.5,
#'   flow_field
#' )
#' @export

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
      .data$curve_id > 0,
      .data$x > 0,
      .data$y > 0,
      .data$steps_taken > 0,
      .data$step_ids > 0,
      .data$direction_id > 0,
    )
}
