library(magrittr)
library(httr)
library(jsonlite)

# Get all of the most up-to-date China geocode data from GitHub repo:
# https://github.com/modood/Administrative-divisions-of-China
# Write to txt files as package data.

# If extdata directory does not exist, create it.
pkg_dir <- getwd()
extdata_dir <- file.path(pkg_dir, "inst", "extdata")
if (!dir.exists(extdata_dir)) {
  dir.create(extdata_dir, recursive = TRUE)
}

# Provincial data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/provinces.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
names_edited <- gsub("市|省|自治区", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}

file_name <- "./inst/extdata/prov_array.txt"
for (i in 1:length(res$name)) {
  write("{", file_name, append = TRUE)
  write(paste0('"', res$name[i], '",'), file_name, append = TRUE)
  write(paste0(res$code[i], ","), file_name, append = TRUE)
  write("},", file_name, append = TRUE)
}

# City data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/cities.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
names_edited <- gsub("市|自治区|盟|自治州|地区", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}

file_name <- "./inst/extdata/city_array.txt"
for (i in 1:length(res$name)) {
  write("{", file_name, append = TRUE)
  write(paste0('"', res$name[i], '",'), file_name, append = TRUE)
  write(paste0(res$code[i], ","), file_name, append = TRUE)
  write(paste0(res$provinceCode[i], ","), file_name, append = TRUE)
  write("},", file_name, append = TRUE)
}

# County data
res <- httr::GET("https://raw.githubusercontent.com/modood/Administrative-divisions-of-China/master/dist/areas.json") %>% 
  httr::content("text", encoding = "UTF-8") %>% 
  jsonlite::fromJSON()
Sys.sleep(2)
res <- res[!nchar(res$name) < 2, ]
names_edited <- gsub("地区|区|县|市|自治", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}

file_name <- "./inst/extdata/county_array.txt"
for (i in 1:length(res$name)) {
  write("{", file_name, append = TRUE)
  write(paste0('"', res$name[i], '",'), file_name, append = TRUE)
  write(paste0(res$code[i], ","), file_name, append = TRUE)
  write(paste0(res$cityCode[i], ","), file_name, append = TRUE)
  write(paste0(res$provinceCode[i], ","), file_name, append = TRUE)
  write("},", file_name, append = TRUE)
}


