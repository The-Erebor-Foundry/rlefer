# rlefer

R package to draw non-overlapping and evenly-spaced curves in a flow field (i.e. vector field)
using the Jobard and Lefer (1997) algorithm. This algorithm is thoroughly described in a scientific paper (Jobard and Lefer 1997), but you might [find this article useful too](https://pedro-faria.netlify.app/posts/2024/2024-02-19-flow-even/en/index.html).

![](./man/figures/even_curves2.png)

# Install the package

To install the package, run:

```r
remotes::install_github("The-Erebor-Foundry/rlefer")
```

# Getting started

In order to draw any curve with `rlefer`, you need to have
a flow field first. That are different ways of generating a flow field,
but the easiest and most common way of doing so, is by using
2D (or "two dimensional") noise generators, such as the
Perlin Noise algorithm.

In R, you can easily generate a flow field through Perlin
Noise by using the `noise_perlin()` function from the `ambient` package.
A flow field, in essence, is a 2D matrix of numeric values. Each
value in this matrix represents an angle value. In other words,
you can visualize a flow field as a 2D grid of angle values.

In the example below, we are generating a flow field that
is 240x240. That is, a matrix of 240 rows and 240 columns.

```r
library(ambient)
set.seed(100)
flow_field <- noise_perlin(c(240, 240))
```

After you generated the flow field you want to use,
you can start to draw curves in it. The `rlefer` package
offers two main functions for you to draw these curves,
which are: `rlefer::non_overlapping_curves()` and
`rlefer::even_spaced_curves()`.

If you want to draw curves that do not overlap each other, but you do not care about how much far they are from each other, you probably want to use the function `rlefer::non_overlapping_curves()`. But if you want to draw curves that are both non-overlapping and evenly-spaced
between each other, then, the function `rlefer::even_spaced_curves()` is more
suitable for you.

In the example below, we are attempting to draw 100 curves into the flow field,
using 0.5 as the "separating distance", 2.4 as the "step length",
and 30 steps for each curve, with a minimum number of steps allowed of 5.
We get as a result, a tibble object with the x and y coordinates of each
curve that was drawn.

```r
library(rlefer)
curves <- even_spaced_curves(
  45, 24,
  100,
  30,
  5,
  0.01 * 240,
  0.5,
  flow_field
)

curves
```

```r
# A tibble: 960 × 6
   curve_id steps_taken     x     y direction_id step_ids
      <dbl>       <dbl> <dbl> <dbl>        <dbl>    <dbl>
 1        2          16  47.4  24.4            1       15
 2        3           6  61.8  23.3            1        5
 3        4           8  64.1  22.0            1        6
 4        4           8  66.5  21.9            1        7
 5        5          30  69.0  22.7            1        1
 6        5          30  71.4  22.6            1        2
 7        5          30  73.8  22.6            1        3
 8        5          30  76.2  22.6            1        4
 9        5          30  78.6  22.6            1        5
10        5          30  81.0  22.7            1        6
# ℹ 950 more rows
# ℹ Use `print(n = ...)` to see more rows
```



# References

Jobard, Bruno, and Wilfrid Lefer. 1997. “Creating Evenly-Spaced Streamlines of Arbitrary Density.” In Visualization in Scientific Computing ’97, edited by Wilfrid Lefer and Michel Grave, 43–55. Vienna: Springer Vienna.
