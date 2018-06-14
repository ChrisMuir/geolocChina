#' Get Chinese Geolocation Codes
#'
#' @param cn_str string, Chinese location
#'
#' @details Function is designed to be used with Chinese addresses and
#'  business names.
#'
#' @return Vector of geolocation data.
#'
#' @examples
#' get_locations("浙江省杭州市余杭区径山镇小古城村")
get_locations <- function(cn_str) {

  # Initialize int vector to house geocodes.
  #codes <- as.list(rep(NA_integer_, 3))
  #names(codes) <- c("province_codes", "city_codes", "county_codes")

  # Initialize char vector to house str geolocations.
  #strings <- as.list(rep(NA_character_, 3))
  #names(strings) <- c("province", "city", "county")

  # Initialize output list.
  #out <- as.list(c(rep(NA_character_, 3), rep(NA_integer_, 3)))
  out <- list(NA_character_, NA_character_, NA_character_, NA_integer_,
              NA_integer_, NA_integer_)
  names(out) <- c("province", "city", "county", "province_code", "city_code",
                  "county_code")

  # If cn_str is empty (NA, NULL, "", length == 0), return vector of NA's.
  if (is_empty(cn_str)) {
    return(out)
  }

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
  cnty_dd_codes_2015 <- dd[cnty_2015_idx, ]$geo_name


  # Look for a Province match.
  provs <- cpp_substring_lookup(cn_str, prov_dd_strings)

  if (length(provs) > 0) {
    provs <- cpp_get_earliest_substr(cn_str, provs)
    out$province <- provs
    out$province_code <- cpp_as_geocodes(provs, prov_dd_strings, prov_dd_codes)
  }

  # Look for a City match.
  if (is.na(out$province_code)) {
    cities <- cpp_substring_lookup(cn_str, city_dd_strings)
  } else {
    cities <- cpp_substring_lookup_w_code(cn_str, city_dd_strings,
                                          out$province_code, city_dd_codes)
  }

  if (length(cities) > 0) {
    cities <- cpp_get_earliest_substr(cn_str, cities)
    out$city <- cities
    out$city_code <- cpp_as_geocodes(cities, city_dd_strings, city_dd_codes)
  }

  # Look for County match.
  if (is.na(out$province_code) && is.na(out$city_code)) {
    cntys <- cpp_substring_lookup(cn_str, cnty_dd_strings)
  } else if (is.na(out$city_code)) {
    cntys <- cpp_substring_lookup_w_code(cn_str, cnty_dd_strings,
                                          out$province_code, cnty_dd_codes)
  } else {
    cntys <- cpp_substring_lookup_w_code(cn_str, cnty_dd_strings,
                                          out$city_code, cnty_dd_codes)
  }

  if (length(cntys) > 0) {
    out$county <- cpp_get_earliest_substr(cn_str, cntys)
    out$county_code <- cpp_as_geocodes(out$county, cnty_dd_strings,
                                      cnty_dd_codes)
  }


  ## Try to fill in any missing codes.

  # If City is NA and County is NA, try to fill in the City code/string from
  # the county_2015 vectors.
  if (is.na(out$county_code) && is.na(out$city_code)) {
    cntys <- cpp_substring_lookup(cn_str, cnty_dd_strings_2015)

    if (length(cntys) > 0) {
      cntys <- cpp_as_geocodes(
        cpp_get_earliest_substr(cn_str, cntys),
        cnty_dd_strings_2015,
        cnty_dd_codes_2015
      )
      out$city <- cpp_as_geostrings(cpp_substr_int(cntys, 1, 4),
                                    city_dd_strings,
                                    city_dd_codes)
      out$city_code <- cpp_substr_int(cntys, 1, 4)

    }
  }

  # If City is NA and County is not NA, fill in City with the first four
  # digits from the County code.
  if (!is.na(out$county_code) && is.na(out$city_code)) {
    out$city <- cpp_as_geostrings(cpp_substr_int(out$county_code, 1, 4),
                                  city_dd_strings,
                                  city_dd_codes)
    out$city_code <- cpp_substr_int(out$county_code, 1, 4)

  }

  # If Province is NA and City is not NA, fill in Province with the first
  # two digits from the City code.
  if (!is.na(out$city_code) && is.na(out$province_code)) {
    out$province <- cpp_as_geostrings(cpp_substr_int(out$city_code, 1, 2),
                                      prov_dd_strings,
                                      prov_dd_codes)
    out$province_code <- cpp_substr_int(out$city_code, 1, 2)
  }

  out
}


# Search for Provincial substring matches.
get_prov_matches <- function(cn_str) {
  # Fetch package data.
  geo_data <- get("geo_data", envir = cn_env)

  provs <- vector(mode = "character")
  provs_len <- 1
  for (j in geo_data[geo_data$geo_type == "province", ]$geo_name) {
    if (grepl(j, cn_str, fixed = TRUE)) {
      provs[provs_len] <- j
      provs_len <- provs_len + 1
    }
  }

  provs
}


# Search for City substrings.
get_city_matches <- function(cn_str, codes) {
  # Fetch package data.
  geo_data <- get("geo_data", envir = cn_env)

  found_prov <- !is.na(codes$Province)
  city_idx <- geo_data$geo_type == "city"
  city_names <- geo_data[city_idx, ]$geo_name
  city_codes <- geo_data[city_idx, ]$geo_code
  cities <- vector(mode = "character")
  cities_len <- 1
  for (j in seq_len(length(city_names))) {
    if (grepl(city_names[j], cn_str, fixed = TRUE)) {
      if (!found_prov ||
          (found_prov && startsWith_int(city_codes[j], codes$Province))) {
        cities[cities_len] <- city_names[j]
        cities_len <- cities_len + 1
      }
    }
  }

  cities
}


# Search for County substrings.
get_county_matches <- function(cn_str, codes,
                               county_type = c("county", "county_2015")) {
  county_type <- match.arg(county_type)

  # Fetch package data.
  geo_data <- get("geo_data", envir = cn_env)

  compare_codes <- FALSE
  if (!is.na(codes$City)) {
    compare_codes <- TRUE
    compare_type <- "City"
  } else if (!is.na(codes$Province)) {
    compare_codes <- TRUE
    compare_type <- "Province"
  }
  cnty_idx <- geo_data$geo_type == county_type
  cnty_names <- geo_data[cnty_idx, ]$geo_name
  cnty_codes <- geo_data[cnty_idx, ]$geo_code
  cntys <- vector(mode = "character")
  cntys_len <- 1

  if (compare_codes) {
    for (j in seq_len(length(cnty_names))) {
      if (grepl(cnty_names[j], cn_str, fixed = TRUE) &&
          startsWith_int(cnty_codes[j], codes[[compare_type]])) {
        cntys[cntys_len] <- cnty_names[j]
        cntys_len <- cntys_len + 1
      }
    }
  } else {
    for (j in seq_len(length(cnty_names))) {
      if (grepl(cnty_names[j], cn_str, fixed = TRUE)) {
        cntys[cntys_len] <- cnty_names[j]
        cntys_len <- cntys_len + 1
      }
    }
  }

  cntys
}
