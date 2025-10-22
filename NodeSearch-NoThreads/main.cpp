#include <iostream>
#include <vector>
#include <string>
#include <locale>         // std::locale, std::tolower
#include <algorithm>
#include <functional>
#include <memory>



/*=====================================================
=======================================================
=======================================================
=======================================================
/* 8.2 sec without -O3, 2.5 sec with -O3 (70% better)
=======================================================
=======================================================
=======================================================
=====================================================*/
class Node
{
  public:
    Node(std::string name, std::vector<std::shared_ptr<Node>> children = {}) : name_(std::move(name)), children_(std::move(children))
    {

    }

    static std::string to_lower(const std::string& input) {
        std::string result = input;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return result;
    }

    int count_nodes_containing_string(std::string& needle) const
    {
        // create counter with 0
        int counter {0};

        // create string with name_ converted to lower case
        std::string name_lower = to_lower(name_);

        // check if needle exists in current node name
        if(name_lower.find(needle) != std::string::npos)
        {
            // if true, increment counter
            ++counter;
        }

        // loop on all nodes
        for(const std::shared_ptr<Node>& n : children_)
        {
            // call each node count_nodes_containing_string
            counter +=  n->count_nodes_containing_string(needle);
        }

        return counter;
    }
private:
    const std::string name_;
    const std::vector<std::shared_ptr<Node>> children_;
};

#ifndef RunTests
int main()
{
    //Create an example tree
    // Generate a large tree with depth and breadth
    const int depth = 7;         // depth of the tree
    const int breadth = 10;       // number of children per node

    // Recursive tree builder
    std::function<std::shared_ptr<Node>(int)> build_tree = [&](int level) -> std::shared_ptr<Node> {
        if (level == 0)
            return std::make_shared<Node>("TEXT");

        std::vector<std::shared_ptr<Node>> children;
        for (int i = 0; i < breadth; ++i) {
            children.push_back(build_tree(level - 1));
        }
        return std::make_shared<Node>("TEXT", std::move(children));
    };


    std::shared_ptr<Node> root = build_tree(depth);

    //Cout the solution
    std::string str_to_find = "Text";
    std::string needle_lower = Node::to_lower(str_to_find);
    std::cout << root->count_nodes_containing_string(needle_lower)<<std::endl;
}
#endif
