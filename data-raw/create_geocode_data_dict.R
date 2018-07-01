library(magrittr)
library(httr)
library(jsonlite)

# Get all of the most up-to-date China geocode data from GitHub repo:
# https://github.com/modood/Administrative-divisions-of-China

geo_data <- list()

# Provincial data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/provinces.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
res$code <- as.integer(res$code)
names_edited <- gsub("市|省|自治区", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}
geo_data$prov_strings <- res$name
geo_data$prov_codes <- res$code

# City data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/cities.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
res$code <- as.integer(res$code)
names_edited <- gsub("市|自治区|盟|自治州|地区", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}
geo_data$city_strings <- res$name
geo_data$city_codes <- res$code

# County data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/areas.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
res$code <- as.integer(res$code)
names_edited <- gsub("地区|区|县|市|自治", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}
geo_data$cnty_strings <- res$name
geo_data$cnty_codes <- res$code

# If extdata directory does not exist, create it.
pkg_dir <- getwd()
extdata_dir <- file.path(pkg_dir, "inst", "extdata")
if (!dir.exists(extdata_dir)) {
  dir.create(extdata_dir, recursive = TRUE)
}

# Save data to extdata dir.
save(
  geo_data, 
  file = file.path(extdata_dir, "cn_geocodes.rda"), 
  compress = "bzip2"
)
