#include "tbb/tbb.h"
#include "glog/logging.h"
#include "thread"
#include "atomic"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "map"

struct data {
    int a;
};

void testStdQueue() {
    boost::shared_ptr<std::queue<int>> intQueue = boost::make_shared<std::queue<int>>();
    for (int i = 0; i < 10; i++) {
        std::thread([intQueue, i]() {
            for (int n = 0; n < 10; n++) {
                intQueue->push(i + n * 10);
            }
        }).detach();
    }

    for (int i = 0; i < 10; i++) {
        std::thread([intQueue, i]() {
            for (;;) {
                while (!intQueue->empty()) {
                    LOG(INFO) << intQueue->front();
                    intQueue->pop();
                }
            }
        }).detach();
    }
    sleep(20);
}

void testQueue() {
    boost::shared_ptr<tbb::concurrent_queue<int>> intQueue = boost::make_shared<tbb::concurrent_queue<int>>();
    for (int i = 0; i < 10; i++) {
        std::thread([intQueue, i]() {
            for (int n = 0; n < 10; n++) {
                intQueue->push(i + n * 10);
            }
        }).detach();
    }

    for (int i = 0; i < 10; i++) {
        std::thread([intQueue, i]() {
            for (;;) {
                int i;
                while (intQueue->try_pop(i)) {
                    usleep(50);
                    LOG(INFO) << i;
                }
            }
        }).detach();
    }
    sleep(20);
}

void testStdVector() {
    boost::shared_ptr<std::vector<int>> intvector2 = boost::make_shared<std::vector<int>>();
    boost::shared_ptr<std::vector<int>> intvector1 = boost::make_shared<std::vector<int>>();
    for (int i = 0; i < 1000; i++) {
        intvector1->push_back(i);
    }
    tbb::parallel_for_each(*intvector1, [intvector2](const int &i) {
        intvector2->push_back(i);
        usleep(1);
    });
    LOG(INFO) << intvector2->size();
}

#include "tbb/concurrent_vector.h"
#include "tbb/concurrent_hash_map.h"

void testTBBVector() {
    boost::shared_ptr<std::vector<int>> intvector1 = boost::make_shared<std::vector<int>>();
    boost::shared_ptr<tbb::concurrent_vector<int>> intvector2 = boost::make_shared<tbb::concurrent_vector<int>>();
    for (int i = 0; i < 1000; i++) {
        intvector1->push_back(i);
    }
    tbb::parallel_for_each(*intvector1, [intvector2](const int &i) {
        intvector2->push_back(i);
        usleep(1);
    });
    LOG(INFO) << intvector2->size();
}

void testStdMap() {
    boost::shared_ptr<std::map<int, int>> maps1 = boost::make_shared<std::map<int, int>>();
    boost::shared_ptr<std::map<int, int>> maps2 = boost::make_shared<std::map<int, int>>();
    for (int i = 0; i < 1000; i++) {
        maps1->insert({i, i});
    }
    tbb::parallel_for_each(*maps1, [maps2](const std::pair<int, int> &it) {
        maps2->insert(it);
        usleep(1);
    });
    LOG(INFO) << maps2->size();
//    boost::shared_ptr<tbb::concurrent_hash_map<int, int>> tbbmaps = boost::make_shared<tbb::concurrent_hash_map<int, int>>();
}

void testTBBMap() {
    boost::shared_ptr<std::map<int, int>> maps1 = boost::make_shared<std::map<int, int>>();
    boost::shared_ptr<tbb::concurrent_hash_map<int, int>> maps2 = boost::make_shared<tbb::concurrent_hash_map<int, int>>();
    for (int i = 0; i < 1000; i++) {
        maps1->insert({i, i});
    }
    tbb::parallel_for_each(*maps1, [maps2](const std::pair<int, int> &it) {
        maps2->insert(it);
        usleep(1);
    });
    LOG(INFO) << maps2->size();
}

int main() {
//    testStdQueue();
//    testQueue();
//    for (int i = 0; i < 10; i++)
//        testStdVector();
//    for (int i = 0; i < 10; i++)
//        testTBBVector();
//    for (int i = 0; i < 10; i++)
//        testStdMap();
    for (int i = 0; i < 10; i++)
        testTBBMap();
}



