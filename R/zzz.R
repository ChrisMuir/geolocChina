# Set up package env to house package data used for geolocation.
cn_env <- new.env()

# Load package data.
geo_data_file <- system.file("extdata",
                              "cn_geocodes.rda",
                              package = "geolocChina")

if (file.exists(geo_data_file)) {
  load(geo_data_file, envir = cn_env)
} else {
  stop("package data file 'cn_geocodes.rda' is missing")
}

geo_data_2015 <- system.file("extdata",
                             "cn_geocodes_2015.rda",
                             package = "geolocChina")

if (file.exists(geo_data_2015)) {
  load(geo_data_2015, envir = cn_env)
} else {
  stop("package data file 'cn_geocodes_2015.rda' is missing")
}