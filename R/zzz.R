# Set up package env to house package data used for geolocation.
cn_env <- new.env()

# Load package data.
geo_data_file <- system.file("extdata",
                              "cn_geocodes.rda",
                              package = "geolocChina")

if (file.exists(geo_data_file)) {
  load(geo_data_file, envir = cn_env)
} else {
  stop("geolocChina package data is missing")
}
