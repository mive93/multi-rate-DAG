#include "DataFiles.h"

DataFiles::DataFiles(const std::string &reactions, const std::string &data_age, const std::string &permutations, const std::string &schedulable_dags)
{
    r.open(reactions);
    da.open(data_age);
    p.open(permutations);
    sd.open(schedulable_dags);
    r_values.resize(4); //4 tests allowed
    da_values.resize(4); //4 tests allowed
}

DataFiles::~DataFiles()
{
    r.close();
    da.close();
    p.close();
    sd.close();
}
void DataFiles::newLine()
{
    p << "\n";
    sd << "\n";
}

void DataFiles::addTest()
{
    val_index++;
}

void DataFiles::addR(const float r_val)
{
    r_values[val_index].push_back(r_val);
}
void DataFiles::addDA(const float da_val)
{

    da_values[val_index].push_back(da_val);
}
void DataFiles::writeRDA(const int n_chains)
{

    for (int j = 0; j < n_chains; j++)
    {
        for (int i = 0; i <= val_index; i++)
        {

            if (j < r_values[i].size())
                r << r_values[i][j] << ";";
            else
                r << "0;";

            if (j < da_values[i].size())
                da << da_values[i][j] << ";";
            else
                da << "0;";
        }
        da << "\n";
        r << "\n";
    }

    for (size_t i = 0; i < r_values.size(); i++)
    {
        r_values[i].clear();
        da_values[i].clear();
    }

    val_index = 0;
}

void DataFiles::print()
{
    for (int i = 0; i <= val_index; i++)
        for (auto da : da_values[i])
            std::cout << da << std::endl;
}