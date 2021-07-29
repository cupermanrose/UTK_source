//
// Created by 12859 on 2020/7/15.
//
#ifndef UTK_BV_UTK_MATH_LIB_IMPL_H
#define UTK_BV_UTK_MATH_LIB_IMPL_H

#include "utk_vector.h"
#include <vector>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <algorithm>


template<typename V>
inline c_float L2_norm(V &v) {
    c_float ret = c_float();
    for (auto &i:v) {
        ret += i * i;
    }
    return sqrt(ret);
}

template<typename VV>
VV gram_schmidt_process(const VV &input) {
    /*
     * be careful when:
     * e.g.
     *  input={a_0, a_1, a_2}
     *  if a_2 is a linear combination of {a_0, a_1},
     *   then ret[2] will be a vector of 0s
     */
    assert(!input.empty());
    VV ret(input.size());
    // begin generate orthogonal vectors
    for (int i = 0; i < input.size(); ++i) {
        ret[i] = input[i];
        for (int j = 0; j < i; ++j) {
            ret[i] -= proj(ret[j], input[i]);
        }
    }
    // begin normalize result
    for (int k = 0; k < ret.size(); ++k) {
        c_float l2_norm = L2_norm(ret[k]);
        for (auto &ele:ret[k]) {
            ele /= l2_norm;
        }
    }
    return ret;
}


template<typename V>
inline V proj(const V &u, const V &v) {
    return (u * v) / (u * u) * u;
}

template<typename INTEGER>
vector<vector<c_float>> gen_r_domain_basevec(INTEGER dim) {
    /*
     * \tpara INTEGER bit, byte, char, short, int, long, size_t, unsigned
     */
    vector<vector<c_float>> u(dim);
    u[0] = vector<c_float>(dim, 1.0);
    for (int i = 1; i < dim; ++i) {
        u[i] = vector<c_float>(dim);
        u[i][i] = 1.0;
    }
    vector<vector<c_float>> e = gram_schmidt_process(u);
    e.erase(e.begin());
    return e;
}

template<typename VV, typename V>
void gen_r_domain_vec_dfs(V &&cur, int next,
                          VV &ret, VV &e) {
    if (next == e.size()) {
        ret.push_back(cur);
    } else {
        gen_r_domain_vec_dfs(cur + e[next], next + 1, ret, e);
        gen_r_domain_vec_dfs(cur - e[next], next + 1, ret, e);
    }
}



#endif //UTK_BV_UTK_MATH_LIB_IMPL_H

