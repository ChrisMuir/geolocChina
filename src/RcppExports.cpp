// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// cpp_geo_locate
DataFrame cpp_geo_locate(const CharacterVector& cn_strings);
RcppExport SEXP _geolocChina_cpp_geo_locate(SEXP cn_stringsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const CharacterVector& >::type cn_strings(cn_stringsSEXP);
    rcpp_result_gen = Rcpp::wrap(cpp_geo_locate(cn_strings));
    return rcpp_result_gen;
END_RCPP
}
// cpp_get_package_data
DataFrame cpp_get_package_data();
RcppExport SEXP _geolocChina_cpp_get_package_data() {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    rcpp_result_gen = Rcpp::wrap(cpp_get_package_data());
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_geolocChina_cpp_geo_locate", (DL_FUNC) &_geolocChina_cpp_geo_locate, 1},
    {"_geolocChina_cpp_get_package_data", (DL_FUNC) &_geolocChina_cpp_get_package_data, 0},
    {NULL, NULL, 0}
};

RcppExport void R_init_geolocChina(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
