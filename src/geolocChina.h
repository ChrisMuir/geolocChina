#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::plugins(cpp11)]]


#ifndef GEOLOCCHINA_H
#define GEOLOCCHINA_H

// utils.cpp
void get_locations(const std::string &cn_str, 
                   std::unordered_map<std::string, int> &substr_map, 
                   std::string &matches, 
                   List &out);

void substring_lookup_prov(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter);

void substring_lookup_city(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter);

void substring_lookup_cnty(std::string &matches, 
                           std::unordered_map<std::string, int> &substr_map, 
                           const int &cn_str_len, 
                           std::unordered_map<std::string, int>::iterator &map_end, 
                           std::unordered_map<std::string, int>::iterator &map_iter, 
                           std::unordered_set<std::string>::iterator &set_iter);

void substring_lookup_cnty_2015(std::string &matches, 
                                std::unordered_map<std::string, int> &substr_map, 
                                const int &cn_str_len, 
                                std::unordered_map<std::string, int>::iterator &map_end, 
                                std::unordered_map<std::string, int>::iterator &map_iter, 
                                std::unordered_set<std::string>::iterator &set_iter);

void substring_lookup_city_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len);

void substring_lookup_cnty_w_code(const int &parent_code, 
                                  std::string &matches, 
                                  std::unordered_map<std::string, int> &substr_map, 
                                  const int &cn_str_len);

void get_all_substr(const std::string &input, 
                    std::unordered_map<std::string, int> &substr_map);

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
