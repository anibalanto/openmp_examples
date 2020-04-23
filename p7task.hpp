#pragma once
#include "register_thread.hpp"
#include "function_loaded.hpp"

#include <vector>

struct Node {
    int value;
    Node *left = nullptr;
    Node *right = nullptr;
};

void fill(Node *&p, int value)
{
    if(p == nullptr)
    {
        p = new Node;
        p->value = value;
    }
    else if(p->value > value)
    {
        fill(p->left, value);
    }else
    {
        fill(p->right, value);
    }
}

void fill (Node *&p, const std::vector<int> &elems)
{
    for(auto elem : elems)
    {
        fill(p, elem);
    }
}

void free(Node *&p)
{
    if(p != nullptr)
    {
        free(p->left);
        free(p->right);
        delete(p);
    }
}

void ptask()
{
    Node *root = nullptr;

    fill(root, {50, 30, 70, 10, 20, 60, 80, 40, 1, 90,
                53, 33, 73, 13, 23, 63, 83, 93, 43, 3,
                31, 37, 41, 47, 51, 57, 61, 67, 71, 77 });

    {
        RegisterThread regist("serial");

        // creo una variable de tipo función que
        // el retorno es void y recibe como parametro
        // un puntero a Node
        std::function<void (Node *)> traverse;

        // la instancio con una función lambda recursiva
        traverse = [&regist, &traverse](Node *p) {
            if (p->left)
                traverse(p->left);
            regist.registrate(std::to_string(p->value));
            if (p->right)
                traverse(p->right);
        };

        regist.registrate("begin");
        traverse(root);
        regist.registrate("end");
    }

    {
        RegisterThread regist("task");

        std::function<void (Node *)> traverse;

        #pragma omp parallel
        {
            #pragma omp single
            {
                traverse = [&regist, &traverse](Node *p)
                {
                    if (p->left)
                    #pragma omp task   // p is firstprivate by default
                        traverse(p->left);
                    regist.registrate(std::to_string(p->value));
                    if (p->right)
                    #pragma omp task    // p is firstprivate by default
                        traverse(p->right);
                };

                regist.registrate("begin");
                traverse(root);
                regist.registrate("end");
            }
        }
    }

    free(root);
}

FunctionLoaded load_ptask()
{
    return {"ptask", ptask};
}