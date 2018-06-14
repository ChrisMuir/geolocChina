#include <Rcpp.h>
using namespace Rcpp;


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
                                          std::vector<std::string> dd) {
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
                                                 std::vector<std::string> dd_names,
                                                 int parent_code,
                                                 std::vector<int> dd_codes) {
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
                                std::vector<std::string> substrings) {
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


// Idea taken from:
// https://github.com/rstudio/webinars/blob/master/26-Profiling/Profiling.R#L105
// Given a named list of char vectors, extract all elements that have a given
// name, return as a char vector.
std::vector<std::string> extract_char_vector(List x, std::string name) {
  int x_len = x.size();
  std::vector<std::string> out(x_len);

  List curr_x;
  CharacterVector curr_x_names;
  for(int i = 0; i < x_len; ++i) {
    curr_x = x[i];
    curr_x_names = curr_x.attr("names");
    for(int n = 0; n < curr_x.size(); ++n) {
      if(curr_x_names[n] == name) {
        std::string curr_out = curr_x[n];
        out[i] = curr_out;
        break;
      }
    }
  }

  return(out);
}

// Idea taken from:
// https://github.com/rstudio/webinars/blob/master/26-Profiling/Profiling.R#L105
// Given a named list of int vectors, extract all elements that have a given
// name, return as an int vector.
std::vector<int> extract_int_vector(List x, std::string name) {
  int x_len = x.size();
  std::vector<int> out(x_len);

  List curr_x;
  CharacterVector curr_x_names;
  for(int i = 0; i < x_len; ++i) {
    curr_x = x[i];
    curr_x_names = curr_x.attr("names");
    for(int n = 0; n < curr_x.size(); ++n) {
      if(curr_x_names[n] == name) {
        int curr_out = curr_x[n];
        out[i] = curr_out;
        break;
      }
    }
  }

  return(out);
}


// Convert a Chinese location substring from the pkg data dict to its
// corresponding geocode.
int as_geocode(std::string cn_loc,
               std::vector<std::string> dd_strings,
               std::vector<int> dd_codes) {
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
                         std::vector<std::string> dd_strings,
                         std::vector<int> dd_codes) {
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
List get_locations(std::string cn_str,
                   std::vector<std::string> prov_dd_strings,
                   std::vector<int> prov_dd_codes,
                   std::vector<std::string> city_dd_strings,
                   std::vector<int> city_dd_codes,
                   std::vector<std::string> cnty_dd_strings,
                   std::vector<int> cnty_dd_codes,
                   std::vector<std::string> cnty_dd_strings_2015,
                   std::vector<int> cnty_dd_codes_2015) {

  List out = List::create(Named("province") = NA_STRING,
                          Named("city") = NA_STRING,
                          Named("county") = NA_STRING,
                          Named("province_code") = NA_INTEGER,
                          Named("city_code") = NA_INTEGER,
                          Named("county_code") = NA_INTEGER);

  // TODO: create is_empty(), to check for NA and missing values. Run check
  // here.

  // Look for a Province match.
  int curr_prov_code = NA_INTEGER;
  std::vector<std::string> provs = substring_lookup(cn_str, prov_dd_strings);
  if(provs.size() > 0) {
    std::string prov = get_earliest_substr(cn_str, provs);
    out["province"] = prov;
    curr_prov_code = as_geocode(prov, prov_dd_strings, prov_dd_codes);
    out["province_code"] = curr_prov_code;
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
    out["city"] = city;
    curr_city_code = as_geocode(city, city_dd_strings, city_dd_codes);
    out["city_code"] = curr_city_code;
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
    out["county"] = county;
    curr_cnty_code = as_geocode(county, cnty_dd_strings, cnty_dd_codes);
    out["county_code"] = curr_cnty_code;
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
      std::string city = as_geostring(curr_city_code, city_dd_strings,
                                      city_dd_codes);
      out["city"] = city;
      out["city_code"] = curr_city_code;

    }
  }

  // If City is NA and County is not NA, fill in City with the first four
  // digits from the County code.
  if (curr_cnty_code != NA_INTEGER and curr_city_code == NA_INTEGER) {
    curr_city_code = substr_int(curr_cnty_code, 0, 4);
    out["city_code"] = curr_city_code;
    std::string city = as_geostring(curr_city_code, city_dd_strings,
                                    city_dd_codes);
    out["city"] = city;
  }

  // If Province is NA and City is not NA, fill in Province with the first
  // two digits from the City code.
  if (curr_city_code != NA_INTEGER and curr_prov_code == NA_INTEGER) {
    curr_prov_code = substr_int(curr_city_code, 0, 2);
    out["province_code"] = curr_prov_code;
    std::string prov = as_geostring(curr_prov_code, prov_dd_strings,
                                    prov_dd_codes);
    out["province"] = prov;
  }

  return(out);
}


// Exported function. Takes a vector of Chinese location/address strings,
// sends them all through get_locations() in a loop. Returns a data frame
// of geolocation data related to each input string.
//[[Rcpp::export]]
DataFrame cpp_geo_locate(std::vector<std::string> cn_strings,
                         std::vector<std::string> prov_dd_strings,
                         std::vector<int> prov_dd_codes,
                         std::vector<std::string> city_dd_strings,
                         std::vector<int> city_dd_codes,
                         std::vector<std::string> cnty_dd_strings,
                         std::vector<int> cnty_dd_codes,
                         std::vector<std::string> cnty_dd_strings_2015,
                         std::vector<int> cnty_dd_codes_2015) {
  int cn_strings_len = cn_strings.size();
  List res(cn_strings_len);

  for(int i = 0; i < cn_strings_len; ++i) {
    std::string curr_cn_str = cn_strings[i];
    List geo_locs = get_locations(curr_cn_str, prov_dd_strings, prov_dd_codes,
                                  city_dd_strings, city_dd_codes,
                                  cnty_dd_strings, cnty_dd_codes,
                                  cnty_dd_strings_2015, cnty_dd_codes_2015);
    res[i] = geo_locs;
  }

  DataFrame out = DataFrame::create(
    Named("location") = cn_strings,
    Named("province") = extract_char_vector(res, "province"),
    Named("city") = extract_char_vector(res, "city"),
    Named("county") = extract_char_vector(res, "county"),
    Named("province_code") = extract_int_vector(res, "province_code"),
    Named("city_code") = extract_int_vector(res, "city_code"),
    Named("county_code") = extract_int_vector(res, "county_code")
  );

  return(out);
}
