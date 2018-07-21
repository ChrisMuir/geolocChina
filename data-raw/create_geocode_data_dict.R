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
res$code <- as.integer(res$code)
names_edited <- gsub("市|省|自治区", "", res$name)
idx <- which(nchar(names_edited) < 2)
if (length(idx) == 0) {
  res$name <- names_edited
} else {
  res$name[-idx] <- names_edited[-idx]
}

file_conn <- file("./inst/extdata/prov_strings.txt")
writeLines(res$name, file_conn)
close(file_conn)

file_conn <- file("./inst/extdata/prov_codes.txt")
writeLines(res$code, file_conn)
close(file_conn)

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

file_conn <- file("./inst/extdata/city_strings.txt")
writeLines(res$name, file_conn)
close(file_conn)

file_conn <- file("./inst/extdata/city_codes.txt")
writeLines(res$code, file_conn)
close(file_conn)

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

file_conn <- file("./inst/extdata/cnty_strings.txt")
writeLines(res$name, file_conn)
close(file_conn)

file_conn <- file("./inst/extdata/cnty_codes.txt")
writeLines(res$code, file_conn)
close(file_conn)
