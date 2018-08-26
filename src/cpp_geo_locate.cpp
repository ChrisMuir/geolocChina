#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"


// Exported function. Takes as input a char vector of Chinese strings, sends 
// each one through get_locations() in a loop. Returns a data frame of 
// geolocation data related to each input string.
// [[Rcpp::export]]
DataFrame cpp_geo_locate(const CharacterVector &cn_strings) {
  
  int cn_strings_len = cn_strings.size();
  
  // If input cn_strings is all NA values, then return a data frame full of NA
  // values.
  if(is_true(all(is_na(cn_strings)))) {
    return(get_na_dataframe(cn_strings_len));
  }
  
  std::unordered_map<std::string, int> substr_map;
  std::string matches;
  
  CharacterVector prov_str_out = CharacterVector(cn_strings_len, NA_STRING);
  CharacterVector city_str_out = CharacterVector(cn_strings_len, NA_STRING);
  CharacterVector cnty_str_out = CharacterVector(cn_strings_len, NA_STRING);
  IntegerVector prov_int_out = IntegerVector(cn_strings_len, NA_INTEGER);
  IntegerVector city_int_out = IntegerVector(cn_strings_len, NA_INTEGER);
  IntegerVector cnty_int_out = IntegerVector(cn_strings_len, NA_INTEGER);
  
  // Loop over cn_strings, get geolocations and geocodes for each string.
  for(int i = 0; i < cn_strings_len; ++i) {
    if(!CharacterVector::is_na(cn_strings[i])) {
      const std::string &curr_cn_str = as<std::string>(cn_strings[i]);
      get_locations(curr_cn_str, substr_map, matches, 
                    prov_str_out, city_str_out, cnty_str_out, 
                    prov_int_out, city_int_out, cnty_int_out, 
                    i);
    }
  }
  
  DataFrame out_df = DataFrame::create(
    Named("location") = cn_strings,
    Named("province") = prov_str_out,
    Named("city") = city_str_out,
    Named("county") = cnty_str_out,
    Named("province_code") = prov_int_out,
    Named("city_code") = city_int_out,
    Named("county_code") = cnty_int_out, 
    Named("stringsAsFactors") = false
  );
  
  return(out_df);
}
