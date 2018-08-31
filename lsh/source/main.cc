#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>

#include <global.hh>
#include <neighbor_search.hh>
#include <lsh.hh>
#include <lsh_index.hh>

using namespace neighbor_search;

int main(int argc, char const *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    int d      = 2;
    int k      = 3;
    int L      = 64;
    int radius = 3;

    float w = radius * 1.1;

    string read;

    NeighborSearch *ns;

    ns = new LSH(w, d, k, L);

    while (1) {
        getline(cin, read);
        printf("%s\n", read.c_str());

        Json json;
        json.Parse(read.c_str());

        bool error = json.HasParseError();
        if (error) {
            size_t offset       = json.GetErrorOffset();
            ParseErrorCode code = json.GetParseError();
            const char *msg     = GetParseError_En(code);

            printf("%lu:%d(%s)\n", offset, code, msg);
            exit(1);
        }

        Value *object;

        object = Pointer("/update").Get(json);

        if (object && object->IsObject()) {
            printf("update done\n");
        }

        object = Pointer("/finish").Get(json);
        if (object && object->IsObject()) {
            printf("finish\n");
            continue;
        }

        object = Pointer("/init").Get(json);
        if (object && object->IsObject()) {
            ns->Init(object, &json);
            printf("init done\n");
            continue;
        }
    }

    // while (1) {

    // LSHIndex lsh(w, d, k, L, radius);

    // std::vector<std::vector<float>> points = {
    //     {0, -1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}};

    // lsh.Index(points);

    // std::vector<float> query = {0, 5};

    // std::vector<int> answer;

    // puts("Query:");
    // answer = lsh.Query(query);

    // puts("");

    // puts("Answer:");
    // for (auto &&i : answer) {
    //     printf("(%.2f, %.2f)\n", points[i][0], points[i][1]);
    // }
    // }

    return 0;
}
