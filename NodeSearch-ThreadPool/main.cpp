#include <iostream>
#include <vector>
#include <string>
#include <locale>         // std::locale, std::tolower
#include <algorithm>
#include <functional>
#include <memory>
#include <thread>
#include <boost/asio.hpp>
#include <atomic>

std::atomic<int> matching_counter{0};
boost::asio::thread_pool pool(12);


/*=====================================================
=======================================================
=======================================================
=======================================================
/* 2.3 sec (72% better)
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

    void count_nodes_containing_string(std::string& needle, int depth = 0) const {
        std::string name_lower = to_lower(name_);
        if (name_lower.find(needle) != std::string::npos) {
            ++matching_counter;
        }

        if (depth < 3) { // Parallelize only top 2 levels
            for (const auto& child : children_) {
                boost::asio::post(pool, [&child, &needle, &depth] {
                    child->count_nodes_containing_string(needle, depth + 1);
                });
            }
        } else {
            for (const auto& child : children_) {
                child->count_nodes_containing_string(needle, depth + 1);
            }
        }
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
    root->count_nodes_containing_string(needle_lower);
    pool.join();
    std::cout << matching_counter <<std::endl;

}
#endif
