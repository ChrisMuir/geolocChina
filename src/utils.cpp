#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"
#include "pkg_data.h"
#include "na_list.h"


// Given an input Chinese location/address string, determine the Province,
// City, and County of the input string (and the assoicated geocodes for all
// three), return the six values as a named list.
void get_locations(const std::string &cn_str, 
                   std::unordered_map<std::string, int> &substr_map, 
                   std::string &matches, 
                   List &out) {
  
  out = clone(na_list);
  int cn_str_len = cn_str.size();
  
  // Get all possible substrings of cn_str that could possibly have a match 
  // in the package data strings (just based on char length).
  get_all_substr(cn_str, substr_map);
  
  // Create iterators that will be used in function calls below.
  std::unordered_map<std::string, int>::iterator map_end = substr_map.end();
  std::unordered_map<std::string, int>::iterator map_iter;
  std::unordered_set<std::string>::iterator set_iter;
  
  // Look for a Province match.
  int curr_prov_code = NA_INTEGER;
  substring_lookup_prov(matches, substr_map, cn_str_len, map_end, map_iter, 
                        set_iter);
  if(matches.size() > 0) {
    out[0] = matches;
    curr_prov_code = as_geocode_prov(matches);
    out[3] = curr_prov_code;
  }
  
  // Look for a City match.
  if(curr_prov_code == NA_INTEGER) {
    substring_lookup_city(matches, substr_map, cn_str_len, map_end, map_iter, 
                          set_iter);
  } else {
    substring_lookup_city_w_code(curr_prov_code, matches, substr_map, 
                                 cn_str_len);
  }
  
  int curr_city_code = NA_INTEGER;
  if(matches.size() > 0) {
    out[1] = matches;
    curr_city_code = as_geocode_city(matches);
    out[4] = curr_city_code;
  }
  
  // Look for County match.
  if(curr_prov_code == NA_INTEGER && curr_city_code == NA_INTEGER) {
    substring_lookup_cnty(matches, substr_map, cn_str_len, map_end, map_iter, 
                          set_iter);
  } else if(curr_city_code == NA_INTEGER) {
    substring_lookup_cnty_w_code(curr_prov_code, matches, substr_map, 
                                 cn_str_len);
  } else {
    substring_lookup_cnty_w_code(curr_city_code, matches, substr_map, 
                                 cn_str_len);
  }
  
  int curr_cnty_code = NA_INTEGER;
  if(matches.size() > 0) {
    out[2] = matches;
    curr_cnty_code = as_geocode_cnty(matches);
    out[5] = curr_cnty_code;
  }
  
  //// Try to fill in any missing codes.
  
  // If City is NA and County is NA, try to fill in the City code/string from
  // the county_2015 vectors.
  if(curr_cnty_code == NA_INTEGER && curr_city_code == NA_INTEGER) {
    substring_lookup_cnty_2015(matches, substr_map, cn_str_len, map_end, 
                               map_iter, set_iter);
    
    if(matches.size() > 0) {
      int init_code = as_geocode_cnty_2015(matches);
      curr_city_code = substr_int(init_code, 0, 4);
      
      // If curr_city_code appears in the city_code_set, use it and its 
      // associated city string as outputs.
      if(city_code_set.find(curr_city_code) != city_code_set.end()) {
        std::string city = as_geostring_city(curr_city_code);
        out[1] = city;
        out[4] = curr_city_code;
      }
    }
  }
  
  // If City is NA and County is not NA, fill in City with the first four
  // digits from the County code.
  if(curr_cnty_code != NA_INTEGER && curr_city_code == NA_INTEGER) {
    curr_city_code = substr_int(curr_cnty_code, 0, 4);
    
    // If curr_city_code appears in the city_code_set, use it and its 
    // associated city string as outputs.
    if(city_code_set.find(curr_city_code) != city_code_set.end()) {
      out[4] = curr_city_code;
      std::string city = as_geostring_city(curr_city_code);
      out[1] = city;
    }
  }
  
  // If Province is NA and City is not NA, fill in Province with the first
  // two digits from the City code.
  if(curr_city_code != NA_INTEGER && curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    out[3] = curr_prov_code;
    std::string prov = as_geostring_prov(curr_prov_code);
    out[0] = prov;
  }

}


