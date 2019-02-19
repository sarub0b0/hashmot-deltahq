#ifndef USING_H
#define USING_H

#include <vector>
#include <string>

#include <rapidjson/document.h>

#define K_FUNCS 3
#define DIMENSION 2
#define HASHES 24

// using int32_t = int;

namespace neighbor_search {

using namespace std;
using namespace rapidjson;
using Json = rapidjson::Document;

} // namespace neighbor_search
#endif
