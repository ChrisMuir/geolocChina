# Set up package env to house package data used for geolocation.
cn_env <- new.env()

# assign paths to all of the package data vectors to the cn_env.
assign(
  "prov_str_path", 
  system.file("extdata",
              "prov_strings.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "prov_int_path", 
  system.file("extdata",
              "prov_codes.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "city_str_path", 
  system.file("extdata",
              "city_strings.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "city_int_path", 
  system.file("extdata",
              "city_codes.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "cnty_str_path", 
  system.file("extdata",
              "cnty_strings.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "cnty_int_path", 
  system.file("extdata",
              "cnty_codes.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "cnty_2015_str_path", 
  system.file("extdata",
              "cnty_2015_strings.txt",
              package = "geolocChina"), 
  envir = cn_env
)

assign(
  "cnty_2015_int_path", 
  system.file("extdata",
              "cnty_2015_codes.txt",
              package = "geolocChina"), 
  envir = cn_env
)
