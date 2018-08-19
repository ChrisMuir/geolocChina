#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"
#include "pkg_data.h"
#include "na_list.h"


// Given an input Chinese location/address string, determine the Province,
// City, and County of the input string (and the assoicated geocodes for all
// three), return the six values as a named list.
List get_locations(const std::string &cn_str, 
                   std::unordered_set<std::string> &substr_set, 
                   std::vector<std::string> &matches) {
  
  List out = clone(na_list);

  // Get all possible substrings of cn_str that could possibly have a match 
  // in the package data strings (just based on char length).
  get_all_substr(cn_str, substr_set);
  
  // Look for a Province match.
  int curr_prov_code = NA_INTEGER;
  substring_lookup_prov(cn_str, matches, substr_set);
  if(matches.size() > 0) {
    std::string prov = get_earliest_substr(cn_str, matches);
    out[0] = prov;
    curr_prov_code = as_geocode_prov(prov);
    out[3] = curr_prov_code;
  }
  
  // Look for a City match.
  if(curr_prov_code == NA_INTEGER) {
    substring_lookup_city(cn_str, matches, substr_set);
  } else {
    substring_lookup_city_w_code(cn_str, curr_prov_code, matches, substr_set);
  }
  
  int curr_city_code = NA_INTEGER;
  if(matches.size() > 0) {
    std::string city = get_earliest_substr(cn_str, matches);
    out[1] = city;
    curr_city_code = as_geocode_city(city);
    out[4] = curr_city_code;
  }
  
  // Look for County match.
  if(curr_prov_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    substring_lookup_cnty(cn_str, matches, substr_set);
  } else if(curr_city_code == NA_INTEGER) {
    substring_lookup_cnty_w_code(cn_str, curr_prov_code, matches, substr_set);
  } else {
    substring_lookup_cnty_w_code(cn_str, curr_city_code, matches, substr_set);
  }
  
  int curr_cnty_code = NA_INTEGER;
  if(matches.size() > 0) {
    std::string county = get_earliest_substr(cn_str, matches);
    out[2] = county;
    curr_cnty_code = as_geocode_cnty(county);
    out[5] = curr_cnty_code;
  }
  
  //// Try to fill in any missing codes.
  
  // If City is NA and County is NA, try to fill in the City code/string from
  // the county_2015 vectors.
  if(curr_cnty_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    substring_lookup_cnty_2015(cn_str, matches, substr_set);
    
    if(matches.size() > 0) {
      std::string county = get_earliest_substr(cn_str, matches);
      int init_code = as_geocode_cnty_2015(county);
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
  if(curr_cnty_code != NA_INTEGER and curr_city_code == NA_INTEGER) {
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
  if(curr_city_code != NA_INTEGER and curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    out[3] = curr_prov_code;
    std::string prov = as_geostring_prov(curr_prov_code);
    out[0] = prov;
  }
  
  return(out);
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


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings). Return all of the matches.
void substring_lookup_prov(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set) {
  
  matches.clear();
  std::unordered_set<std::string>::iterator iter;
  
  if(prov_set.size() > substr_set.size()) {
    std::unordered_set<std::string>::iterator ending = substr_set.end();
    std::unordered_set<std::string>::iterator prov_end = prov_set.end();
    for(iter = substr_set.begin(); iter != ending; ++iter) {
      if(prov_set.find(*iter) != prov_end) {
        matches.push_back(*iter);
      }
    }
  } else {
    std::unordered_set<std::string>::iterator ending = prov_set.end();
    std::unordered_set<std::string>::iterator substr_end = substr_set.end();
    for(iter = prov_set.begin(); iter != ending; ++iter) {
      if(substr_set.find(*iter) != substr_end) {
        matches.push_back(*iter);
      }
    }
  }
}


// Given a string (cn_str), look up each city string in cn_str (treating the 
// city strings as substrings). Return all of the matches.
void substring_lookup_city(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set) {
  matches.clear();
  std::unordered_set<std::string>::iterator iter;
  
  if(city_set.size() > substr_set.size()) {
    std::unordered_set<std::string>::iterator ending = substr_set.end();
    std::unordered_set<std::string>::iterator city_end = city_set.end();
    for(iter = substr_set.begin(); iter != ending; ++iter) {
      if(city_set.find(*iter) != city_end) {
        matches.push_back(*iter);
      }
    }
  } else {
    std::unordered_set<std::string>::iterator ending = city_set.end();
    std::unordered_set<std::string>::iterator substr_end = substr_set.end();
    for(iter = city_set.begin(); iter != ending; ++iter) {
      if(substr_set.find(*iter) != substr_end) {
        matches.push_back(*iter);
      }
    }
  }
}


// Given a string (cn_str), look up each county string in cn_str (treating the 
// county strings as substrings). Return all of the matches.
void substring_lookup_cnty(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set) {
  matches.clear();
  std::unordered_set<std::string>::iterator iter;
  
  if(cnty_set.size() > substr_set.size()) {
    std::unordered_set<std::string>::iterator ending = substr_set.end();
    std::unordered_set<std::string>::iterator cnty_end = cnty_set.end();
    for(iter = substr_set.begin(); iter != ending; ++iter) {
      if(cnty_set.find(*iter) != cnty_end) {
        matches.push_back(*iter);
      }
    }
  } else {
    std::unordered_set<std::string>::iterator ending = cnty_set.end();
    std::unordered_set<std::string>::iterator substr_end = substr_set.end();
    for(iter = cnty_set.begin(); iter != ending; ++iter) {
      if(substr_set.find(*iter) != substr_end) {
        matches.push_back(*iter);
      }
    }
  }
}


// Given a string (cn_str), look up each county_2015 string in cn_str (treating
// the county_2015 strings as substrings). Return all of the matches.
void substring_lookup_cnty_2015(const std::string &cn_str, 
                                std::vector<std::string> &matches, 
                                std::unordered_set<std::string> &substr_set) {
  matches.clear();
  std::unordered_set<std::string>::iterator iter;
  
  if(cnty_2015_set.size() > substr_set.size()) {
    std::unordered_set<std::string>::iterator ending = substr_set.end();
    std::unordered_set<std::string>::iterator cnty_2015_end = cnty_2015_set.end();
    for(iter = substr_set.begin(); iter != ending; ++iter) {
      if(cnty_2015_set.find(*iter) != cnty_2015_end) {
        matches.push_back(*iter);
      }
    }
  } else {
    std::unordered_set<std::string>::iterator ending = cnty_2015_set.end();
    std::unordered_set<std::string>::iterator substr_end = substr_set.end();
    for(iter = cnty_2015_set.begin(); iter != ending; ++iter) {
      if(substr_set.find(*iter) != substr_end) {
        matches.push_back(*iter);
      }
    }
  }
}


// Given a string (cn_str), look up each city string in cn_str (treating the 
// city strings as substrings). Also takes an int provincial code as input, 
// used for validating string matches. Return all of the matches.
void substring_lookup_city_w_code(const std::string &cn_str,
                                  const int &parent_code, 
                                  std::vector<std::string> &matches, 
                                  std::unordered_set<std::string> &substr_set) {
  
  substring_lookup_city(cn_str, matches, substr_set);
  
  if(matches.size() > 0) {
    std::string pc_str = std::to_string(parent_code);
    std::string curr_city_code;
    
    std::unordered_set<std::string> match_set(matches.begin(), matches.end());
    std::unordered_set<std::string>::iterator match_set_end = match_set.end();
    
    std::vector<std::string> temp_matches = matches;
    matches.clear();
    
    for(int i = 0; i < city_dd_len; ++i) {
      if(match_set.find(city_dd_strings[i]) != match_set_end) {
        curr_city_code = std::to_string(city_dd_codes[i]);
        if(pc_str == curr_city_code.substr(0, 2)) {
          matches.push_back(city_dd_strings[i]);
        }
      }
    }
  }
}


// Given a string (cn_str), look up each county string in cn_str (treating the 
// county strings as substrings). Also takes an int geo code as input (either 
// provincial code or city code), used for validating string matches. Return 
// all of the matches.
void substring_lookup_cnty_w_code(const std::string &cn_str, 
                                  const int &parent_code, 
                                  std::vector<std::string> &matches, 
                                  std::unordered_set<std::string> &substr_set) {
  
  matches.clear();
  std::string pc_str = std::to_string(parent_code);
  int pc_str_len = pc_str.size();
  std::unordered_set<std::string>::iterator set_end = substr_set.end();
  
  std::string curr_cnty_code;
  for(int i = 0; i < cnty_dd_len; i++) {
    if(substr_set.find(cnty_dd_strings[i]) != set_end) {
      curr_cnty_code = std::to_string(cnty_dd_codes[i]);
      if(pc_str == curr_cnty_code.substr(0, pc_str_len)) {
        matches.push_back(cnty_dd_strings[i]);
      }
    }
  }
}


// Get numeric index of a substring pattern with a string.
int substring_index(const std::string &term, const std::string &pat) {
  int pat_len = pat.size();
  int term_len = term.size();
  int term_less_pat = term_len - pat_len;
  
  if(term == pat) {
    return(0);
  }
  
  // Look for pat in substrings of term.
  bool match;
  char pat_0 = pat[0];
  for(int i = 0; i < term_less_pat + 1; ++i) {
    if(pat_0 == term[i]) {
      match = TRUE;
      for(int n = 1; n < pat_len; ++n) {
        if(pat[n] != term[i+n]) {
          match = FALSE;
          break;
        }
      }
      if(match) {
        return(i);
      }
    }
  }
  return(-1);
}


// Given a string (term) and a vector of substrings that all appear in term,
// return the substring that appears "earliest" in term (has the smallest
// beginning index within term).
std::string get_earliest_substr(const std::string &term,
                                const std::vector<std::string> &substrings) {
  int substrings_len = substrings.size();
  IntegerVector idx(substrings_len);
  
  for(int i = 0; i < substrings_len; ++i) {
    idx[i] = substring_index(term, substrings[i]);
  }
  
  return(substrings[which_min(idx)]);
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


// Helper function for appending a string (val) to the end of a vector (region_type) 
// multiple times (times).
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
