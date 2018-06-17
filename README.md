
<!-- README.md is generated from README.Rmd. Please edit that file -->
geolocChina
===========

[![lifecycle](https://img.shields.io/badge/lifecycle-experimental-orange.svg)](https://www.tidyverse.org/lifecycle/#experimental)

R package that takes Chinese location/address stings as input, and returns geolocation data for each input string.

The functions are designed to work with business name strings and address strings. For each input string, the following values are returned: `Province`, `City`, `County`, `Provincial geocode`, `City geocode`, and `County geocode`. Values are returned as a tidy data frame.

The package is using geo substrings and associated geocodes from these sources:

-   [Administrative-divisions-of-China](https://github.com/modood/Administrative-divisions-of-China) on GitHub, put together by user [modood](https://github.com/modood).
-   [this](http://www.stats.gov.cn/tjsj/tjbz/tjyqhdmhcxhfdm/) site from stats.gov.cn.

There are other R packages that offer similar functionality, namely [baidumap](https://github.com/badbye/baidumap) and [poster](https://github.com/Ironholds/poster). For more info on geolocating Chinese strings, see [this](https://pdfs.semanticscholar.org/ca9d/2d09d0a2420a7ce398e14ed43f8cd7464705.pdf) 2016 paper on the subject.

Please [report](https://github.com/ChrisMuir/geolocChina/issues) issues, comments, or feature requests.

Installation
------------

Install from GitHub

``` r
# install.packages("devtools")
devtools::install_github("ChrisMuir/geolocChina")
```

Example Usage
-------------

``` r
library(geolocChina)
library(knitr)

terms <- c(
  "大连市甘井子区南关岭街道姚工街101号",
  "浙江省杭州市余杭区径山镇小古城村",
  "大连御洋食品有限公司",
  "徐州雅莲连锁超市有限公司", 
  "四川省南充市阆中市公园路63号", 
  "同德县鲜肉蔬菜配送行"
)

knitr::kable(
  geo_locate(terms)
)
```

| location                                                                                                                            | province | city | county |  province\_code|  city\_code|  county\_code|
|:------------------------------------------------------------------------------------------------------------------------------------|:---------|:-----|:-------|---------------:|-----------:|-------------:|
| <U+5927><U+8FDE><U+5E02><U+7518><U+4E95><U+5B50><U+533A><U+5357><U+5173><U+5CAD><U+8857><U+9053><U+59DA><U+5DE5><U+8857>101<U+53F7> | NA       | NA   | NA     |              NA|          NA|            NA|
| <U+6D59><U+6C5F><U+7701><U+676D><U+5DDE><U+5E02><U+4F59><U+676D><U+533A><U+5F84><U+5C71><U+9547><U+5C0F><U+53E4><U+57CE><U+6751>    | NA       | NA   | NA     |              NA|          NA|            NA|
| <U+5927><U+8FDE><U+5FA1><U+6D0B><U+98DF><U+54C1><U+6709><U+9650><U+516C><U+53F8>                                                    | NA       | NA   | NA     |              NA|          NA|            NA|
| <U+5F90><U+5DDE><U+96C5><U+83B2><U+8FDE><U+9501><U+8D85><U+5E02><U+6709><U+9650><U+516C><U+53F8>                                    | NA       | NA   | NA     |              NA|          NA|            NA|
| <U+56DB><U+5DDD><U+7701><U+5357><U+5145><U+5E02><U+9606><U+4E2D><U+5E02><U+516C><U+56ED><U+8DEF>63<U+53F7>                          | NA       | NA   | NA     |              NA|          NA|            NA|
| <U+540C><U+5FB7><U+53BF><U+9C9C><U+8089><U+852C><U+83DC><U+914D><U+9001><U+884C>                                                    | NA       | NA   | NA     |              NA|          NA|            NA|

How It Works
------------

Will update soon
