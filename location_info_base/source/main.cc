#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <rapidjson/error/en.h>

#include <global.hh>
#include <neighbor_search.hh>
#include <lsh/lsh.hh>
#include <lsh/lsh_index.hh>
#include <kdtree/kdtree.hh>

using namespace neighbor_search;

int main(int argc, char const *argv[]) {
    cin.tie(0);
    ios::sync_with_stdio(false);

    int d = 2;
    int k = 3;
    int L = 64;

    string read;

    NeighborSearch *ns;

    // TODO 引数で近傍探索のアルゴリズムを切り替える
    ns = new LSH(d, k, L);
    ns = new KdTree();

    while (1) {

        ///////////////////////////////////////
        // Read stream
        ///////////////////////////////////////
        getline(cin, read);
        // printf("%s\n", read.c_str());

        Json json;
        json.Parse(read.c_str());

        ///////////////////////////////////////
        // Check parse error
        ///////////////////////////////////////
        bool error = json.HasParseError();
        if (error) {
            size_t offset       = json.GetErrorOffset();
            ParseErrorCode code = json.GetParseError();
            const char *msg     = GetParseError_En(code);

            printf("%lu:%d(%s)\n", offset, code, msg);
            exit(1);
        }

        const Value &root = json;
        for (Value::ConstMemberIterator iter = root.MemberBegin();
             iter != root.MemberEnd();
             ++iter) {
            string key         = iter->name.GetString();
            const Value &value = iter->value;

            if (key == "init") {
                printf("%s\n", read.c_str());
                std::flush(std::cout);
                // std::cout << key << std::endl;
                ns->Init(value);
                // TODO init send json

                int id = 0;
                for (auto &&n : value["node"].GetArray()) {

                    Value node(n, json.GetAllocator());
                    node.AddMember("id", id, json.GetAllocator());

                    vector<int> neighbor;
                    neighbor = ns->GetNeighbor(node);

                    ns->SendDeltaHQ(neighbor, node, key);
                    id++;
                }
            }
            if (key == "update") {
                // std::cout << key << std::endl;
                ns->Update(value);

                vector<int> neighbor;

                neighbor = ns->GetNeighbor(value);
                ns->SendDeltaHQ(neighbor, value, key);
                // TODO send json
            }
            if (key == "finish") {
                // std::cout << key << std::endl;
                printf("{\"finish\":\"finish\"}\n");
                std::flush(std::cout);

                goto FINISH_HANDLER;
            }
        }
    }

FINISH_HANDLER:

    return 0;
}
