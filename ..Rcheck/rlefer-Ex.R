pkgname <- "rlefer"
source(file.path(R.home("share"), "R", "examples-header.R"))
options(warn = 1)
options(pager = "console")
library('rlefer')

base::assign(".oldSearch", base::search(), pos = 'CheckExEnv')
base::assign(".old_wd", base::getwd(), pos = 'CheckExEnv')
cleanEx()
nameEx("even_spaced_curves")
### * even_spaced_curves

flush(stderr()); flush(stdout())

### Name: even_spaced_curves
### Title: Draws multiple evenly-spaced and non-overlapping curves in a
###   flow field.
### Aliases: even_spaced_curves

### ** Examples

library(ambient)
set.seed(50)
flow_field <- noise_perlin(c(240, 240))
# The coordinates x = 45 and y = 24 are used as the starting point:
curves <- even_spaced_curves(
  45, 24,
  100,
  30,
  5,
  0.01*240,
  0.5,
  flow_field
)




cleanEx()
nameEx("non_overlapping_curves")
### * non_overlapping_curves

flush(stderr()); flush(stdout())

### Name: non_overlapping_curves
### Title: Draws multiple non-overlapping curves in a flow field.
### Aliases: non_overlapping_curves

### ** Examples

library(ambient)
set.seed(50)
flow_field <- noise_perlin(c(240, 240))




### * <FOOTER>
###
cleanEx()
options(digits = 7L)
base::cat("Time elapsed: ", proc.time() - base::get("ptime", pos = 'CheckExEnv'),"\n")
grDevices::dev.off()
###
### Local variables: ***
### mode: outline-minor ***
### outline-regexp: "\\(> \\)?### [*]+" ***
### End: ***
quit('no')
