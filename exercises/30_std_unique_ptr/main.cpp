#include "../exercise.h"
#include <cstring>
#include <memory>
#include <string>
#include <vector>

// READ: `std::unique_ptr` <https://zh.cppreference.com/w/cpp/memory/unique_ptr>

std::vector<std::string> RECORDS;

class Resource {
    std::string _records;

public:
    void record(char record) {
        _records.push_back(record);
    }

    ~Resource() {
        RECORDS.push_back(_records);
    }
};

using Unique = std::unique_ptr<Resource>;
Unique reset(Unique ptr) {
    if (ptr) ptr->record('r');
    return std::make_unique<Resource>();
}
Unique drop(Unique ptr) {
    if (ptr) ptr->record('d');
    return nullptr;
}
Unique forward(Unique ptr) {
    if (ptr) ptr->record('f');
    return ptr;
}

int main(int argc, char **argv) {
    std::vector<std::string> problems[3];

    drop(forward(reset(nullptr)));
    problems[0] = std::move(RECORDS);

    forward(drop(reset(forward(forward(reset(nullptr))))));
    problems[1] = std::move(RECORDS);

    drop(drop(reset(drop(reset(reset(nullptr))))));
    problems[2] = std::move(RECORDS);

    // ---- 不要修改以上代码 ----

    std::vector<const char *> answers[]{
        {"fd"},
        // TODO: 分析 problems[1] 中资源的生命周期，将记录填入 `std::vector`
        // NOTICE: 此题结果依赖对象析构逻辑，平台相关，提交时以 CI 实际运行平台为准
        // problem 1: forward(drop(reset(forward(forward(reset(nullptr))))))
        // 1. reset(nullptr) -> 返回 new Res1, records="", 没记录
        // 2. forward(Res1) -> Res1.record('f') -> "", "f" -> 返回 Res1
        // 3. forward(Res1) -> Res1.record('f') -> "f", "ff" -> 返回 Res1
        // 4. reset(Res1) -> Res1.record('r') -> "ffr", 返回 new Res2. Res1 析构 -> RECORDS=["ffr"]
        // 5. drop(Res2) -> Res2.record('d') -> "d", 返回 nullptr. Res2 析构 -> RECORDS=["ffr", "d"]
        // 6. forward(nullptr) -> 不做任何事, 返回 nullptr.
        {"ffr", "d"},
        // problem 2: drop(drop(reset(drop(reset(reset(nullptr))))))
        // 1. reset(nullptr) -> 返回 Res1.
        // 2. reset(Res1) -> Res1.record('r') -> "r", 返回 Res2. Res1 析构 -> RECS=["r"]
        // 3. drop(Res2) -> Res2.record('d') -> "d", 返回 nullptr. Res2 析构 -> RECS=["r", "d"]
        // 4. reset(nullptr) -> 返回 Res3.
        // 5. drop(Res3) -> Res3.record('d') -> "d", 返回 nullptr. Res3 析构 -> RECS=["r", "d", "d"]
        // 6. drop(nullptr) -> 返回 nullptr.
        {"r", "d", "d"},
    };

    // ---- 不要修改以下代码 ----

    for (auto i = 0; i < 3; ++i) {
        ASSERT(problems[i].size() == answers[i].size(), "wrong size");
        for (auto j = 0; j < problems[i].size(); ++j) {
            ASSERT(std::strcmp(problems[i][j].c_str(), answers[i][j]) == 0, "wrong location");
        }
    }

    return 0;
}
