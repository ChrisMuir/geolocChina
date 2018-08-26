#include <Rcpp.h>
using namespace Rcpp;

#ifndef GLOBAL_DATA_H
#define GLOBAL_DATA_H


struct iter_pkg_data {
  std::vector<int> pkg_data_str_lens;
  std::unordered_map<int, std::string> int_to_str_map;
  const std::unordered_set<std::string> prov_set;
  const std::unordered_set<std::string> city_set;
  const std::unordered_set<std::string> cnty_set;
  const std::unordered_set<std::string> cnty_2015_set;
  const std::unordered_set<int> city_code_set;
};


namespace raw_pkg_data {
  extern const std::vector<std::string> prov_dd_strings;
  extern const std::vector<int> prov_dd_codes;
  extern const std::vector<std::string> city_dd_strings;
  extern const std::vector<int> city_dd_codes;
  extern const std::vector<std::string> cnty_dd_strings;
  extern const std::vector<int> cnty_dd_codes;
  extern const std::vector<std::string> cnty_dd_strings_2015;
  extern const std::vector<int> cnty_dd_codes_2015;
}


namespace supp_pkg_data {
  extern const int &prov_dd_len;
  extern const int &city_dd_len;
  extern const int &cnty_dd_len;
  extern const int &cnty_dd_2015_len;
  extern iter_pkg_data containers;
}


namespace rcpp_na_list {
  extern List na_list;
}


#endif // GLOBAL_DATA_H
