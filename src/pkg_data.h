#include <Rcpp.h>
using namespace Rcpp;
#include <fstream>
#include <sstream>


#ifndef PKG_DATA_H
#define PKG_DATA_H

// Read csv data from a txt file, return as a string vector.
static std::vector<std::string> read_file_str(String file_path) {
  std::vector<std::string> output;
  
  // Open the file.
  std::ifstream myfile(file_path.get_cstring());
  
  std::string line;
  int counter = 0;
  
  while(std::getline(myfile, line, '\n')) {
    if(counter % 10000 == 0) {
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
    if(counter % 10000 == 0) {
      Rcpp::checkUserInterrupt();
    }
    output.push_back(atoi(line.c_str()));
    counter++;
  }
  
  return(output);
}


//// Read in package data.
// Fetch "cn_env" environment, which contains file path strings for all of the 
// package data files.
Environment geoloc_env = Environment::namespace_env("geolocChina");
Environment cn_env = geoloc_env["cn_env"];

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

// Record lengths of some of the pkg data vectors.
const int &prov_dd_len = prov_dd_strings.size();
const int &city_dd_len = city_dd_strings.size();
const int &cnty_dd_len = cnty_dd_strings.size();
const int &cnty_dd_2015_len = cnty_dd_strings_2015.size();

// Create set of city codes, to use as a reference when attempting to assign 
// a city code from a matched county code.
const std::unordered_set<int> city_code_set(city_dd_codes.begin(), 
                                            city_dd_codes.end());

#endif // PKG_DATA_H