// Get all possible substrings of cn_str that could possibly have a match 
// in the package data strings (just based on char length).
void get_all_substr(const std::string &input, 
                    std::unordered_set<std::string> &substr_set) {
  int input_len = input.size();
  substr_set.clear();
  std::string curr_token;
  
  // Reserve output size of the substr_set (this is based on the largest
  // possible collection of substrings...char len 2 thru char len 12, with 
  // each substring potentially being made up of 3 bytes per char).
  substr_set.reserve(462);
  
  std::vector<int>::iterator iter;
  std::vector<int>::iterator loop_end = pkg_data_str_lens.end();
  for(iter = pkg_data_str_lens.begin(); iter != loop_end; ++iter) {
    int curr_token_len = *iter;
    if(curr_token_len > input_len) {
      break;
    }
    int input_iter_len = (input_len - curr_token_len) + 1;
    for(int j = 0; j < input_iter_len; ++j) {
      curr_token = input.substr(j, curr_token_len);
      substr_set.insert(curr_token);
    }
  }
}


// Get all possible substrings of cn_str that could possibly have a match 
// in the package data strings (just based on char length).
void get_all_substr(const std::string &input, 
                   std::unordered_map<std::string, int> &substr_map) {
  int input_len = input.size();
  substr_map.clear();
  std::string curr_token;
  
  // Reserve output size of the substr_set (this is based on the largest
  // possible collection of substrings...char len 2 thru char len 12, with 
  // each substring potentially being made up of 3 bytes per char).
  substr_map.reserve(462);
  
  std::vector<int>::iterator iter;
  std::vector<int>::iterator loop_end = pkg_data_str_lens.end();
  for(iter = pkg_data_str_lens.begin(); iter != loop_end; ++iter) {
    int curr_token_len = *iter;
    if(curr_token_len > input_len) {
      break;
    }
    int input_iter_len = (input_len - curr_token_len) + 1;
    for(int j = 0; j < input_iter_len; ++j) {
      curr_token = input.substr(j, curr_token_len);
      substr_map[curr_token] = j;
    }
  }
}


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings). Return all of the matches.
void substring_lookup_prov(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  
  matches.clear();
  int min_seen = cn_str_len;

  if(prov_set.size() > substr_map.size()) {
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(prov_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    std::unordered_set<std::string>::iterator prov_end = prov_set.end();
    for(set_iter = prov_set.begin(); set_iter != prov_end; ++set_iter) {
      if(substr_map.count(*set_iter) > 0 && substr_map[*set_iter] < min_seen) {
        matches = *set_iter;
        min_seen = substr_map[*set_iter];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings). Return all of the matches.
void substring_lookup_city(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  
  matches.clear();
  int min_seen = cn_str_len;
  
  if(city_set.size() > substr_map.size()) {
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(city_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    std::unordered_set<std::string>::iterator city_end = city_set.end();
    for(set_iter = city_set.begin(); set_iter != city_end; ++set_iter) {
      if(substr_map.count(*set_iter) > 0 && substr_map[*set_iter] < min_seen) {
        matches = *set_iter;
        min_seen = substr_map[*set_iter];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings). Return all of the matches.
void substring_lookup_cnty(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  
  matches.clear();
  int min_seen = cn_str_len;
  
  if(cnty_set.size() > substr_map.size()) {
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(cnty_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    std::unordered_set<std::string>::iterator cnty_end = cnty_set.end();
    for(set_iter = cnty_set.begin(); set_iter != cnty_end; ++set_iter) {
      if(substr_map.count(*set_iter) > 0 && substr_map[*set_iter] < min_seen) {
        matches = *set_iter;
        min_seen = substr_map[*set_iter];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings). Return all of the matches.
void substring_lookup_cnty_2015(std::string &matches, 
                                std::unordered_map<std::string, int> &substr_map, 
                                const int &cn_str_len, 
                                std::unordered_map<std::string, int>::iterator &map_end, 
                                std::unordered_map<std::string, int>::iterator &map_iter, 
                                std::unordered_set<std::string>::iterator &set_iter) {
  
  matches.clear();
  int min_seen = cn_str_len;
  
  if(cnty_2015_set.size() > substr_map.size()) {
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(cnty_2015_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    std::unordered_set<std::string>::iterator cnty_end = cnty_2015_set.end();
    for(set_iter = cnty_2015_set.begin(); set_iter != cnty_end; ++set_iter) {
      if(substr_map.count(*set_iter) > 0 && substr_map[*set_iter] < min_seen) {
        matches = *set_iter;
        min_seen = substr_map[*set_iter];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given a string (cn_str), look up each city string in cn_str (treating the 
// city strings as substrings). Also takes an int provincial code as input, 
// used for validating string matches. Return all of the matches.
void substring_lookup_city_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len) {
  matches.clear();
  int min_seen = cn_str_len;
  std::string pc_str = std::to_string(parent_code);
  std::string curr_city_code;
  
  for(int i = 0; i < city_dd_len; ++i) {
    if(substr_map.count(city_dd_strings[i]) > 0 && 
       substr_map[city_dd_strings[i]] < min_seen) {
      curr_city_code = std::to_string(city_dd_codes[i]);
      if(pc_str == curr_city_code.substr(0, 2)) {
        matches = city_dd_strings[i];
        min_seen = substr_map[city_dd_strings[i]];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given a string (cn_str), look up each county string in cn_str (treating the 
// county strings as substrings). Also takes an int geo code as input (either 
// provincial code or city code), used for validating string matches. Return 
// all of the matches.
void substring_lookup_cnty_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len) {
  
  matches.clear();
  int min_seen = cn_str_len;
  std::string pc_str = std::to_string(parent_code);
  int pc_str_len = pc_str.size();
  std::string curr_cnty_code;
  
  for(int i = 0; i < cnty_dd_len; ++i) {
    if(substr_map.count(cnty_dd_strings[i]) > 0 && 
       substr_map[cnty_dd_strings[i]] < min_seen) {
      curr_cnty_code = std::to_string(cnty_dd_codes[i]);
      if(pc_str == curr_cnty_code.substr(0, pc_str_len)) {
        matches = cnty_dd_strings[i];
        min_seen = substr_map[cnty_dd_strings[i]];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given an int, return a portion of the digits in int (a "substring" of int).
int substr_int(const int &x, const int &start, const int &out_len) {
  std::string x_str = std::to_string(x);
  return(atoi(x_str.substr(start, out_len).c_str()));
}


// Given a list of lists, extract the idx'th element  
// from each inner list, return as a char vector.
CharacterVector extract_char_vector(const List &x, const int &idx) {
  int x_len = x.size();
  CharacterVector out(x_len);
  
  List curr_x;
  for(int i = 0; i < x_len; ++i) {
    curr_x = x[i];
    std::string curr_out = curr_x[idx];
    if(curr_out != "NA") {
      out[i] = String(curr_out, CE_UTF8);
    } else {
      out[i] = NA_STRING;
    }
  }
  
  return(out);
}


// Given a list of lists, extract the idx'th element
// from each inner list, return as an int vector.
IntegerVector extract_int_vector(const List &x, const int &idx) {
  int x_len = x.size();
  IntegerVector out(x_len);
  
  List curr_x;
  for(int i = 0; i < x_len; ++i) {
    curr_x = x[i];
    out[i] = curr_x[idx];
  }
  
  return(out);
}


// Convert a provincial string from the pkg data dict to its
// corresponding geocode.
int as_geocode_prov(const std::string &cn_loc) {
  int out = 0;
  
  for(int i = 0; i < prov_dd_len; ++i) {
    if(prov_dd_strings[i] == cn_loc) {
      out = prov_dd_codes[i];
      break;
    }
  }
  
  return(out);
}


// Convert a city string from the pkg data dict to its
// corresponding geocode.
int as_geocode_city(const std::string &cn_loc) {
  int out = 0;
  
  for(int i = 0; i < city_dd_len; ++i) {
    if(city_dd_strings[i] == cn_loc) {
      out = city_dd_codes[i];
      break;
    }
  }
  
  return(out);
}


// Convert a county string from the pkg data dict to its
// corresponding geocode.
int as_geocode_cnty(const std::string &cn_loc) {
  int out = 0;
  
  for(int i = 0; i < cnty_dd_len; ++i) {
    if(cnty_dd_strings[i] == cn_loc) {
      out = cnty_dd_codes[i];
      break;
    }
  }
  
  return(out);
}


// Convert a county_2015 string from the pkg data dict to its
// corresponding geocode.
int as_geocode_cnty_2015(const std::string &cn_loc) {
  int out = 0;
  
  for(int i = 0; i < cnty_dd_2015_len; ++i) {
    if(cnty_dd_strings_2015[i] == cn_loc) {
      out = cnty_dd_codes_2015[i];
      break;
    }
  }
  
  return(out);
}


// Convert a provincial geocode from the pkg data dict to its
// corresponding location string.
std::string as_geostring_prov(const int &code) {
  std::string out;
  
  for(int i = 0; i < prov_dd_len; ++i) {
    if(prov_dd_codes[i] == code) {
      out = prov_dd_strings[i];
      break;
    }
  }
  
  return(out);
}


// Convert a city geocode from the pkg data dict to its
// corresponding location string.
std::string as_geostring_city(const int &code) {
  std::string out;
  
  for(int i = 0; i < city_dd_len; ++i) {
    if(city_dd_codes[i] == code) {
      out = city_dd_strings[i];
      break;
    }
  }
  
  return(out);
}


// Function that returns a data frame filled with NA values (this will be 
// used only when the input char vector is made up of all NA values).
DataFrame get_na_dataframe(const int &x) {
  CharacterVector na_strings = CharacterVector(x, NA_STRING);
  IntegerVector na_ints = IntegerVector(x, NA_INTEGER);
  DataFrame out = DataFrame::create(
    Named("location") = na_strings,
    Named("province") = na_strings,
    Named("city") = na_strings,
    Named("county") = na_strings,
    Named("province_code") = na_ints,
    Named("city_code") = na_ints,
    Named("county_code") = na_ints, 
    Named("stringsAsFactors") = false
  );
  
  return(out);
}


// Helper function for appending a string (val) to the end of a vector 
// (region_type) multiple times (times).
void rep_push_back(std::vector<std::string> &region_type, 
                   const std::string &val, 
                   const int &times) {
  for(int i = 0; i < times; ++i) {
    region_type.push_back(val);
  }
}


// Return a data frame containing all of the package geocode data.
// [[Rcpp::export]]
DataFrame cpp_get_package_data() {
  int out_len = prov_dd_len + city_dd_len + cnty_dd_len + cnty_dd_2015_len;
  
  // Create vector containing all package data region names.
  std::vector<std::string> strings;
  strings.reserve(out_len);
  strings.insert(strings.end(), prov_dd_strings.begin(), prov_dd_strings.end());
  strings.insert(strings.end(), city_dd_strings.begin(), city_dd_strings.end());
  strings.insert(strings.end(), cnty_dd_strings.begin(), cnty_dd_strings.end());
  strings.insert(strings.end(), cnty_dd_strings_2015.begin(), cnty_dd_strings_2015.end());

  // Create vector containing all package data geocodes.
  std::vector<int> codes;
  codes.reserve(out_len);
  codes.insert(codes.end(), prov_dd_codes.begin(), prov_dd_codes.end());
  codes.insert(codes.end(), city_dd_codes.begin(), city_dd_codes.end());
  codes.insert(codes.end(), cnty_dd_codes.begin(), cnty_dd_codes.end());
  codes.insert(codes.end(), cnty_dd_codes_2015.begin(), cnty_dd_codes_2015.end());

  // Create vector containing string indicators that correspond to one of the
  // four region types.
  std::vector<std::string> region_type;
  region_type.reserve(out_len);
  rep_push_back(region_type, "province", prov_dd_len);
  rep_push_back(region_type, "city", city_dd_len);
  rep_push_back(region_type, "county", cnty_dd_len);
  rep_push_back(region_type, "county_2015", cnty_dd_2015_len);

  DataFrame out = DataFrame::create(
    Named("region") = strings,
    Named("geocode") = codes,
    Named("region_type") = region_type,
    Named("stringsAsFactors") = false
  );
  
  return(out);
}
