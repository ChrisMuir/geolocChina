#' Get Chinese Geolocation Data
#'
#' Get Chinese geolocation info as a data frame. Takes as input a char vector
#' of Chinese location strings, returns geolocation and geocode data. For each 
#' input string the following variables are created: province, city, county, 
#' province_code, city_code, county_code.
#'
#' @param cn_strings char vector, Chinese locations.
#'
#' @details Function is designed to be used with Chinese addresses and
#'  business names.
#'
#' @return data frame
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
  cpp_geo_locate(cn_strings, cn_env)
}
