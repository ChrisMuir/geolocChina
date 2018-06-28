#include <Rcpp.h>
using namespace Rcpp;


// [[Rcpp::plugins(cpp11)]]


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


// Given a string (cn_str) and a vector of strings, look up each vector
// element in cn_str (treating the vector elements as substrings).
std::vector<std::string> substring_lookup(std::string cn_str,
                                          const std::vector<std::string> &dd) {
  int dd_len = dd.size();
  std::vector<std::string> out;

  std::string curr_dd;
  for(int i = 0; i < dd_len; i++) {
    curr_dd = dd[i];
    if(grepl_fixed(curr_dd, cn_str)) {
      out.push_back(curr_dd);
    }
  }

  return(out);
}


// Given a string (cn_str) and a vector of strings, look up each vector
// element in cn_str (treating the vector elements as substrings). Also takes
// int values as input, used for validating string matches.
std::vector<std::string> substring_lookup_w_code(std::string cn_str,
                                                 const std::vector<std::string> &dd_names,
                                                 int parent_code,
                                                 const std::vector<int> &dd_codes) {
  int dd_len = dd_names.size();
  std::vector<std::string> out;
  std::string pc_str = std::to_string(parent_code);
  int pc_str_len = pc_str.size();

  std::string curr_dd_str;
  std::string curr_dd_code;
  for(int i = 0; i < dd_len; i++) {
    curr_dd_str = dd_names[i];
    if(grepl_fixed(curr_dd_str, cn_str)) {
      curr_dd_code = std::to_string(dd_codes[i]);
      if(pc_str == curr_dd_code.substr(0, pc_str_len)) {
        out.push_back(curr_dd_str);
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


// Given a list of lists, extract the element with index == idx  
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


// Given a list of lists, extract the element with index == idx 
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


// Convert a Chinese location substring from the pkg data dict to its
// corresponding geocode.
int as_geocode(std::string cn_loc,
               const std::vector<std::string> &dd_strings,
               const std::vector<int> &dd_codes) {
  int dd_len = dd_strings.size();
  int out = 0;

  for(int i = 0; i < dd_len; ++i) {
    if(dd_strings[i] == cn_loc) {
      out = dd_codes[i];
      break;
    }
  }

  return(out);
}


// Convert a Chinese location geocode from the pkg data dict to its
// corresponding location string.
std::string as_geostring(int code,
                         const std::vector<std::string> &dd_strings,
                         const std::vector<int> &dd_codes) {
  int dd_len = dd_strings.size();
  std::string out;

  for(int i = 0; i < dd_len; ++i) {
    if(dd_codes[i] == code) {
      out = dd_strings[i];
      break;
    }
  }

  return(out);
}


// Given an input Chinese location/address string, determine the Province,
// City, and County of the input string (and the assoicated geocodes for all
// three), return the six values as a named list.
List get_locations(const std::string &cn_str,
                   const std::vector<std::string> &prov_dd_strings,
                   const std::vector<int> &prov_dd_codes,
                   const std::vector<std::string> &city_dd_strings,
                   const std::vector<int> &city_dd_codes,
                   const std::vector<std::string> &cnty_dd_strings,
                   const std::vector<int> &cnty_dd_codes,
                   const std::vector<std::string> &cnty_dd_strings_2015,
                   const std::vector<int> &cnty_dd_codes_2015, 
                   const std::unordered_set<int> &city_code_set) {

  List out = List::create(NA_STRING, NA_STRING, NA_STRING, NA_INTEGER, 
                          NA_INTEGER, NA_INTEGER);

  // TODO: create is_empty(), to check for NA and missing values. Run check
  // here.

  // Look for a Province match.
  int curr_prov_code = NA_INTEGER;
  std::vector<std::string> provs = substring_lookup(cn_str, prov_dd_strings);
  if(provs.size() > 0) {
    std::string prov = get_earliest_substr(cn_str, provs);
    out[0] = prov;
    curr_prov_code = as_geocode(prov, prov_dd_strings, prov_dd_codes);
    out[3] = curr_prov_code;
  }

  // Look for a City match.
  std::vector<std::string> cities;
  if(curr_prov_code == NA_INTEGER) {
    cities = substring_lookup(cn_str, city_dd_strings);
  } else {
    cities = substring_lookup_w_code(cn_str, city_dd_strings,
                                     curr_prov_code, city_dd_codes);
  }

  int curr_city_code = NA_INTEGER;
  if(cities.size() > 0) {
    std::string city = get_earliest_substr(cn_str, cities);
    out[1] = city;
    curr_city_code = as_geocode(city, city_dd_strings, city_dd_codes);
    out[4] = curr_city_code;
  }

  // Look for County match.
  std::vector<std::string> cntys;
  if(curr_prov_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    cntys = substring_lookup(cn_str, cnty_dd_strings);
  } else if(curr_city_code == NA_INTEGER) {
    cntys = substring_lookup_w_code(cn_str, cnty_dd_strings,
                                    curr_prov_code, cnty_dd_codes);
  } else {
    cntys = substring_lookup_w_code(cn_str, cnty_dd_strings,
                                    curr_city_code, cnty_dd_codes);
  }

  int curr_cnty_code = NA_INTEGER;
  if (cntys.size() > 0) {
    std::string county = get_earliest_substr(cn_str, cntys);
    out[2] = county;
    curr_cnty_code = as_geocode(county, cnty_dd_strings, cnty_dd_codes);
    out[5] = curr_cnty_code;
  }

  //// Try to fill in any missing codes.

  // If City is NA and County is NA, try to fill in the City code/string from
  // the county_2015 vectors.
  if(curr_cnty_code == NA_INTEGER and curr_city_code == NA_INTEGER) {
    cntys = substring_lookup(cn_str, cnty_dd_strings_2015);

    if(cntys.size() > 0) {
      std::string county = get_earliest_substr(cn_str, cntys);
      int init_code = as_geocode(county, cnty_dd_strings_2015,
                                 cnty_dd_codes_2015);
      curr_city_code = substr_int(init_code, 0, 4);
      
      // If curr_city_code appears in the city_code_set, use it and its 
      // associated city string as outputs.
      if(city_code_set.find(curr_city_code) != city_code_set.end()) {
        std::string city = as_geostring(curr_city_code, city_dd_strings,
                                        city_dd_codes);
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
      std::string city = as_geostring(curr_city_code, city_dd_strings,
                                      city_dd_codes);
      out[1] = city;
    }
  }

  // If Province is NA and City is not NA, fill in Province with the first
  // two digits from the City code.
  if (curr_city_code != NA_INTEGER and curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    out[3] = curr_prov_code;
    std::string prov = as_geostring(curr_prov_code, prov_dd_strings,
                                    prov_dd_codes);
    out[0] = prov;
  }

  return(out);
}


// Exported function. Takes as input a char vector of Chinese strings, sends 
// each one through get_locations() in a loop (along with the geolocChina 
// package data). Returns a data frame of geolocation data related to each 
// input string.
//[[Rcpp::export]]
DataFrame cpp_geo_locate(const CharacterVector &cn_strings, 
                         const Environment &cn_env) {
  
  // Get package data frame from cn_env.
  DataFrame geo_data = cn_env.get("geo_data");
  
  // Unpack cols of data from geo_data df.
  std::vector<std::string> geo_type = geo_data["geo_type"];
  std::vector<std::string> geo_name = geo_data["geo_name"];
  std::vector<int> geo_code = geo_data["geo_code"];
  
  // Get counts for each level in col "geo_type".
  int prov_len = geo_data.attr("prov_len");
  int city_len = geo_data.attr("city_len");
  int cnty_len = geo_data.attr("cnty_len");
  int cnty_2015_len = geo_data.attr("cnty_2015_len");
  
  // Unpack values from geo_data df into vectors.
  std::vector<std::string> prov_dd_strings(prov_len);
  std::vector<int> prov_dd_codes(prov_len);
  std::vector<std::string> city_dd_strings(city_len);
  std::vector<int> city_dd_codes(city_len);
  std::vector<std::string> cnty_dd_strings(cnty_len);
  std::vector<int> cnty_dd_codes(cnty_len);
  std::vector<std::string> cnty_dd_strings_2015(cnty_2015_len);
  std::vector<int> cnty_dd_codes_2015(cnty_2015_len);
  
  int prov_count = 0;
  int city_count = 0;
  int cnty_count = 0;
  int cnty_2015_count = 0;
  
  int geo_data_nrow = geo_data.nrow();
  
  for(int i = 0; i < geo_data_nrow; ++i) {
    if(geo_type[i] == "county") {
      cnty_dd_strings[cnty_count] = geo_name[i];
      cnty_dd_codes[cnty_count] = geo_code[i];
      cnty_count++;
    } else if (geo_type[i] == "city") {
      city_dd_strings[city_count] = geo_name[i];
      city_dd_codes[city_count] = geo_code[i];
      city_count++;
    } else if (geo_type[i] == "province") {
      prov_dd_strings[prov_count] = geo_name[i];
      prov_dd_codes[prov_count] = geo_code[i];
      prov_count++;
    } else if (geo_type[i] == "county_2015") {
      cnty_dd_strings_2015[cnty_2015_count] = geo_name[i];
      cnty_dd_codes_2015[cnty_2015_count] = geo_code[i];
      cnty_2015_count++;
    }
  }
  
  int cn_strings_len = cn_strings.size();
  List res(cn_strings_len);
  
  // Create set of city codes, to use as a reference when attempting to assign 
  // a city code from a matched county code.
  std::unordered_set<int> city_code_set;
  for(int n = 0; n < city_dd_codes.size(); ++n) {
    city_code_set.insert(city_dd_codes[n]);
  }
  
  // Loop over cn_strings, get geolocations and geocodes for each string.
  for(int i = 0; i < cn_strings_len; ++i) {
    std::string curr_cn_str = as<std::string>(cn_strings[i]);
    List geo_locs = get_locations(curr_cn_str, prov_dd_strings, prov_dd_codes,
                                  city_dd_strings, city_dd_codes,
                                  cnty_dd_strings, cnty_dd_codes,
                                  cnty_dd_strings_2015, cnty_dd_codes_2015, 
                                  city_code_set);
    res[i] = geo_locs;
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