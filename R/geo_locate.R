#' Get Chinese Geolocation Data
#'
#' Get Chinese geolocation info as a data frame. Takes as input a char vector
#' of Chinese location strings, returns geolocation and geocode data. This 
#' package uses Chinese geolocation data from this GitHub repo:
#' \url{https://github.com/modood/Administrative-divisions-of-China}
#'
#' @param cn_strings char vector, Chinese locations.
#'
#' @details Function is designed to be used with Chinese addresses and
#'  business names.
#'
#' @return data frame. For each input string, the following values are 
#'  returned: Province, City, County, Provincial geocode, City geocode, and 
#'  County geocode
#' 
#' @export
#'
#' @useDynLib geolocChina, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#'
#' @examples
#' geo_locate(c("中百超市有限公司长堤街二分店",
#'              "浙江省杭州市余杭区径山镇小古城村",
#'              "成都高梁红餐饮管理有限公司"))
geo_locate <- function(cn_strings) {
  stopifnot(is.character(cn_strings))
  cpp_geo_locate(cn_strings)
}

#' Get geolocChina Package Data
#' 
#' Returns the package data used for geolocation as a tidy data frame. This 
#' package uses Chinese geolocation data from this GitHub repo:
#' \url{https://github.com/modood/Administrative-divisions-of-China}
#'
#' @return data frame
#' @export
#'
#' @examples
#' pkg_data <- get_package_data()
get_package_data <- function() {
  cpp_get_package_data()
}
