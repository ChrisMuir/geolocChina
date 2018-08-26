#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"
#include "global_data.h"


// Given an input Chinese location/address string, determine the Province,
// City, and County of the input string (and the assoicated geocodes for all
// three). Append the six values to the input vectors ("_out" vectors).
void get_locations(const std::string &cn_str, 
                   std::unordered_map<std::string, int> &substr_map, 
                   std::string &matches, 
                   CharacterVector &prov_str_out, 
                   CharacterVector &city_str_out, 
                   CharacterVector &cnty_str_out, 
                   IntegerVector &prov_int_out, 
                   IntegerVector &city_int_out, 
                   IntegerVector &cnty_int_out, 
                   const int &index) {
  
  int cn_str_len = cn_str.size();
  
  // Get all possible substrings of cn_str that could possibly have a match 
  // in the package data strings (based only on char length).
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
    prov_str_out[index] = String(matches, CE_UTF8);
    as_geocode_prov(matches, curr_prov_code);
    prov_int_out[index] = curr_prov_code;
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
    city_str_out[index] = String(matches, CE_UTF8);
    as_geocode_city(matches, curr_city_code);
    city_int_out[index] = curr_city_code;
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
    cnty_str_out[index] = String(matches, CE_UTF8);
    as_geocode_cnty(matches, curr_cnty_code);
    cnty_int_out[index] = curr_cnty_code;
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
      if(supp_pkg_data::containers.city_code_set.count(curr_city_code) > 0) {
        std::string city = as_geostring_city(curr_city_code);
        city_str_out[index] = city;
        city_int_out[index] = curr_city_code;
      }
    }
  }
  
  // If City is NA and County is not NA, fill in City with the first four
  // digits from the County code.
  if(curr_cnty_code != NA_INTEGER && curr_city_code == NA_INTEGER) {
    curr_city_code = substr_int(curr_cnty_code, 0, 4);
    
    // If curr_city_code appears in the city_code_set, use it and its 
    // associated city string as outputs.
    if(supp_pkg_data::containers.city_code_set.count(curr_city_code) > 0) {
      city_int_out[index] = curr_city_code;
      std::string city = as_geostring_city(curr_city_code);
      city_str_out[index] = city;
    }
  }
  
  // If Province is NA and City is not NA, fill in Province with the first
  // two digits from the City code.
  if(curr_city_code != NA_INTEGER && curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    prov_int_out[index] = curr_prov_code;
    std::string prov = as_geostring_prov(curr_prov_code);
    prov_str_out[index] = prov;
  }
  
}


