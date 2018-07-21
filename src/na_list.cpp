#include <Rcpp.h>
using namespace Rcpp;

// Define object "na_list". This is a list of NA values that corresponds to 
// schema of the output data frame (province, city, county, province_code, 
// city_code, county_code).
List na_list = List::create(NA_STRING, NA_STRING, NA_STRING, 
                            NA_INTEGER, NA_INTEGER, NA_INTEGER);
