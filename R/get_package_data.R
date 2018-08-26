#' Get geolocChina Package Data
#' 
#' Returns the package data used for geolocation as a tidy data frame. This 
#' package uses Chinese geolocation data from this GitHub repo:
#' \url{https://github.com/modood/Administrative-divisions-of-China}
#'
#' @return data frame, internal package data
#' @export
#'
#' @examples
#' pkg_data <- get_package_data()
get_package_data <- function() {
  cpp_get_package_data()
}
