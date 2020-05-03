#include <cstdio>
#include "tbb/flow_graph.h"
#include "glog/logging.h"
#include "thread"

using namespace tbb::flow;

struct Runner {
    int id;

    std::vector<int> operator()(int n) {
        LOG(INFO) << std::this_thread::get_id();
        std::vector<int> result;
        for (int i = id; i < id + n; i++) {
            result.push_back(i);
            usleep(500);
        }
        return result;
    };
};

/*
 *
 *
 *                  input
 *                 /     \
 *                /       \
 *                r1 ...   r5
 *                \        /
 *                 \      /
 *                   join
 *                     |
 *                   finish
 * 函数调用流程图如上,接受到输入,input函数转发给r1 .. r5函数,每个函数分别生成一个std::vector<int>,汇总到joint函数,最后将参数传递给finish函数.
 * r1到r5完全并行状态
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

    Runner runner3;
    runner3.id = 300;
    function_node<int, std::vector<int> > r3(g, unlimited, runner3);

    Runner runner4;
    runner4.id = 400;
    function_node<int, std::vector<int> > r4(g, unlimited, runner4);

    Runner runner5;
    runner2.id = 500;
    function_node<int, std::vector<int> > r5(g, unlimited, runner5);

    join_node<tbb::flow::tuple<std::vector<int>,
            std::vector<int>,
            std::vector<int>,
            std::vector<int>,
            std::vector<int>>> join(g);

    function_node<tbb::flow::tuple<std::vector<int>,
            std::vector<int>,
            std::vector<int>,
            std::vector<int>,
            std::vector<int>
    >, int>
            finish(g, unlimited,
                   [](const tbb::flow::tuple<std::vector<int>, std::vector<int>,
                           std::vector<int>,
                           std::vector<int>,
                           std::vector<int>> &t2) {
                       LOG(INFO) << std::this_thread::get_id();
                       LOG(INFO) << std::get<0>(t2)[0] << " " << std::get<0>(t2).size() << "size     ";
                       LOG(INFO) << std::get<0>(t2)[1] << " " << std::get<1>(t2).size() << "size     ";
                       LOG(INFO) << std::get<0>(t2)[2] << " " << std::get<2>(t2).size() << "size     ";
                       LOG(INFO) << std::get<0>(t2)[3] << " " << std::get<3>(t2).size() << "size     ";
                       LOG(INFO) << std::get<0>(t2)[4] << " " << std::get<4>(t2).size() << "size     ";
                       return 1;
                   });

    make_edge(input, r1);
    make_edge(input, r2);
    make_edge(input, r3);
    make_edge(input, r4);
    make_edge(input, r5);
    make_edge(r1, get<0>(join.input_ports()));
    make_edge(r2, get<1>(join.input_ports()));
    make_edge(r3, get<2>(join.input_ports()));
    make_edge(r4, get<3>(join.input_ports()));
    make_edge(r5, get<4>(join.input_ports()));
    make_edge(join, finish);

//    for (int i = 1; i <= 10; ++i)
    input.try_put(20000);
    g.wait_for_all();

    return 0;
}
