
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

| location                            | province | city     | county |  province\_code|  city\_code|  county\_code|
|:------------------------------------|:---------|:---------|:-------|---------------:|-----------:|-------------:|
| 大连市甘井子区南关岭街道姚工街101号 | 辽宁     | 大连     | 甘井子 |              21|        2102|        210211|
| 浙江省杭州市余杭区径山镇小古城村    | 浙江     | 杭州     | 余杭   |              33|        3301|        330110|
| 大连御洋食品有限公司                | 辽宁     | 大连     | NA     |              21|        2102|            NA|
| 徐州雅莲连锁超市有限公司            | 江苏     | 徐州     | NA     |              32|        3203|            NA|
| 四川省南充市阆中市公园路63号        | 四川     | 南充     | 阆中   |              51|        5113|        511381|
| 同德县鲜肉蔬菜配送行                | 青海     | 海南藏族 | 同德   |              63|        6325|        632522|

How It Works
------------

The geolocation function `geo_locate()` works by using both substring matching and geocodes to validate child regions and infer parent regions. The package ships with internal data that contains region substrings and associated geocodes. Access to the package data is provided via function `get_package_data()`, which returns a tidy data frame.

``` r
df <- get_package_data()
head(df)
#>   region geocode region_type
#> 1   北京      11    province
#> 2   天津      12    province
#> 3   河北      13    province
#> 4   山西      14    province
#> 5 内蒙古      15    province
#> 6   辽宁      21    province
```

The variable `region_type` features four unique values.

``` r
# Province
head(df[df$region_type == "province", ], 1)
#>   region geocode region_type
#> 1   北京      11    province

# City
head(df[df$region_type == "city", ], 1)
#>    region geocode region_type
#> 32   辖区    1101        city

# County
head(df[df$region_type == "county", ], 1)
#>     region geocode region_type
#> 375   东城  110101      county

# County 2015
head(df[df$region_type == "county_2015", ], 1)
#>      region geocode region_type
#> 3231   密云  110228 county_2015
```

`geo_locate()` looks for matching substrings in this order: province, then city, then county. It operates largely on the premise that county geocodes contain a city geocode, and city geocodes contain a provincial geocode. So the county with geocode `330110` resides in the city with geocode `3301`, which resides in the province with geocode `33`.

Here are some different examples showing how `geo_locate()` works. To begin, we'll start with a generic input location string.

``` r
base_str <- "工业路25号"
```

#### No matches

``` r
geo_locate(base_str)
#>     location province city county province_code city_code county_code
#> 1 工业路25号     <NA> <NA>   <NA>            NA        NA          NA
```

#### Province match

Add a province to the `base_str`.

``` r
x <- paste0("浙江省", base_str)
geo_locate(x)
#>           location province city county province_code city_code
#> 1 浙江省工业路25号     浙江 <NA>   <NA>            33        NA
#>   county_code
#> 1          NA
```

#### Province and city match

Add a province and a city that exists within the province to the `base_str`.

``` r
x <- paste0("浙江省", "杭州市", base_str)
geo_locate(x)
#>                 location province city county province_code city_code
#> 1 浙江省杭州市工业路25号     浙江 杭州   <NA>            33      3301
#>   county_code
#> 1          NA
```

It's important to note that, within the return data, the first two digits of the `city_code` match the `province_code`.

On the other hand, if we try to combine a province with a city that does NOT exist within the province, the `city` variables in the output will return `NA`.

``` r
x <- paste0("浙江省", "大连市", base_str)
geo_locate(x)
#>                 location province city county province_code city_code
#> 1 浙江省大连市工业路25号     浙江 <NA>   <NA>            33        NA
#>   county_code
#> 1          NA
```

This is because the functions attempt to establish a provincial match first, and then any city matches found must have an associated geocode that contain leading digits that match the provincial geocode. Here is the package data observation of the matching city substring.

``` r
df[df$region == "大连", ]
#>    region geocode region_type
#> 70   大连    2102        city
```

This same principle applies to the relationship between city matches/geocodes and county matches/geocodes, as we'll see below.

