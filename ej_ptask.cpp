#include "register_thread.hpp"

struct node {
    int value;
    struct node *left = nullptr;
    struct node *right = nullptr;
};

void fill(struct node *&p, int value)
{
    if(p == nullptr)
    {
        p = new struct node;
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

void fill (struct node *&p, const std::vector<int> &elems)
{
    for(auto elem : elems)
    {
        fill(p, elem);
    }
}

class TreeTraverser
{
    struct node *_root;
    RegisterThread _regist;
public:
    TreeTraverser(const std::string &name, struct node *root) :
        _regist{name},
        _root(root)
    { }

    void treverse()
    {
        _regist.registrate("begining my travell");
        #pragma omp parallel
        {
            #pragma omp single
            {
                traverse(_root);
            }
        }
        _regist.registrate("ending my travell");
    }

protected:

    virtual void traverse(struct node *p) = 0;

    void process(struct node *p)
    {
        _regist.registrate(std::to_string(p->value));
    }

};

class TreeTraverserSerial: public TreeTraverser
{
public:
    TreeTraverserSerial(struct node *root):
        TreeTraverser("serial", root)
    {
    }

protected:
    void traverse(struct node *p) override
    {
        if (p->left)
            traverse(p->left);
        process(p);
        if (p->right)
            traverse(p->right);
    }
};

class TreeTraverserTask: public TreeTraverser
{
public:
    TreeTraverserTask(struct node *root):
        TreeTraverser("task", root)
    {
    }

protected:
    void traverse(struct node *p)
    {
        if (p->left)
        #pragma omp task   // p is firstprivate by default
            traverse(p->left);
        process(p);
        if (p->right)
        #pragma omp task    // p is firstprivate by default
            traverse(p->right);
        
    }
};

#include <vector>
int main()
{
    struct node *root = nullptr;
    fill(root, {                 
                                                            50,
                            30,                                                             70,
            10,                             20,                             60,                             80,
    40,             1,              90,             53,             33,             73,             13,             23, 63, 83, 93, 43, 3,31, 37, 41, 47, 51, 57, 61, 67, 71, 77 });

    {
        TreeTraverserSerial tts(root);
        tts.treverse();
    }
    {
        TreeTraverserTask ttt(root);
        ttt.treverse();
    }
    
    return 0;
}
