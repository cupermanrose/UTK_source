//
// Created by 12859 on 2020/7/15.
//

#ifndef UTK_BV_UTK_MATH_LIB_H
#define UTK_BV_UTK_MATH_LIB_H
#include "utk_vector.h"
#include <set>


template<typename V>
inline V proj(const V &u, const V &v);

template<typename VV>
VV gram_schmidt_process(const VV &input);
template<typename V>
inline c_float L2_norm(V &v);

template<typename INTEGER>
vector<vector<c_float>> gen_r_domain_basevec(INTEGER dim);

template<typename VV, typename V>
void gen_r_domain_vec_dfs(V &&cur, int next,
                          VV &ret, VV &e);




#include "utk_math_lib_impl.h"
//the implementation (definition) of a template class
// or function must be in the same file as its declaration.

// the template fun and non-template fun are not suggested to be compile together
#endif //UTK_BV_UTK_MATH_LIB_H