// Get all possible substrings of cn_str that could possibly have a match 
// in the package data strings (based only on char length).
void get_all_substr(const std::string &input, 
                   std::unordered_map<std::string, int> &substr_map) {
  int input_len = input.size();
  substr_map.clear();
  std::string curr_token;
  
  // Reserve output size of the substr_set (this is based on the largest
  // possible collection of substrings...char len 2 thru char len 12, with 
  // each substring potentially being made up of 3 bytes per char).
  substr_map.reserve(462);
  
  // Iterate over all unique char length ints of the raw package data strings, 
  // for each int generate all possible substrings of the input string that 
  // have the same char length. Save them all as keys to substr_map, and set 
  // the associated first-char index integer as the map values.
  std::vector<int>::iterator iter;
  std::vector<int>::iterator loop_end = supp_pkg_data::containers.pkg_data_str_lens.end();
  for(iter = supp_pkg_data::containers.pkg_data_str_lens.begin(); iter != loop_end; ++iter) {
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


// Lookup each package data Province string in the substr_map. Find the 
// Provincial string with the smallest associated integer in substr_map (this 
// is the index of the first char of the Province string in the input cn_str).
void substring_lookup_prov(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  
  if(supp_pkg_data::containers.prov_set.size() > substr_map.size()) {
    // If the number of package data Province strings is greater than the size 
    // of the substr_map, iterate over the keys in substr_map.
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(supp_pkg_data::containers.prov_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    // Else, if the size of the substr_map is greater than the number of 
    // package data Province strings, iterate over the package data Province 
    // strings.
    std::unordered_set<std::string>::iterator prov_end = supp_pkg_data::containers.prov_set.end();
    for(set_iter = supp_pkg_data::containers.prov_set.begin(); set_iter != prov_end; ++set_iter) {
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


// Lookup each package data City string in the substr_map. Find the 
// City string with the smallest associated integer in substr_map (this 
// is the index of the first char of the City string in the input cn_str).
void substring_lookup_city(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  
  if(supp_pkg_data::containers.city_set.size() > substr_map.size()) {
    // If the number of package data City strings is greater than the size 
    // of the substr_map, iterate over the keys in substr_map.
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(supp_pkg_data::containers.city_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    // Else, if the size of the substr_map is greater than the number of 
    // package data City strings, iterate over the package data City strings.
    std::unordered_set<std::string>::iterator city_end = supp_pkg_data::containers.city_set.end();
    for(set_iter = supp_pkg_data::containers.city_set.begin(); set_iter != city_end; ++set_iter) {
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


// Lookup each package data County string in the substr_map. Find the 
// County string with the smallest associated integer in substr_map (this 
// is the index of the first char of the County string in the input cn_str).
void substring_lookup_cnty(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  
  if(supp_pkg_data::containers.cnty_set.size() > substr_map.size()) {
    // If the number of package data County strings is greater than the size 
    // of the substr_map, iterate over the keys in substr_map.
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(supp_pkg_data::containers.cnty_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    // Else, if the size of the substr_map is greater than the number of 
    // package data County strings, iterate over the package data County 
    // strings.
    std::unordered_set<std::string>::iterator cnty_end = supp_pkg_data::containers.cnty_set.end();
    for(set_iter = supp_pkg_data::containers.cnty_set.begin(); set_iter != cnty_end; ++set_iter) {
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


// Lookup each package data County 2015 string in the substr_map. Find the 
// County 2015 string with the smallest associated integer in substr_map (this 
// is the index of the first char of the County 2015 string in the input 
// cn_str).
void substring_lookup_cnty_2015(std::string &matches, 
                                std::unordered_map<std::string, int> &substr_map, 
                                const int &cn_str_len, 
                                std::unordered_map<std::string, int>::iterator &map_end, 
                                std::unordered_map<std::string, int>::iterator &map_iter, 
                                std::unordered_set<std::string>::iterator &set_iter) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  
  if(supp_pkg_data::containers.cnty_2015_set.size() > substr_map.size()) {
    // If the number of package data County 2015 strings is greater than the 
    // size of the substr_map, iterate over the keys in substr_map.
    for(map_iter = substr_map.begin(); map_iter != map_end; ++map_iter) {
      if(supp_pkg_data::containers.cnty_2015_set.count(map_iter->first) > 0 && map_iter->second < min_seen) {
        matches = map_iter->first;
        min_seen = map_iter->second;
        if(min_seen == 0) {
          break;
        }
      }
    }
  } else {
    // Else, if the size of the substr_map is greater than the number of 
    // package data County 2015 strings, iterate over the package data 
    // County 2015 strings.
    std::unordered_set<std::string>::iterator cnty_end = supp_pkg_data::containers.cnty_2015_set.end();
    for(set_iter = supp_pkg_data::containers.cnty_2015_set.begin(); set_iter != cnty_end; ++set_iter) {
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

// Lookup each package data City string in the substr_map. Find the 
// City string with the smallest associated integer in substr_map (this 
// is the index of the first char of the City string in the input 
// cn_str). Also takes as input a parent geocode, used for validating the 
// matching City string...when a City string match is found, the first two 
// digits of it's associated goecode must match the input parent geocode.
void substring_lookup_city_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  //std::string pc_str = std::to_string(parent_code);
  std::string pc_str = supp_pkg_data::containers.int_to_str_map[parent_code];
  std::string curr_city_code;
  
  for(int i = 0; i < supp_pkg_data::city_dd_len; ++i) {
    if(substr_map.count(raw_pkg_data::city_dd_strings[i]) > 0 && 
       substr_map[raw_pkg_data::city_dd_strings[i]] < min_seen) {
      //curr_city_code = std::to_string(raw_pkg_data::city_dd_codes[i]);
      curr_city_code = supp_pkg_data::containers.int_to_str_map[raw_pkg_data::city_dd_codes[i]];
      if(pc_str == curr_city_code.substr(0, 2)) {
        matches = raw_pkg_data::city_dd_strings[i];
        min_seen = substr_map[raw_pkg_data::city_dd_strings[i]];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}

// Lookup each package data County string in the substr_map. Find the 
// County string with the smallest associated integer in substr_map (this 
// is the index of the first char of the County string in the input 
// cn_str). Also takes as input a parent geocode, used for validating the 
// matching County string...when a County string match is found, the leading 
// digits of it's associated goecode must match the input parent geocode.
void substring_lookup_cnty_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len) {
  // Clear string "matches", this will house the output string value.
  matches.clear();
  int min_seen = cn_str_len;
  //std::string pc_str = std::to_string(parent_code);
  std::string pc_str = supp_pkg_data::containers.int_to_str_map[parent_code];
  int pc_str_len = pc_str.size();
  std::string curr_cnty_code;
  
  for(int i = 0; i < supp_pkg_data::cnty_dd_len; ++i) {
    if(substr_map.count(raw_pkg_data::cnty_dd_strings[i]) > 0 && 
       substr_map[raw_pkg_data::cnty_dd_strings[i]] < min_seen) {
      //curr_cnty_code = std::to_string(raw_pkg_data::cnty_dd_codes[i]);
      curr_cnty_code = supp_pkg_data::containers.int_to_str_map[raw_pkg_data::cnty_dd_codes[i]];
      if(pc_str == curr_cnty_code.substr(0, pc_str_len)) {
        matches = raw_pkg_data::cnty_dd_strings[i];
        min_seen = substr_map[raw_pkg_data::cnty_dd_strings[i]];
        if(min_seen == 0) {
          break;
        }
      }
    }
  }
}


// Given an int, return a portion of the digits (a "substring" of int).
int substr_int(const int &x, const int &start, const int &out_len) {
  //std::string x_str = std::to_string(x);
  std::string x_str = supp_pkg_data::containers.int_to_str_map[x];
  return(atoi(x_str.substr(start, out_len).c_str()));
}


// Convert a Provincial string from the pkg data dict to its
// corresponding geocode.
void as_geocode_prov(const std::string &cn_loc, int &curr_prov_code) {
  for(int i = 0; i < supp_pkg_data::prov_dd_len; ++i) {
    if(raw_pkg_data::prov_dd_strings[i] == cn_loc) {
      curr_prov_code = raw_pkg_data::prov_dd_codes[i];
      break;
    }
  }
}


// Convert a City string from the pkg data dict to its
// corresponding geocode.
void as_geocode_city(const std::string &cn_loc, int &curr_city_code) {
  for(int i = 0; i < supp_pkg_data::city_dd_len; ++i) {
    if(raw_pkg_data::city_dd_strings[i] == cn_loc) {
      curr_city_code = raw_pkg_data::city_dd_codes[i];
      break;
    }
  }
}


// Convert a County string from the pkg data dict to its
// corresponding geocode.
void as_geocode_cnty(const std::string &cn_loc, int &curr_cnty_code) {
  for(int i = 0; i < supp_pkg_data::cnty_dd_len; ++i) {
    if(raw_pkg_data::cnty_dd_strings[i] == cn_loc) {
      curr_cnty_code = raw_pkg_data::cnty_dd_codes[i];
      break;
    }
  }
}


// Convert a County 2015 string from the pkg data dict to its
// corresponding geocode.
int as_geocode_cnty_2015(const std::string &cn_loc) {
  int out = 0;
  
  for(int i = 0; i < supp_pkg_data::cnty_dd_2015_len; ++i) {
    if(raw_pkg_data::cnty_dd_strings_2015[i] == cn_loc) {
      out = raw_pkg_data::cnty_dd_codes_2015[i];
      break;
    }
  }
  
  return(out);
}


// Convert a Provincial geocode from the pkg data dict to its
// corresponding location string.
std::string as_geostring_prov(const int &code) {
  std::string out;
  
  for(int i = 0; i < supp_pkg_data::prov_dd_len; ++i) {
    if(raw_pkg_data::prov_dd_codes[i] == code) {
      out = raw_pkg_data::prov_dd_strings[i];
      break;
    }
  }
  
  return(out);
}


// Convert a City geocode from the pkg data dict to its
// corresponding location string.
std::string as_geostring_city(const int &code) {
  std::string out;
  
  for(int i = 0; i < supp_pkg_data::city_dd_len; ++i) {
    if(raw_pkg_data::city_dd_codes[i] == code) {
      out = raw_pkg_data::city_dd_strings[i];
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


// Helper function for appending a string to the end of a vector multiple 
// times.
void rep_push_back(std::vector<std::string> &input_vector, 
                   const std::string &val, 
                   const int &times) {
  for(int i = 0; i < times; ++i) {
    input_vector.push_back(val);
  }
}
