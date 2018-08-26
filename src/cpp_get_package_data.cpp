#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"
#include "global_data.h"


// Return a data frame containing all of the package geocode data.
// [[Rcpp::export]]
DataFrame cpp_get_package_data() {
  int out_len = supp_pkg_data::prov_dd_len + 
    supp_pkg_data::city_dd_len + 
    supp_pkg_data::cnty_dd_len + 
    supp_pkg_data::cnty_dd_2015_len;
  
  // Create vector containing all package data region names.
  std::vector<std::string> strings;
  strings.reserve(out_len);
  
  strings.insert(strings.end(), 
                 raw_pkg_data::prov_dd_strings.begin(), 
                 raw_pkg_data::prov_dd_strings.end());
  
  strings.insert(strings.end(), 
                 raw_pkg_data::city_dd_strings.begin(), 
                 raw_pkg_data::city_dd_strings.end());
  
  strings.insert(strings.end(), 
                 raw_pkg_data::cnty_dd_strings.begin(), 
                 raw_pkg_data::cnty_dd_strings.end());
  
  strings.insert(strings.end(), 
                 raw_pkg_data::cnty_dd_strings_2015.begin(), 
                 raw_pkg_data::cnty_dd_strings_2015.end());
  
  // Create vector containing all package data geocodes.
  std::vector<int> codes;
  codes.reserve(out_len);
  
  codes.insert(codes.end(), 
               raw_pkg_data::prov_dd_codes.begin(), 
               raw_pkg_data::prov_dd_codes.end());
  
  codes.insert(codes.end(), 
               raw_pkg_data::city_dd_codes.begin(), 
               raw_pkg_data::city_dd_codes.end());
  
  codes.insert(codes.end(), 
               raw_pkg_data::cnty_dd_codes.begin(), 
               raw_pkg_data::cnty_dd_codes.end());
  
  codes.insert(codes.end(), 
               raw_pkg_data::cnty_dd_codes_2015.begin(), 
               raw_pkg_data::cnty_dd_codes_2015.end());
  
  // Create vector containing string indicators that correspond to one of the
  // four region types.
  std::vector<std::string> region_type;
  region_type.reserve(out_len);
  rep_push_back(region_type, "province", supp_pkg_data::prov_dd_len);
  rep_push_back(region_type, "city", supp_pkg_data::city_dd_len);
  rep_push_back(region_type, "county", supp_pkg_data::cnty_dd_len);
  rep_push_back(region_type, "county_2015", supp_pkg_data::cnty_dd_2015_len);
  
  DataFrame out = DataFrame::create(
    Named("region") = strings,
    Named("geocode") = codes,
    Named("region_type") = region_type,
    Named("stringsAsFactors") = false
  );
  
  return(out);
}
