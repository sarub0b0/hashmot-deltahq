#include <set>

#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <global.hh>
#include <lsh.hh>

namespace neighbor_search {

LSH::LSH(float w, int d, int k, int L) : w_(w), d_(d), k_(k), L_(L) {
}

LSH::~LSH() {
}
void LSH::Init(Value *object, Json *json) {
    fprintf(stderr, "LSH Init\n");

    vector<vector<float>> points;

    Value *array;
    array = Pointer("/init/node").Get(*json);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    json->Accept(writer);

    // printf("%s\n", buffer.GetString());

    for (auto &&n : array->GetArray()) {
        // printf("radius=%d\n", n["radius"].GetInt());
        assert(n["radius"].IsNumber());
        radius_.insert(n["radius"].GetInt());

        std::vector<float> p{static_cast<float>(n["x"].GetDouble()),
                             static_cast<float>(n["y"].GetDouble())};
        points.push_back(p);
    }

    for (auto &&r : radius_) {
        lsh_.push_back(new LSHIndex(w_, d_, k_, L_, r));
        lsh_[lsh_.size() - 1]->Index(points);
    }
}
void LSH::Update(Value *object, Json *json) {
    // TODO 更新処理の作成
}
void LSH::GetNeighbor(Json *json) {
}
void LSH::SendDeltaHQ(vector<int> neighbor, Json *json, string key) {
}

} // namespace neighbor_search
