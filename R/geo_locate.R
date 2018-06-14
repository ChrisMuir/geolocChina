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
#' @useDynLib geoChina, .registration = TRUE
#' @importFrom Rcpp sourceCpp
#'
#' @examples
#' geo_locate(c("中百超市有限公司长堤街二分店",
#'              "浙江省杭州市余杭区径山镇小古城村",
#'              "成都高梁红餐饮管理有限公司"))
geo_locate <- function(cn_strings) {
  stopifnot(is.character(cn_strings))

  # Unpack cols from the geo_data data dictionary.
  dd <- get("geo_data", envir = cn_env)

  prov_idx <- dd$geo_type == "province"
  prov_dd_strings <- dd[prov_idx, ]$geo_name
  prov_dd_codes <- dd[prov_idx, ]$geo_code

  city_idx <- dd$geo_type == "city"
  city_dd_strings <- dd[city_idx, ]$geo_name
  city_dd_codes <- dd[city_idx, ]$geo_code

  cnty_idx <- dd$geo_type == "county"
  cnty_dd_strings <- dd[cnty_idx, ]$geo_name
  cnty_dd_codes <- dd[cnty_idx, ]$geo_code

  cnty_2015_idx <- dd$geo_type == "county_2015"
  cnty_dd_strings_2015 <- dd[cnty_2015_idx, ]$geo_name
  cnty_dd_codes_2015 <- dd[cnty_2015_idx, ]$geo_code

  cpp_geo_locate(cn_strings, prov_dd_strings, prov_dd_codes, city_dd_strings,
                 city_dd_codes, cnty_dd_strings, cnty_dd_codes,
                 cnty_dd_strings_2015, cnty_dd_codes_2015)
}
