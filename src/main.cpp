/*
----------------------- UTK problem -----------------
------------Author: Bo Tang @ SUSTech----------------
------------Email: tangloner@gmail.com---------------
*/

#include "header.h"
#include "hypercube.h"
#include "rentry.h"
#include "rnode.h"
#include "rtree.h"
#include "filemem.h"
#include "tgs.h"
#include "global.h"
#include "skyline.h"
#include "cellTree.h"
#include "param.h"
#include "utk.h"
#include "queryGen.h"

void initContainer(int& insertCount, vector<cell*>& finalResult, vector<long>& newAddSL, multimap<float, int>& heap,
                   unordered_set<long int>& skylines, unordered_set<long int>& removeSL, unordered_set<long int>& singular,
                   unordered_map<long int, long int>& RecordIDtoHalfPlaneID, vector<vector<float>>& HalfSpaces, unordered_set<long int>& ignorerecords);

// gobal variables
vector<vector<float>> HalfSpaces; // halfspace 
unordered_map<long int, long int> RecordIDtoHalfPlaneID;  //  record ID to halfplane ID
unordered_map<long int, RtreeNode*> ramTree; // load Rtree to main-memory
unordered_map<long int, cell*> cellID; // cell ID
unordered_set<long int> finalRetOptions; // it stores the final top-k results in utk2.

int objCnt = 0; // # of data objects
double totalIO = 0; // # of IO access
double totalSpaceCost = 0.0; // space cost (MB)
double treeSpacecost = 0.0;
double Spacecost = 0.0;

