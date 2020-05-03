#include <cstdio>
#include "tbb/flow_graph.h"

using namespace tbb::flow;

struct Runner {
    int id;

    std::vector<int> operator()(int n) {
        std::vector<int> result;
        for (int i = id; i < id + n; i++) {
            result.push_back(i);
        }
        return result;
    };
};

#include "glog/logging.h"

/*
 *
 *
 *                  input
 *                 /     \
 *                /       \
 *                r1      r2
 *                \        /
 *                 \      /
 *                   join
 *                     |
 *                   finish
 * 函数调用流程图如上,接受到输入,input函数转发给r1 r2函数,两个函数分别生成一个std::vector<int>,汇总到joint函数,最后将参数传递给finish函数.
 */
int main() {
    graph g;
    broadcast_node<int> input(g);
    Runner runner1;
    runner1.id = 100;
    function_node<int, std::vector<int> > r1(g, unlimited, runner1);
    Runner runner2;
    runner2.id = 200;
    function_node<int, std::vector<int> > r2(g, unlimited, runner2);

    join_node<tbb::flow::tuple<std::vector<int>, std::vector<int>>> join(g);

    function_node<tbb::flow::tuple<std::vector<int>, std::vector<int>>, int>
            finish(g, unlimited,
                   [](const tbb::flow::tuple<std::vector<int>, std::vector<int>> &t2) {
                       LOG(INFO) << std::get<0>(t2)[0] << " " << std::get<0>(t2).size() << "     " << std::get<1>(t2)[0]
                                 << " " << std::get<0>(t2).size();
                       return 1;
                   });

    make_edge(input, r1);
    make_edge(input, r2);
    make_edge(r1, get<0>(join.input_ports()));
    make_edge(r2, get<1>(join.input_ports()));
    make_edge(join, finish);

//    for (int i = 1; i <= 10; ++i)
    input.try_put(2);
    g.wait_for_all();

    return 0;
}