#### Province, city, and county match

Add a logical province, city, and county to `base_str`.

``` r
x <- paste0("浙江省", "杭州市余", "余杭区", base_str)
geo_locate(x)
#>                         location province city county province_code
#> 1 浙江省杭州市余余杭区工业路25号     浙江 杭州   余杭            33
#>   city_code county_code
#> 1      3301      330110
```

Just like in the example above, if we try to combine a county with a city that does NOT exist within the city, the `county` variables will return `NA`

``` r
x <- paste0("浙江省", "杭州市余", "甘井子区", base_str)
geo_locate(x)
#>                           location province city county province_code
#> 1 浙江省杭州市余甘井子区工业路25号     浙江 杭州   <NA>            33
#>   city_code county_code
#> 1      3301          NA
```

``` r
# Print the pkg data observation related to county "甘井子"
df[df$region == "甘井子", ]
#>     region geocode region_type
#> 814 甘井子  210211      county
```

#### City match only

Add a city to `base_str`.

``` r
x <- x <- paste0("杭州市余", base_str)
geo_locate(x)
#>             location province city county province_code city_code
#> 1 杭州市余工业路25号     浙江 杭州   <NA>            33      3301
#>   county_code
#> 1          NA
```

`geo_locate()` will return a matched province and city. This is because it will first look for a province match and return `NA`. It then looks for a city match (in this example, a city match is found). If a city match is found AND the province is `NA`, the province output values will be inferred from the city geocode. In this example, the matching city geocode is `3301`, the first two digits are taken and assigned as the matching provincial geocode. Then that geocode `33` is used to fetch a corresponding matching province (`浙江`) from the package data.

#### County match only

Add a county to `base_str`

``` r
x <- paste0("余杭区", base_str)
geo_locate(x)
#>           location province city county province_code city_code
#> 1 余杭区工业路25号     浙江 杭州   余杭            33      3301
#>   county_code
#> 1      330110
```

`geo_locate()` will return a matched province, city, and county. Just like in the last example, when the function cannot find a match for province and city, but a county match is found, the province and city output info will be inferred from the county geocode.

#### 2015 county package data

As part of the internal package data, `geolocChina` ships with a list of counties (and corresponding geocodes) that existed in 2015 and do not currently exist, due to rezoning and county/district lines being redrawn. `geo_locate()` will only use the 2015 county data if there was no provincial, city, nor county matches found. And if a match is found in the 2015 county data, the function will infer the output values for province and city, but will leave the county output values as `NA` (as opposed to returning county values that do not currently exist).

``` r
x <- paste0("北市区", base_str)
geo_locate(x)
#>           location province city county province_code city_code
#> 1 北市区工业路25号     河北 保定   <NA>            13      1306
#>   county_code
#> 1          NA
```

#### Two different regions of the same type

If there are multiple matches of the same region type in an input string (say, more than one provincial matches found), then `geo_locate()` will return the region that occupies the earliest character index within the input string (i.e. the substring that appears closest to the beginning of the input string). In the example below, the actual province is `浙江省`, but the road name `辽宁` also matches to a different province in China.

``` r
x <- "浙江省杭州市辽宁路25号"
geo_locate(x)
#>                 location province city county province_code city_code
#> 1 浙江省杭州市辽宁路25号     浙江 杭州   <NA>            33      3301
#>   county_code
#> 1          NA
```

The logic behind this comes from the fact that Chinese address/location strings typically have this structure

    province-city-county-district-street-number

Reading left to right, the geographic resolution starts large and gets more and more fine grained.

Using `四川省南充市阆中市公园路63号` as an example, we have

-   province:..**四川省** 南充市阆中市公园路63号
-   city:......四川省 **南充市** 阆中市公园路63号
-   county:....四川省南充市 **阆中市** 公园路63号

``` r
geo_locate("四川省南充市阆中市公园路63号")
#>                       location province city county province_code
#> 1 四川省南充市阆中市公园路63号     四川 南充   阆中            51
#>   city_code county_code
#> 1      5113      511381
```
