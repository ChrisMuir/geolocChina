#include <Rcpp.h>
using namespace Rcpp;
#include "geolocChina.h"


// cpp version of base::grepl(pattern, string, fixed = TRUE)
bool grepl_fixed(std::string pat, std::string term) {
  int pat_len = pat.size();
  int term_len = term.size();
  int term_less_pat = term_len - pat_len;

  if(term_less_pat < 0) {
    return FALSE;
  }

  if(term_less_pat == 0) {
    return pat == term;
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
        return TRUE;
      }
    }
  }
  return FALSE;
}


// Given a string (cn_str), look up each provincial string in cn_str (treating 
// the province strings as substrings).
std::vector<std::string> substring_lookup_prov(std::string cn_str) {
  std::vector<std::string> out;
  std::string curr_dd;
  for(int i = 0; i < prov_dd_len; i++) {
    curr_dd = prov_dd_strings[i];
    if(grepl_fixed(curr_dd, cn_str)) {
      out.push_back(curr_dd);
    }
  }
  
  return(out);
}


// Given a string (cn_str), look up each city string in cn_str (treating the 
// city strings as substrings).
std::vector<std::string> substring_lookup_city(std::string cn_str) {
  std::vector<std::string> out;
  std::string curr_dd;
  for(int i = 0; i < city_dd_len; i++) {
    curr_dd = city_dd_strings[i];
    if(grepl_fixed(curr_dd, cn_str)) {
      out.push_back(curr_dd);
    }
  }
  
  return(out);
}


// Given a string (cn_str), look up each county string in cn_str (treating the 
// county strings as substrings).
std::vector<std::string> substring_lookup_cnty(std::string cn_str) {
  std::vector<std::string> out;
  std::string curr_dd;
  for(int i = 0; i < cnty_dd_len; i++) {
    curr_dd = cnty_dd_strings[i];
    if(grepl_fixed(curr_dd, cn_str)) {
      out.push_back(curr_dd);
    }
  }
  
  return(out);
}


// Given a string (cn_str), look up each county_2015 string in cn_str (treating
// the county_2015 strings as substrings).
std::vector<std::string> substring_lookup_cnty_2015(std::string cn_str) {
  std::vector<std::string> out;
  std::string curr_dd;
  for(int i = 0; i < cnty_dd_2015_len; i++) {
    curr_dd = cnty_dd_strings_2015[i];
    if(grepl_fixed(curr_dd, cn_str)) {
      out.push_back(curr_dd);
    }
  }
  
  return(out);
}


// Given a string (cn_str), look up each city string in cn_str (treating the 
// city strings as substrings). Also takes an int provincial code as input, 
// used for validating string matches.
std::vector<std::string> substring_lookup_city_w_code(std::string cn_str,
                                                      int parent_code) {
  std::vector<std::string> out;
  std::string pc_str = std::to_string(parent_code);
  //int pc_str_len = pc_str.size();

  std::string curr_city_str;
  std::string curr_city_code;
  for(int i = 0; i < city_dd_len; i++) {
    curr_city_str = city_dd_strings[i];
    if(grepl_fixed(curr_city_str, cn_str)) {
      curr_city_code = std::to_string(city_dd_codes[i]);
      if(pc_str == curr_city_code.substr(0, 2)) {
        out.push_back(curr_city_str);
      }
    }
  }
  
  return(out);
}


// Given a string (cn_str), look up each county string in cn_str (treating the 
// county strings as substrings). Also takes an int geo code as input (either 
// provincial code or city code), used for validating string matches.
std::vector<std::string> substring_lookup_cnty_w_code(std::string cn_str,
                                                      int parent_code) {
  std::vector<std::string> out;
  std::string pc_str = std::to_string(parent_code);
  int pc_str_len = pc_str.size();

  std::string curr_cnty_str;
  std::string curr_cnty_code;
  for(int i = 0; i < cnty_dd_len; i++) {
    curr_cnty_str = cnty_dd_strings[i];
    if(grepl_fixed(curr_cnty_str, cn_str)) {
      curr_cnty_code = std::to_string(cnty_dd_codes[i]);
      if(pc_str == curr_cnty_code.substr(0, pc_str_len)) {
        out.push_back(curr_cnty_str);
      }
    }
  }
  
  return(out);
}


