#include "tbb/tbb.h"
#include "glog/logging.h"
#include "thread"
#include "atomic"

//https://software.intel.com/en-us/node/506156
void testParallelfor() {
    {
        LOG(INFO) << "start No TBB";
        std::atomic<long> sum;
        sum = 0;
        for (int i = 1; i < 1000000; i++) {
            sum.fetch_add(i);
        }
//        6461
        LOG(INFO) << "sum is " << sum;
    }
    {
        LOG(INFO) << "TBB start";
        std::atomic<long> ssum;
        ssum = 0;
        tbb::parallel_for(1, 1000000, [&](size_t i) {
            ssum.fetch_add(i);
        });
//    16630
        LOG(INFO) << "sum is " << ssum;
    }
}

void testParalleldo() {
    std::vector<int> ints;
    for (int i = 0; i < 100000; i++) {
        ints.push_back(i);
    }
    tbb::parallel_do(ints.begin(), ints.end(), [](int &i) {
        LOG(INFO) << i;
    });
    LOG(INFO) << "parallel_do finish ";
}

void testParallelforeach() {
    std::vector<int> ints;
    for (int i = 0; i < 100000; i++) {
        ints.push_back(i);
    }
    tbb::parallel_for_each(ints, [](int &i) {
        LOG(INFO) << i;
    });
    LOG(INFO) << "parallel_for_each finish ";
}

#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

void testparallel_sort() {
    {
        LOG(INFO) << "sort start";
        std::vector<int> ints;
        for (int i = 0; i < 99999999; i++) {
            ints.push_back(i);
        }
        std::sort(ints.begin(), ints.end(), [](int &i, int &i1) {
            return i > i1;
        });
        LOG(INFO) << "sort end";
    }
    {
        LOG(INFO) << "parallel_sort start";
        boost::shared_ptr<std::vector<int>> ints = boost::make_shared<std::vector<int>>();
        for (int i = 99999999; i > 0; i--) {
            ints->push_back(i);
        }
        tbb::parallel_sort(ints->begin(), ints->end(), [](int &i, int &i1) {
            return i < i1;
        });
        LOG(INFO) << "parallel_sort finish ";
    }
}

void testParalletinvoke() {
    tbb::parallel_invoke([]() {
        LOG(INFO) << 1;
    }, []() {
        LOG(INFO) << 2;
    }, []() {
        LOG(INFO) << 2;
    }, []() {
        LOG(INFO) << 3;
    }, []() {
        LOG(INFO) << 4;
    }, []() {
        LOG(INFO) << 5;
    }, []() {
        LOG(INFO) << 6;
    }, []() {
        LOG(INFO) << 7;
    });
    LOG(INFO) << "parallel_invoke finish ";
}

int main() {
//    testParallelfor();
//    testParalleldo();
//    testParallelforeach();
//    testParalletinvoke();
    testparallel_sort();
}