int main(const int argc, const char** argv)
{
    cout.precision(4);
    cout << "UTK for VLDB" << endl;
    clock_t at, ad;
    // parameter parser
    cout << "Parse Parameters" << endl;
    if (argc == 1)
    {
        helpmsg(argv[0]);
        return -1;
    }

    int dim = atoi(Param::read(argc, argv, "-d", ""));
    int k = atoi(Param::read(argc, argv, "-k", ""));
    const char* datafile = Param::read(argc, argv, "-f", "");
    const char* indexfile = Param::read(argc, argv, "-i", "");
    const char* queryfile = Param::read(argc, argv, "-q", "");
    float sigma = atof(Param::read(argc, argv, "-R", ""));
    const char* methodName = Param::read(argc, argv, "-m", "");
    int resultSize = 0;



    vector< vector<float> >  Queryregions;
//    void generate_query(int dim, int q_num, float utk_side_length, vector<vector<float>>& q_list)
    generate_query(dim, 50, sigma, Queryregions);
//    QueryGenerator(Queryregions, 20, dim - 1, sigma);
//    readQuery2(Queryregions, queryfile, 7, dim, sigma);
    // data loading
    cout << "Load data points from file" << endl;
    float** PointSet = new float*[MAXPTS + 1];
    RtreeNodeEntry** p = new RtreeNodeEntry*[MAXPTS];
    fstream fpdata;
    fpdata.open(datafile, ios::in);
    while (true)
    {
        int id;
        float* cl = new float[dim];
        float* cu = new float[dim];
        fpdata >> id;
        if (fpdata.eof())
            break;

        PointSet[objCnt + 1] = new float[2 * dim];

        for (int d = 0; d < dim; d++)
        {
            fpdata >> cl[d];
            PointSet[objCnt + 1][d] = cl[d];
        }

        for (int d = 0; d < dim; d++)
        {
            fpdata >> cu[d];
            PointSet[objCnt + 1][d + dim] = cu[d];
        }

        Hypercube hc(dim, cl, cu);
        p[objCnt++] = new RtreeNodeEntry(id, hc);

        //log information
        if (objCnt % 1000 == 0)
            cout << ".";
        if (objCnt % 10000 == 0)
            cout << objCnt << " objects loaded" << endl;
    }

    double rawSize = dataSize(objCnt, dim);
    cout << "Total number of objects: " << objCnt << endl;
    totalSpaceCost += rawSize;

    // build rtree
    cout << "Bulkloading R-tree..." << endl;
    const int maxChild = (PAGESIZE - RtreeNode::size()) / RtreeNodeEntry::size(dim);
    FileMemory mem(PAGESIZE, indexfile, RtreeNodeEntry::fromMem, true);
    Rtree* rtree = TGS::bulkload(mem, dim, maxChild, maxChild, (int)maxChild*0.3, (int)maxChild*0.3, p, objCnt, false);
    cout << "[Rtree build done]" << endl;

    rtreeRAM(*rtree, ramTree);
    totalSpaceCost += ramTree.size()*4096.00 / MB;


    vector<float> regions;

    clock_t alls, alle;
    alls = clock();
    int cnt=0;
    for (int qi = 0; qi < Queryregions.size(); qi++)
    {
//        try {
            cout << "Test Query Region: " << qi << endl;
            if (qi==45) continue;
            treeSpacecost = 0.0;
            regions = Queryregions[qi];
            unordered_map<int, cell *> utkRet;
            Point pt;
            int Noofdominator;
            int updatek;
            unordered_set<long int> skylines;
            unordered_set<long int> ignorerecords;
            unordered_set<long int> removeSL;
            unordered_set<long int> singular;
            multimap<float, int> heap;
            vector<long> newAddSL;
            int insertCount = 0;
            vector<cell *> leaves;
            vector<cell *> finalResult;
            utkRet.clear();
            finalRetOptions.clear();
            if (strcmp(methodName, "BB") == 0) {
                at = clock();
                vector<long int> skyband;
                kskyband(dim, *rtree, skyband, PointSet, k);
                cout << "k-skyband size: " << skyband.size() << endl;
                totalSpaceCost += skyband.size() * 4.00 / MB;

                for (int i = 0; i < skyband.size(); i++) {
                    cout << "============================================" << endl;
                    cout << "No. " << i + 1 << ", Record ID: " << skyband[i] << endl;

                    // init containers
                    initContainer(insertCount, finalResult, newAddSL, heap, skylines, removeSL, singular,
                                  RecordIDtoHalfPlaneID, HalfSpaces, ignorerecords);

                    // init focal record
                    pt.m_dimen = dim;
                    for (int j = 0; j < dim; j++) {
                        pt.m_coor[j] = (PointSet[skyband[i]][j] + PointSet[skyband[i]][j + dim]) / 2.0;
                    }

                    // count dominators
                    Noofdominator = countDominator(*rtree, PointSet, pt, ignorerecords);
                    if (Noofdominator > k) {
                        cout << Noofdominator << "; " << k << endl;
                        continue;
                    } else {
                        // update k, first,
                        updatek = k - Noofdominator;
                        initHS(dim, regions);
                        cellTree *sol = new cellTree(2 * (dim - 1));

                        do {
                            updateSkylines(dim, *rtree, skylines, PointSet, ignorerecords);
                            sol->maintainDAG(skylines, removeSL, PointSet, dim);
                            computeHP(dim, PointSet, pt, skylines, newAddSL);

                            cout << "(1) Skylines(" << insertCount << "): " << newAddSL.size() << endl;
                            if (newAddSL.size() > 0) {
                                sol->insert(newAddSL, updatek, *rtree, pt, finalResult);
                                sol->collectLeaf(leaves, updatek);
                            } else {
                                sol->collectLeaf(leaves, updatek);
                            }

                            if (treeSpacecost < sol->treeSize) {
                                treeSpacecost = sol->treeSize;
                            }
                            // process leafs
                            sol->markSingular(leaves, removeSL, updatek, finalResult, singular);
                            cout << "(3) Singular, #Ret: " << finalResult.size() << endl;
                            removeSkylines(skylines, removeSL, ignorerecords);
                            if (removeSL.size() == 0) {
                                break;
                            }
                            insertCount++;
                        } while (leaves.size() > 0);


                        if (finalResult.size() > 0) {
                            utkRet[skyband[i]] = finalResult[0];
                        }
                    }
                }
                cout << "============================================" << endl;
                cout << "# UTK records / # of k-skyband: " << utkRet.size() << "/" << skyband.size() << endl;
                cout << "% of total k-skyband: " << utkRet.size() * 100.0 / skyband.size() << "%" << endl;
                ad = clock();
                cout << "Total time cost: " << fixed << (ad - at) * 1.0 / CLOCKS_PER_SEC << " SEC " << endl;
                cout << "Total space cost: " << fixed << totalSpaceCost + treeSpacecost << " MB " << endl;
                Spacecost += treeSpacecost;
                cout << "============================================" << endl;
            }
            else if (strcmp(methodName, "ON") == 0) {
                at = clock();
                vector<long int> skyband;
                kskyband(dim, *rtree, skyband, PointSet, k);
                vector<long int> klayers;
                onionlayer(skyband, PointSet, k, klayers, dim);
                totalSpaceCost += klayers.size() * 4.00 / MB;
                cout << "# k-onion layer records / # of k-skyband: " << klayers.size() << "/" << skyband.size() << endl;
                for (int i = 0; i < klayers.size(); i++) {
                    cout << "============================================" << endl;
                    cout << "No. " << i + 1 << ", Record ID: " << klayers[i] << endl;

                    // init containers
                    initContainer(insertCount, finalResult, newAddSL, heap, skylines, removeSL, singular,
                                  RecordIDtoHalfPlaneID, HalfSpaces, ignorerecords);

                    // init focal record
                    pt.m_dimen = dim;
                    for (int j = 0; j < dim; j++) {
                        pt.m_coor[j] = (PointSet[klayers[i]][j] + PointSet[klayers[i]][j + dim]) / 2.0;
                    }

                    // count dominators
                    Noofdominator = countDominator(*rtree, PointSet, pt, ignorerecords);
                    if (Noofdominator > k) {
                        cout << Noofdominator << "; " << k << endl;
                        continue;
                    } else {
                        // update k, first,
                        updatek = k - Noofdominator;
                        initHS(dim, regions);

                        cellTree *sol = new cellTree(2 * (dim - 1));

                        do {
                            updateSkylines(dim, *rtree, skylines, PointSet, ignorerecords);
                            sol->maintainDAG(skylines, removeSL, PointSet, dim);
                            computeHP(dim, PointSet, pt, skylines, newAddSL);

                            cout << "(1) Skylines(" << insertCount << "): " << newAddSL.size() << endl;
                            if (newAddSL.size() > 0) {
                                sol->insert(newAddSL, updatek, *rtree, pt, finalResult);
                                sol->collectLeaf(leaves, updatek);
                            } else {
                                sol->collectLeaf(leaves, updatek);
                            }

                            if (treeSpacecost < sol->treeSize) {
                                treeSpacecost = sol->treeSize;
                            }

                            // process leafs
                            sol->markSingular(leaves, removeSL, updatek, finalResult, singular);
                            cout << "(3) Singular, #Ret: " << finalResult.size() << endl;
                            removeSkylines(skylines, removeSL, ignorerecords);
                            if (removeSL.size() == 0) {
                                break;
                            }
                            insertCount++;
                        } while (leaves.size() > 0);


                        if (finalResult.size() > 0) {
                            utkRet[klayers[i]] = finalResult[0];
                        }
                    }
                }
                cout << "============================================" << endl;
                cout << "# UTK records / # of k-skyband: " << utkRet.size() << "/" << klayers.size() << endl;
                cout << "% of total k-skyband: " << utkRet.size() * 100.0 / klayers.size() << "%" << endl;
                ad = clock();
                cout << "Total time cost: " << fixed << (ad - at) * 1.0 / CLOCKS_PER_SEC << " SEC " << endl;
                cout << "Total space cost: " << fixed << totalSpaceCost + treeSpacecost << " MB " << endl;
                Spacecost += treeSpacecost;
                cout << "============================================" << endl;
            }
            else if (strcmp(methodName, "RSA") == 0) {
                at = clock();
                utkRet.clear();
                UTK *sol = new UTK();
                vector<long int> rskyband;

                sol->rskyband(regions, dim, *rtree, rskyband, PointSet, k); // filter
                if (rskyband.size() == k) {
                    for (auto iter = rskyband.begin(); iter < rskyband.end(); iter++) {
                        utkRet[*iter] = new cell();
                    }
                } else {
                    sol->rsa(regions, utkRet, k, dim, PointSet, *rtree); // refinement
                    treeSpacecost = sol->treeSize;
                }

                cout << "============================================" << endl;
                cout << "# UTK records / # of k-skyband: " << utkRet.size() << "/" << rskyband.size() << endl;
                resultSize += utkRet.size();
                cout << "% of total k-skyband: " << utkRet.size() * 100.0 / rskyband.size() << "%" << endl;
                ad = clock();
                cout << "Total time cost: " << fixed << (ad - at) * 1.0 / CLOCKS_PER_SEC << " SEC " << endl;
                cout << "Total space cost: " << fixed << totalSpaceCost + treeSpacecost << " MB " << endl;
                Spacecost += treeSpacecost;
                cout << "============================================" << endl;
            }
            else if (strcmp(methodName, "JAA") == 0) {
//                if(qi==0)
//                    continue;
                at = clock();
                utkRet.clear();
                UTK *sol = new UTK();
                vector<long int> rskyband;
                vector<cell *> exactutk;


                sol->rskyband(regions, dim, *rtree, rskyband, PointSet, k); // filter
                cout << "r-skyband size: " << rskyband.size() << endl;
                if (rskyband.size() == k) {
                    cellTree *candidate = new cellTree(2 * (dim - 1));
                    exactutk.push_back(candidate->root);
                } else {
                    sol->jaa(regions, exactutk, k, dim, PointSet, *rtree); // refinement
                    treeSpacecost = sol->treeSize;
                }

                cout << "============================================" << endl;
                cout << "Regions: " <<  exactutk.size()<< endl;
                resultSize += exactutk.size();
                ad = clock();
                cout << "Total time cost: " << fixed << (ad - at) * 1.0 / CLOCKS_PER_SEC << " SEC " << endl;
                cout << "Total space cost: " << fixed << totalSpaceCost + treeSpacecost << " MB " << endl;
                Spacecost += treeSpacecost;
                cout << "============================================" << endl;
            } else {
                cout << "we only support: BB, ON, RSA, JAA" << endl;
            }
//        } catch (exception &_) {

//        }
    }

    alle = clock();
    cout << "Average time cost of each query: " << fixed << (alle - alls)*1.0 / CLOCKS_PER_SEC / Queryregions.size() << endl;
    cout << "Average space cost of each query: " << fixed << (Spacecost + totalSpaceCost)*1.0 / Queryregions.size() << endl;
    cout << "Average result size: " << fixed << resultSize*1.0 / Queryregions.size() << endl;

    return 0;
}



