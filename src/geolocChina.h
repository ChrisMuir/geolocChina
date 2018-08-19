#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins(cpp11)]]


#ifndef GEOLOCCHINA_H
#define GEOLOCCHINA_H

// utils.cpp
List get_locations(const std::string &cn_str, 
                   std::unordered_set<std::string> &substr_set, 
                   std::vector<std::string> &matches);

void substring_lookup_prov(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set);

void substring_lookup_city(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set);

void substring_lookup_cnty(const std::string &cn_str, 
                           std::vector<std::string> &matches, 
                           std::unordered_set<std::string> &substr_set);

void substring_lookup_cnty_2015(const std::string &cn_str, 
                                std::vector<std::string> &matches, 
                                std::unordered_set<std::string> &substr_set);

void substring_lookup_city_w_code(const std::string &cn_str, 
                                  const int &parent_code, 
                                  std::vector<std::string> &matches, 
                                  std::unordered_set<std::string> &substr_set);

void substring_lookup_cnty_w_code(const std::string &cn_str, 
                                  const int &parent_code, 
                                  std::vector<std::string> &matches, 
                                  std::unordered_set<std::string> &substr_set);

void get_all_substr(const std::string &input, 
                    std::unordered_set<std::string> &substr_set);

std::string get_earliest_substr(const std::string &term,
                                const std::vector<std::string> &substrings);

int substr_int(const int &x, const int &start, const int &out_len);

int as_geocode_prov(const std::string &cn_loc);

int as_geocode_city(const std::string &cn_loc);

int as_geocode_cnty(const std::string &cn_loc);

int as_geocode_cnty_2015(const std::string &cn_loc);

std::string as_geostring_prov(const int &code);

std::string as_geostring_city(const int &code);

DataFrame get_na_dataframe(const int &x);

CharacterVector extract_char_vector(const List &x, const int &idx);

IntegerVector extract_int_vector(const List &x, const int &idx);


#endif // GEOLOCCHINA_H
