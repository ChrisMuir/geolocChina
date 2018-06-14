# Utility function for determining if an object is NA, or NULL, or an
# empty string.
is_empty <- function(x) {
  if (is.na(x) || is.null(x) || x == "" || length(x) == 0) {
    TRUE
  } else {
    FALSE
  }
}


# Get the geocode associated with an input Chinese geolocation.
as_geocodes <- function(cn_locs, loc_type) {
  # Fetch package data dictionary.
  dd <- get("geo_data", envir = cn_env)

  # Return geocode associated with cn_loc.
  vapply(cn_locs, function(x) {
    dd[dd$geo_type == loc_type & dd$geo_name == x, "geo_code"]
  }, numeric(1), USE.NAMES = FALSE)
}


# Get the geo location string associated with an input geocode.
as_geostrings <- function(geo_codes, loc_type) {
  # Fetch package data dictionary.
  dd <- get("geo_data", envir = cn_env)

  # Return geocode associated with cn_loc.
  vapply(geo_codes, function(x) {
    dd[dd$geo_type == loc_type & dd$geo_code == x, "geo_name"]
  }, character(1), USE.NAMES = FALSE)
}


# Given a string (x) and a vector of substrings, find the substring that
# appears positionally the earliest in x (i.e. closest to the first char in x).
get_earliest_substr <- function(x, substrings) {
  # If substrings has length == 1, return it as a string.
  if (length(substrings) == 1) {
    return(substrings[1])
  }

  # For each element of substrings, get its index of the starting point within
  # string x.
  idx <- vapply(substrings, function(sub_str) {
    unlist(gregexpr(sub_str, x, fixed = TRUE), FALSE, FALSE)
  }, integer(1), USE.NAMES = FALSE)

  # Return the substring with the smallest positional index in x.
  substrings[which.min(idx)]
}


# version of base::startsWith() that is designed to take integer inputs.
startsWith_int <- function(x, prefix) {
  startsWith(as.character(x), as.character(prefix))
}


# Version of base::substr that is designed to take integer input.
substr_int <- function(x, start, stop) {
  as.integer(substr(as.character(x), start, stop))
}
