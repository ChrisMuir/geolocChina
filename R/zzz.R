# Set up package env to house package data used for geolocation.
cn_env <- new.env()

# Function for assigning a system file path to an R object in a specified env.
id_pkg_data_file <- function(obj_name, env, file_name, pkg) {
  assign(
    obj_name, 
    system.file("extdata",
                file_name,
                package = pkg), 
    envir = env
  )
}

# assign paths to all of the package data vectors to the cn_env.
id_pkg_data_file("prov_str_path", cn_env, "prov_strings.txt", "geolocChina")
id_pkg_data_file("prov_int_path", cn_env, "prov_codes.txt", "geolocChina")
id_pkg_data_file("city_str_path", cn_env, "city_strings.txt", "geolocChina")
id_pkg_data_file("city_int_path", cn_env, "city_codes.txt", "geolocChina")
id_pkg_data_file("cnty_str_path", cn_env, "cnty_strings.txt", "geolocChina")
id_pkg_data_file("cnty_int_path", cn_env, "cnty_codes.txt", "geolocChina")
id_pkg_data_file("cnty_2015_str_path", cn_env, "cnty_2015_strings.txt", "geolocChina")
id_pkg_data_file("cnty_2015_int_path", cn_env, "cnty_2015_codes.txt", "geolocChina")
