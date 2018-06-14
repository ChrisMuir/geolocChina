geoChina
========

R package that takes Chinese location/address stings as input, and returns geolocation data for each input string.

The functions are designed to work with business name strings and address strings. For each input string, the following values are returned: `Province`, `City`, `County`, `Provincial geocode`, `City geocode`, and `County geocode`. Values are returned as a tidy data frame.

The package is using geo substrings and associated geocodes from [this](http://www.stats.gov.cn/tjsj/tjbz/tjyqhdmhcxhfdm/) stats.gov.cn source.

There are other R packages that offer similar functionality, namely [baidumap](https://github.com/badbye/baidumap) and [poster](https://github.com/Ironholds/poster). For more info on geolocating Chinese strings, see [this](https://pdfs.semanticscholar.org/ca9d/2d09d0a2420a7ce398e14ed43f8cd7464705.pdf) 2016 paper on the subject.

Please [report](https://github.com/ChrisMuir/geoChina/issues) issues, comments, or feature requests.

Installation
------------

Install from GitHub

``` r
# install.packages("devtools")
devtools::install_github("ChrisMuir/geoChina")
```

Example Usage
-------------

```r
library(geoChina)
```

```r
terms <- c(
  "大连市甘井子区南关岭街道姚工街101号",
  "浙江省杭州市余杭区径山镇小古城村",
  "大连御洋食品有限公司",
  "徐州雅莲连锁超市有限公司", 
  "四川省南充市阆中市公园路63号", 
  "同德县鲜肉蔬菜配送行"
)

geo_locate(terms)
#>                              location province           city county province_code city_code county_code
#> 1 大连市甘井子区南关岭街道姚工街101号     辽宁           大连 甘井子            21      2102      210211
#> 2    浙江省杭州市余杭区径山镇小古城村     浙江           杭州   余杭            33      3301      330110
#> 3                大连御洋食品有限公司     辽宁           大连     NA            21      2102          NA
#> 4            徐州雅莲连锁超市有限公司     江苏           徐州     NA            32      3203          NA
#> 5        四川省南充市阆中市公园路63号     四川           南充   阆中            51      5113      511381
#> 6                同德县鲜肉蔬菜配送行     青海 海南藏族自治州   同德            63      6325      632522
```

How It Works
------------

Will update soon
