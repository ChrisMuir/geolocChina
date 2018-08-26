#include <Rcpp.h>
using namespace Rcpp;
#include <fstream>
#include <sstream>
#include "global_data.h"


// Fetch "cn_env" environment, which contains file path strings for all of the 
// package data files.
Environment geoloc_env = Environment::namespace_env("geolocChina");
Environment cn_env = geoloc_env["cn_env"];

// Read csv data from a txt file, return as a string vector.
static std::vector<std::string> read_file_str(String file_path) {
  std::vector<std::string> output;
  
  // Open the file.
  std::ifstream myfile(file_path.get_cstring());
  
  std::string line;
  int counter = 0;
  
  while(std::getline(myfile, line, '\n')) {
    if(counter % 1000 == 0) {
      Rcpp::checkUserInterrupt();
    }
    output.push_back(line);
    counter++;
  }
  
  return(output);
}

// Read csv data from a txt file, return as an int vector.
static std::vector<int> read_file_int(String file_path) {
  std::vector<int> output;
  
  // Open the file.
  std::ifstream myfile(file_path.get_cstring());
  
  std::string line;
  int counter = 0;
  
  while(std::getline(myfile, line, '\n')) {
    if(counter % 1000 == 0) {
      Rcpp::checkUserInterrupt();
    }
    output.push_back(atoi(line.c_str()));
    counter++;
  }
  
  return(output);
}


// Namespace to house all of the raw package data.
namespace raw_pkg_data {
  // Read in Provincial strings.
  const std::vector<std::string> prov_dd_strings = read_file_str(
    String(cn_env.get("prov_str_path"))
  );
  
  // Read in Provincial geocodes.
  const std::vector<int> prov_dd_codes = read_file_int(
    String(cn_env.get("prov_int_path"))
  );
  
  // Read in City strings.
  const std::vector<std::string> city_dd_strings = read_file_str(
    String(cn_env.get("city_str_path"))
  );
  
  // Read in City geocodes.
  const std::vector<int> city_dd_codes = read_file_int(
    String(cn_env.get("city_int_path"))
  );
  
  // Read in County strings.
  const std::vector<std::string> cnty_dd_strings = read_file_str(
    String(cn_env.get("cnty_str_path"))
  );
  
  // Read in County geocodes.
  const std::vector<int> cnty_dd_codes = read_file_int(
    String(cn_env.get("cnty_int_path"))
  );
  
  // Read in County 2015 strings.
  const std::vector<std::string> cnty_dd_strings_2015 = read_file_str(
    String(cn_env.get("cnty_2015_str_path"))
  );
  
  // Read in County 2015 geocodes.
  const std::vector<int> cnty_dd_codes_2015 = read_file_int(
    String(cn_env.get("cnty_2015_int_path"))
  );
}


// Create objects used by functions in utils.cpp. These data objects are 
// created by iterating over the raw package data. Objects created are:
// prov_set: unordered_set of provincial strings.
// city_set: unordered_set of city strings.
// cnty_set: unordered_set of county strings.
// cnty_2015_set: unordered_set of county strings from 2015.
// city_code_set: unordered_set of city geocode integers.
// pkg_data_set_lens: int vector containing all unique char lens of each 
//                    package data string.
// int_to_str_map: unordered_map containing string representations of each 
//                 integer value in the package data. Save strings as values in
//                 the unordered_map, with ints as the keys.
iter_pkg_data get_supp_pkg_containers() {
  // Initialize output variables.
  std::unordered_map<int, std::string> int_to_str_map;
  std::unordered_set<std::string> prov_set;
  std::unordered_set<std::string> city_set;
  std::unordered_set<std::string> cnty_set;
  std::unordered_set<std::string> cnty_2015_set;
  std::unordered_set<int> city_code_set;
  
  std::unordered_set<int> all_lens;
  
  // Provincial data.
  for(unsigned int i = 0; i < raw_pkg_data::prov_dd_codes.size(); ++i) {
    all_lens.insert(raw_pkg_data::prov_dd_strings[i].size());
    int_to_str_map[raw_pkg_data::prov_dd_codes[i]] = std::to_string(raw_pkg_data::prov_dd_codes[i]);
    prov_set.insert(raw_pkg_data::prov_dd_strings[i]);
  }
  
  // City data.
  for(unsigned int i = 0; i < raw_pkg_data::city_dd_codes.size(); ++i) {
    all_lens.insert(raw_pkg_data::city_dd_strings[i].size());
    int_to_str_map[raw_pkg_data::city_dd_codes[i]] = std::to_string(raw_pkg_data::city_dd_codes[i]);
    city_set.insert(raw_pkg_data::city_dd_strings[i]);
    city_code_set.insert(raw_pkg_data::city_dd_codes[i]);
  }
  
  // County data.
  for(unsigned int i = 0; i < raw_pkg_data::cnty_dd_codes.size(); ++i) {
    all_lens.insert(raw_pkg_data::cnty_dd_strings[i].size());
    int_to_str_map[raw_pkg_data::cnty_dd_codes[i]] = std::to_string(raw_pkg_data::cnty_dd_codes[i]);
    cnty_set.insert(raw_pkg_data::cnty_dd_strings[i]);
  }
  
  // 2015 County data.
  for(unsigned int i = 0; i < raw_pkg_data::cnty_dd_codes_2015.size(); ++i) {
    all_lens.insert(raw_pkg_data::cnty_dd_strings_2015[i].size());
    int_to_str_map[raw_pkg_data::cnty_dd_codes_2015[i]] = std::to_string(raw_pkg_data::cnty_dd_codes_2015[i]);
    cnty_2015_set.insert(raw_pkg_data::cnty_dd_strings_2015[i]);
  }
  
  // Initialize output int vector.
  std::vector<int> pkg_data_str_lens(all_lens.begin(), all_lens.end());
  
  // Sort int vector.
  std::sort(pkg_data_str_lens.begin(), pkg_data_str_lens.end());
  
  return {
    pkg_data_str_lens, 
    int_to_str_map, 
    prov_set, 
    city_set, 
    cnty_set, 
    cnty_2015_set, 
    city_code_set
  };
}


// Namespace to house supplemental package data objects. These are 
// objects used by the functions in utils.cpp, and are built from the raw 
// package data.
namespace supp_pkg_data {
  // Record lengths of the raw pkg data vectors.
  const int &prov_dd_len = raw_pkg_data::prov_dd_strings.size();
  const int &city_dd_len = raw_pkg_data::city_dd_strings.size();
  const int &cnty_dd_len = raw_pkg_data::cnty_dd_strings.size();
  const int &cnty_dd_2015_len = raw_pkg_data::cnty_dd_strings_2015.size();
  
  // Create seven container objects.
  iter_pkg_data containers = get_supp_pkg_containers();
}


// Namespace to house na_list object.
namespace rcpp_na_list {
  // Define object "na_list". This is a list of NA values that corresponds to 
  // the schema of the output data frame (province, city, county, province_code,
  // city_code, county_code).
  List na_list = List::create(NA_STRING, NA_STRING, NA_STRING, 
                              NA_INTEGER, NA_INTEGER, NA_INTEGER);
}
