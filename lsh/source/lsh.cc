#include <set>

#include <rapidjson/pointer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <global.hh>
#include <lsh.hh>

namespace neighbor_search {

LSH::LSH(int d, int k, int L) : d_(d), k_(k), L_(L) {
}

LSH::~LSH() {
}
void LSH::Init(const Value &json) {
    fprintf(stderr, "LSH Init\n");

    vector<vector<float>> points;

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);

    json.Accept(writer);

    printf("%s\n", buffer.GetString());

    for (auto &&n : json["node"].GetArray()) {
        assert(n["radius"].IsNumber());
        radius_.insert(n["radius"].GetInt());

        std::vector<float> p{static_cast<float>(n["x"].GetDouble()),
                             static_cast<float>(n["y"].GetDouble())};
        points.push_back(p);
    }

    for (auto &&r : radius_) {
        lsh_.push_back(new LSHIndex(r * 1.1, d_, k_, L_, r));
        lsh_[lsh_.size() - 1]->Index(points);
    }
}
void LSH::Update(const Value &json) {
    int id, r;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();
    r  = json["r"].GetInt();

    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            vector<float> point{x, y};

            lsh->Update(id, point);
        }
    }
}

void LSH::GetNeighbor(const Value &json) {
    int id, r;
    float x, y;
    id = json["id"].GetInt();
    x  = json["x"].GetDouble();
    y  = json["y"].GetDouble();
    r  = json["r"].GetInt();

    vector<int> neighbor;
    for (auto &&lsh : lsh_) {
        if (lsh->IsSameRadius(r)) {
            vector<float> point{x, y};

            neighbor = lsh->Query(point);
        }
    }

    printf("neighbor: ");
    for (auto &&n : neighbor) {
        printf("%d ", n);
    }
    puts("");
}
void LSH::SendDeltaHQ(vector<int> &neighbor, const Value &json, string &key) {
}

} // namespace neighbor_search
