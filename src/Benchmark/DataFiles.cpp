#include "DataFiles.h"

DataFiles::DataFiles(const std::string &reactions, const std::string &data_age, const std::string &schedulable_dags, const std::string &times, const std::string &deletions, const std::string &permutations)
{
    r.open(reactions);
    da.open(data_age);
    sd.open(schedulable_dags);
    d.open(deletions);
    t.open(times);
    p.open(permutations);
    r_values.resize(4);  //4 tests allowed
    da_values.resize(4); //4 tests allowed
}

DataFiles::~DataFiles()
{
    r.close();
    da.close();
    sd.close();
    d.close();
    p.close();
    t.close();
}
void DataFiles::newLine()
{
    sd << "\n";
    d << "\n";
    t << "\n";
    p << "\n";
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

    bool write = false;
    for (auto c : da_values)
        for (auto da : c)
            if (da > 0)
                write = true;

    if (write)
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