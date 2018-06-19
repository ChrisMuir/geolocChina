library(magrittr)
library(httr)
library(jsonlite)

# Get all of the most up-to-date China geocode data from GitHub repo:
# https://github.com/modood/Administrative-divisions-of-China

# Provincial data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/provinces.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON() %>% 
  `colnames<-`(c("geo_code", "geo_name"))
res <- res[!nchar(res$geo_name) < 2, ]
res$geo_code <- as.integer(res$geo_code)
res$geo_type <- "province"
names_edited <- gsub("市|省|自治区", "", res$geo_name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$geo_name <- names_edited
} else {
  res$geo_name[-idx] <- names_edited[-idx]
}
geo_data <- res

# City data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/cities.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON() %>% 
  .[, c("code", "name")] %>% 
  `colnames<-`(c("geo_code", "geo_name"))
res <- res[!nchar(res$geo_name) < 2, ]
res$geo_code <- as.integer(res$geo_code)
res$geo_type <- "city"
names_edited <- gsub("市|自治区|盟|自治州|地区", "", res$geo_name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$geo_name <- names_edited
} else {
  res$geo_name[-idx] <- names_edited[-idx]
}
geo_data <- rbind(geo_data, res)

# County data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/areas.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON() %>% 
  .[, c("code", "name")] %>% 
  `colnames<-`(c("geo_code", "geo_name"))
res <- res[!nchar(res$geo_name) < 2, ]
res$geo_code <- as.integer(res$geo_code)
res$geo_type <- "county"
names_edited <- gsub("地区|区|县|市|自治", "", res$geo_name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$geo_name <- names_edited
} else {
  res$geo_name[-idx] <- names_edited[-idx]
}
geo_data <- rbind(geo_data, res)

# If extdata directory does not exist, create it.
pkg_dir <- getwd()
extdata_dir <- file.path(pkg_dir, "inst", "extdata")
if (!dir.exists(extdata_dir)) {
  dir.create(extdata_dir, recursive = TRUE)
}

# If pkg data file "cn_geocodes_2015.rda" exists, read it in and rbind it 
# to geo_data.
geo_2015_file <- system.file("extdata",
                             "cn_geocodes_2015.rda",
                             package = "geolocChina")

if (file.exists(geo_2015_file)) {
  load(geo_2015_file)
} else {
  stop("package data file 'cn_geocodes_2015.rda' is missing")
}
geo_data <- rbind(geo_data, geo_data_2015)

# Set data lengths as attributes to the pkg data frame.
attributes(geo_data)$prov_len <- sum(geo_data$geo_type == "province")
attributes(geo_data)$city_len <- sum(geo_data$geo_type == "city")
attributes(geo_data)$cnty_len <- sum(geo_data$geo_type == "county")
attributes(geo_data)$cnty_2015_len <- sum(geo_data$geo_type == "county_2015")

# Save data to extdata dir.
save(
  geo_data, 
  file = file.path(extdata_dir, "cn_geocodes.rda"), 
  compress = "bzip2"
)
