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

int main() {
    int result = 0;

    graph g;
    broadcast_node<int> input(g);
    Runner runner1;
    runner1.id = 100;
    function_node<int, std::vector<int> > r1(g, unlimited, runner1);
    Runner runner2;
    runner2.id = 200;
    function_node<int, std::vector<int> > r2(g, unlimited, runner2);

    join_node<tbb::flow::tuple<std::vector<int>, std::vector<int>>> j(g);

    function_node<tbb::flow::tuple<std::vector<int>, std::vector<int>>, int>
            finish(g, unlimited,
                   [](const tbb::flow::tuple<std::vector<int>, std::vector<int>> &t2) {
                       LOG(INFO) << std::get<0>(t2)[0] << "     " << std::get<1>(t2)[0];
                       return 1;
                   });

    make_edge(input, r1);
    make_edge(input, r2);
    make_edge(r1, get<0>(j.input_ports()));
    make_edge(r2, get<1>(j.input_ports()));
    make_edge(j, finish);

//    for (int i = 1; i <= 10; ++i)
    input.try_put(2);
    g.wait_for_all();

    printf("Final result is %d\n", result);
    return 0;
}