void initContainer(int& insertCount, vector<cell*>& finalResult, vector<long>& newAddSL, multimap<float, int>& heap, unordered_set<long int>& skylines, unordered_set<long int>& removeSL, unordered_set<long int>& singular,
                   unordered_map<long int, long int>& RecordIDtoHalfPlaneID, vector<vector<float>>& HalfSpaces, unordered_set<long int>& ignorerecords)
{
    insertCount = 0;
    for (int i = 0; i < finalResult.size(); i++)
    {
        finalResult[i]->release();
        delete finalResult[i];
    }
    finalResult.clear();
    vector<cell*>().swap(finalResult);
    newAddSL.clear();
    vector<long>().swap(newAddSL);
    heap.clear();
    multimap<float, int>().swap(heap);
    skylines.clear();
    unordered_set<long int>().swap(skylines);
    removeSL.clear();
    unordered_set<long int>().swap(removeSL);
    singular.clear();
    unordered_set<long int>().swap(singular);
    RecordIDtoHalfPlaneID.clear();
    unordered_map<long int, long int>().swap(RecordIDtoHalfPlaneID);
    HalfSpaces.clear();
    vector<vector<float>>().swap(HalfSpaces);
    ignorerecords.clear();
    unordered_set<long int>().swap(ignorerecords);
}