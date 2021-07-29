#include "queryGen.h"

void QueryGenerator(vector < vector<float> >& QryGen, int q_num, const int dim, const float sigma)
{

    srand(0); // random seed
    QryGen.clear();
    for (int i=0;i<q_num;i++){
        vector<float> v(dim*2,0.0);
        float res=1.0-sigma*dim;
        for (int d=0;d<dim;d++){
            v[d*2] = res*rand()/RAND_MAX;
            v[d*2+1] = v[d*2]+sigma;
            res=res-v[d*2];
        }
        QryGen.push_back(v);
    }
    return;

    QryGen.clear();
    vector<float> region(2 * dim, 0);
    float query[20];
    srand(0);
    while (QryGen.size() < 50)
    {
        float sum = 0;
        for (int di = 0; di < dim; di++)
        {
            query[di] = rand() *1.0 / RAND_MAX;
            sum += query[di];
        }

        if (sum < 1 - sigma)
        {
            for (int di = 0; di < dim; di++)
            {
                region[di * 2] = query[di];
                region[di * 2 + 1] = query[di] + sigma;
            }
            QryGen.push_back(region);
        }
    }
}

void readQuery(vector<vector<float>>& QryGen, const char* queryfile, int q_num, int dim){
    fstream f;
    try {
        f.open(queryfile);
        fstream queryout(queryfile, ios::in);
        for (int i=0;i<q_num;i++){
            vector<float> tmp((dim-1)*2);
            for (int j=0;j<(dim-1)*2;j++) {
                queryout>>tmp[j];
            }
            QryGen.emplace_back(tmp);
        }
        f.close();
    } catch (exception& e) {
        f.close();
    }
}

void readQuery2(vector<vector<float>>& QryGen, const char* queryfile, int q_num, int dim, float sigma){
    fstream f;
    try {
        float t;
        f.open(queryfile);
        fstream queryout(queryfile, ios::in);
        for (int i=0;i<q_num;i++){
            vector<float> tmp((dim-1)*2);
            for (int j=0;j<(dim-1);j+=2) {
                queryout>>tmp[j];
                queryout>>t;
                tmp[j+1]=tmp[j]+sigma;
            }
            QryGen.emplace_back(tmp);
        }
        f.close();
    } catch (exception& e) {
        f.close();
    }
}

void generate_query(int dim, int q_num, float utk_side_length, vector<vector<float>>& q_list) {
    srand(1); // random seed
    q_list.clear();
    for (int i=0;i<q_num;i++){
        vector<float> v((dim-1)*2,0.0);
        int res=rand()%10000;
        cout << res << endl;
        for (int d=0;d<dim-1;d++){
            int tmp=rand() % res;
            v[d*2] = (float) tmp / 10000.0;
            v[d*2+1] = v[d*2]+utk_side_length;
            res=res-tmp;
        }
        q_list.push_back(v);
    }
    return;
}

void GenerateData(int dim, int size)
{
    srand(NULL);
    vector<float> record(dim, 0);
    for (int i = 0; i < size; i++)
    {
        for (int di = 0; di < dim; di++)
        {
            record[di] = rand() *1.0 / RAND_MAX;
        }
        cout << i+1 << " ";
        for (int di = 0; di < dim; di++)
        {
            printf("%.4f ", record[di] - 0.0001);
        }
        for (int di = 0; di < dim; di++)
        {
            printf("%.4f ", record[di] + 0.0001);
        }
        cout << endl;
    }
}