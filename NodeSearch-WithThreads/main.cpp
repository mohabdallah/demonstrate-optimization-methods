#include <iostream>
#include <vector>
#include <string>
#include <locale>         // std::locale, std::tolower
#include <algorithm>
#include <functional>
#include <memory>
#include <future>
#include <thread>


bool top_level_entered{false};


/*=====================================================
=======================================================
=======================================================
=======================================================
/* 2.1 sec (75% better)
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
        if(top_level_entered)
        {
            for(const std::shared_ptr<Node>& n : children_)
            {
                // call each node count_nodes_containing_string
                counter +=  n->count_nodes_containing_string(needle);
            }
        }
        else
        {
            top_level_entered = true;
            int threads_created_count{0};
            std::vector<std::future<int>> async_future_arr;

            for(const std::shared_ptr<Node>& n : children_)
            {
                if(threads_created_count < std::thread::hardware_concurrency())
                {
                    ++threads_created_count;

                    async_future_arr.emplace_back(std::async(std::launch::async, [&n, &needle]{return n->count_nodes_containing_string(needle);}));
                }
                else
                {
                    counter +=  n->count_nodes_containing_string(needle);
                }
            }

            for(std::future<int>& f : async_future_arr)
            {
                counter += f.get();
            }
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