// Get numeric index of a substring pattern with a string.
int substring_index(std::string term, std::string pat) {
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
std::string get_earliest_substr(std::string term,
                                const std::vector<std::string> &substrings) {
  int substrings_len = substrings.size();
  IntegerVector idx(substrings_len);

  for(int i = 0; i < substrings_len; ++i) {
    idx[i] = substring_index(term, substrings[i]);
  }

  return(substrings[which_min(idx)]);
}


// Given an int, return a portion of the digits in int (a "substring" of int).
int substr_int(int x, int start, int out_len) {
  std::string x_str = std::to_string(x);
  return(atoi(x_str.substr(start, out_len).c_str()));
}


// Given a list of lists, extract the idx'th element  
// from each inner list, return as a char vector.
CharacterVector extract_char_vector(const List &x, int idx) {
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
std::vector<int> extract_int_vector(const List &x, int idx) {
  int x_len = x.size();
  std::vector<int> out(x_len);
  
  List curr_x;
  for(int i = 0; i < x_len; ++i) {
    curr_x = x[i];
    out[i] = curr_x[idx];
  }
  
  return(out);
}


// Convert a provincial string from the pkg data dict to its
// corresponding geocode.
int as_geocode_prov(std::string cn_loc) {
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
int as_geocode_city(std::string cn_loc) {
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
int as_geocode_cnty(std::string cn_loc) {
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
int as_geocode_cnty_2015(std::string cn_loc) {
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
std::string as_geostring_prov(int code) {
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
std::string as_geostring_city(int code) {
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


// Given an input Chinese location/address string, determine the Province,
// City, and County of the input string (and the assoicated geocodes for all
// three), return the six values as a named list.
List get_locations(const std::string &cn_str) {

  List out = clone(na_res);
  
  // Look for a Province match.
  int curr_prov_code = NA_INTEGER;
  std::vector<std::string> provs = substring_lookup_prov(cn_str);
  if(provs.size() > 0) {
    std::string prov = get_earliest_substr(cn_str, provs);
    out[0] = prov;
    curr_prov_code = as_geocode_prov(prov);
    out[3] = curr_prov_code;
  }

  // Look for a City match.
  std::vector<std::string> cities;
  if(curr_prov_code == NA_INTEGER) {
    cities = substring_lookup_city(cn_str);
  } else {
    cities = substring_lookup_city_w_code(cn_str, curr_prov_code);
  }

  int curr_city_code = NA_INTEGER;
  if(cities.size() > 0) {
    std::string city = get_earliest_substr(cn_str, cities);
    out[1] = city;
    curr_city_code = as_geocode_city(city);
    out[4] = curr_city_code;
  }

  // Look for County match.
  std::vector<std::string> cntys;
  if(curr_prov_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    cntys = substring_lookup_cnty(cn_str);
  } else if(curr_city_code == NA_INTEGER) {
    cntys = substring_lookup_cnty_w_code(cn_str, curr_prov_code);
  } else {
    cntys = substring_lookup_cnty_w_code(cn_str, curr_city_code);
  }

  int curr_cnty_code = NA_INTEGER;
  if (cntys.size() > 0) {
    std::string county = get_earliest_substr(cn_str, cntys);
    out[2] = county;
    curr_cnty_code = as_geocode_cnty(county);
    out[5] = curr_cnty_code;
  }

  //// Try to fill in any missing codes.

  // If City is NA and County is NA, try to fill in the City code/string from
  // the county_2015 vectors.
  if(curr_cnty_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    cntys = substring_lookup_cnty_2015(cn_str);

    if(cntys.size() > 0) {
      std::string county = get_earliest_substr(cn_str, cntys);
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
  if (curr_cnty_code != NA_INTEGER and curr_city_code == NA_INTEGER) {
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
  if (curr_city_code != NA_INTEGER and curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    out[3] = curr_prov_code;
    std::string prov = as_geostring_prov(curr_prov_code);
    out[0] = prov;
  }

  return(out);
}


// Exported function. Takes as input a char vector of Chinese strings, sends 
// each one through get_locations() in a loop (along with the geolocChina 
// package data). Returns a data frame of geolocation data related to each 
// input string.
//[[Rcpp::export]]
DataFrame cpp_geo_locate(const CharacterVector &cn_strings) {
  
  int cn_strings_len = cn_strings.size();
  
  // If input cn_strings is all NA values, then return a data frame full of NA
  // values.
  if(is_true(all(is_na(cn_strings)))) {
    return(get_na_dataframe(cn_strings_len));
  }
  
  List res(cn_strings_len);
  std::string curr_cn_str;
  List geo_locs;
  
  // Loop over cn_strings, get geolocations and geocodes for each string.
  for(int i = 0; i < cn_strings_len; ++i) {
    if(CharacterVector::is_na(cn_strings[i])) {
      res[i] = na_res;
    } else {
      curr_cn_str = as<std::string>(cn_strings[i]);
      geo_locs = get_locations(curr_cn_str);
      res[i] = geo_locs;
    }
  }
  
  DataFrame out = DataFrame::create(
    Named("location") = cn_strings,
    Named("province") = extract_char_vector(res, 0),
    Named("city") = extract_char_vector(res, 1),
    Named("county") = extract_char_vector(res, 2),
    Named("province_code") = extract_int_vector(res, 3),
    Named("city_code") = extract_int_vector(res, 4),
    Named("county_code") = extract_int_vector(res, 5), 
    Named("stringsAsFactors") = false
  );
  
  return(out);
}
