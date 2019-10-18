#ifndef DATAFILES_H
#define DATAFILES_H

#include <fstream>
#include <cstring>
#include <vector>
#include <iostream>

class DataFiles
{
private:
    std::ofstream r;
    std::ofstream da;

public:
    int val_index = 0;

    std::ofstream sd;
    std::ofstream t;
    std::ofstream d;
    std::ofstream p;
    std::vector<std::vector<float>> r_values;
    std::vector<std::vector<float>> da_values;

    DataFiles();
    DataFiles(const std::string &reactions, const std::string &data_age, const std::string &schedulable_dags, const std::string &times, const std::string &deletions, const std::string &permutations);
    ~DataFiles();
    void newLine();

    void addR(const float r_val);
    void addTest();
    void addDA(const float da_val);
    void writeRDA(const int n_chains);
    void print();
};

#endif /*DATAFILES_H*/