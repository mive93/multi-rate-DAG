#ifndef DPT_H
#define DPT_H

#include <iostream>
#include "MultiRate/MultiRateTaskset.h"

//Job Level Dependency
struct JLD
{
    int tf_; //task from
    int jf_; //j from
    int tt_; //task to
    int jt_; //job to
    JLD(int tf, int jf, int tt, int jt) : tf_(tf), jf_(jf), tt_(tt), jt_(jt){};
};

//Data Propagation Tree Node
struct DPTNode
{
    int i_;
    int j_;
    std::vector<DPTNode> children;
    DPTNode(int i, int j) : i_(i), j_(j) {}
};

//Data Propagation Tree
struct DPT
{
public:
    DPTNode *root = nullptr;
    DPTNode *cur_node = nullptr;
    

    void createRoot(int i, int j)
    {
        root = new DPTNode(i, j);
        cur_node = root;
    }

    void printTree(DPTNode node)
    {
        std::cout << "node: " << node.i_ << " " << node.j_ << std::endl;
        if (node.children.size() > 0)
        {
            for (size_t c = 0; c < node.children.size(); c++)
                printTree(node.children[c]);
        }
    }

    void insertChild(int i, int j, int k, int l, DPTNode &node)
    {
        if (node.i_ == i && node.j_ == j)
        {
            DPTNode new_node(k, l);
            node.children.push_back(new_node);
            return;
        }

        if (node.children.size() > 0)
        {
            for (size_t c = 0; c < node.children.size(); c++)
                insertChild(i, j, k, l, node.children[c]);
        }
    }

    void findDad(DPTNode node, int k, int l, int &z, int &w)
    {
        for (size_t i = 0; i < node.children.size(); i++)
        {
            if (node.children[i].i_ == k && node.children[i].j_ == l)
            {
                z = node.i_;
                w = node.j_;
            }
            findDad(node.children[i], k, l, z, w);
        }
    }

    void findSibling(DPTNode node, int k, int l, int &z, int &w)
    {
        for (size_t i = 0; i < node.children.size(); i++)
        {
            if (node.children[i].i_ == k && node.children[i].j_ == l && i + 1 < node.children.size())
            {
                // std::cout << "----bro " << node.children[i + 1].i_ << " " << node.children[i + 1].j_ << std::endl;
                z = node.children[i + 1].i_;
                w = node.children[i + 1].j_;
                break;
            }
            findSibling(node.children[i], k, l, z, w);
        }
    }


    void findFirstInvalid(DPTNode node, std::vector<std::vector<float>> Rmax, std::vector<std::vector<float>> Rmin, std::vector<std::shared_ptr<MultiNode>> mtasks, const float deadline, int &k, int &l)
    {
        if (node.children.size() > 0)
        {
            for (size_t i = 0; i < node.children.size(); i++)
                findFirstInvalid(node.children[i], Rmax, Rmin, mtasks, deadline, k, l);
        }
        else
        {
            float latency = (Rmax[node.i_][node.j_] + mtasks[node.i_]->wcet) - Rmin[root->i_][root->j_];
            if (latency > deadline && k == -1 && l == -1)
            {
                k = node.i_;
                l = node.j_;
            }
        }
    }

    void DFSMaxLatency(DPTNode node, std::vector<std::vector<float>> Rmax, std::vector<std::vector<float>> Rmin, std::vector<std::shared_ptr<MultiNode>> mtasks, float &max_latency)
    {

        if (node.children.size() > 0)
        {
            for (size_t i = 0; i < node.children.size(); i++)
                DFSMaxLatency(node.children[i], Rmax, Rmin, mtasks, max_latency);
        }
        else
        {
            float latency = (Rmax[node.i_][node.j_] + mtasks[node.i_]->wcet) - Rmin[root->i_][root->j_];
            if (latency > max_latency)
                max_latency = latency;
            std::cout << "latency: " << latency << std::endl;
        }
    }

    float calcMaxLatency(std::vector<std::vector<float>> Rmax, std::vector<std::vector<float>> Rmin, std::vector<std::shared_ptr<MultiNode>> mtasks)
    {
        float max_latency = 0;
        DFSMaxLatency(*root, Rmax, Rmin, mtasks, max_latency);
        return max_latency;
    }
};

struct allDPT
{
    std::vector<DPT> trees;

    void printTrees()
    {
        for (auto tree : trees)
        {
            tree.printTree(*tree.root);
            std::cout << "################################" << std::endl;
        }
    }

    int findDad(int k, int l, int &z, int &w)
    {
        for (auto tree : trees)
        {
            tree.findDad(*tree.root, k, l, z, w);
            if (z != -1 && w != -1)
                break;
        }
    }

    int findFirstSibiling(int k, int l, int &z, int &w, int i_tree)
    {
        int i_dad = -1, j_dad = -1;
        trees[i_tree].findDad(*trees[i_tree].root, k, l, i_dad, j_dad);
        // std::cout << "dad : " << i_dad << " " << j_dad << std::endl;
        int t_index;
        if (i_dad != -1 && j_dad != -1)
            for (t_index = 0; t_index < trees.size(); t_index++)
            {
                trees[t_index].findSibling(*trees[t_index].root, i_dad, j_dad, z, w);
                // std::cout << "dad sibling : " << z << " " << w << std::endl;
                if (z != -1 && w != -1)
                    break;
            }
        return t_index;
    }
};

#endif /*DPT_H*/