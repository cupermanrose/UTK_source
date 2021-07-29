#ifndef _QUERY_GEN_H_
#define _QUERY_GEN_H_

#include "header.h"

void QueryGenerator(vector < vector<float> >& QryGen,  int q_num, const int dim, const float sigma);

vector<float> iPref_QueryGenerator(const int dim, const float sigma, vector<float> &w);

void GenerateData(int dim, int size);

void readQuery(vector<vector<float>>& QryGen, const char* queryfile, int q_num, int dim);

void readQuery2(vector<vector<float>>& QryGen, const char* queryfile, int q_num, int dim, float sigma);

void generate_query(int dim, int q_num, float utk_side_length, vector<vector<float>>& q_list);
#endif
