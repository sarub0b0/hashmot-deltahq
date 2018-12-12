#include <set>
#include <unordered_set>
#include <chrono>

#include <using.hh>
#include <lsh/lsh_index.hh>
namespace neighbor_search {

LSHIndex::LSHIndex(float w, int d, int k, int L, int radius)
    : w_(w), d_(d), k_(k), L_(0), radius_(radius) {
    hash_family_ = new L2HashFamily(w_, d_);
    Resize(L);
    L_ = L;
}
LSHIndex::~LSHIndex() {
}

void LSHIndex::Resize(int L) {
    if (L < L_) {
        hash_table_.resize(L);
    } else {
        vector<vector<L2Hash>> hash_func;
        // hash_func.reserve(L - L_);
        int l = 0;
        for (int i = L_; i < L; ++i) {
            hash_func.push_back(vector<L2Hash>());
            hash_func[l].reserve(k_);
            ++l;
            for (int j = 0; j < k_; ++j) {
                hash_func[i].push_back(hash_family_->CreateHashFunc());
            }
        }

        for (auto &&g : hash_func) {
            Table t;
            t.g = g;
            t.g.reserve(k_);
            hash_table_.push_back(t);
        }
    }
}

array<uint32_t, K_FUNCS> LSHIndex::Hash(vector<L2Hash> &g,
                                        array<float, DIMENSION> &v) {
    // vector<int> hashes;
    // hashes.reserve(k_);
    // for (auto &&h : g) {
    //     point p(v[0], v[1]);
    //     hashes.push_back(h.Hash(p));
    // }
    // printf("hash=%s\n", hash_family_->Combine(hashes).c_str());
    // return hash_family_->Combine(hashes);
    array<uint32_t, K_FUNCS> hashes;
    for (int i = 0; i < K_FUNCS; ++i) {
        point p(v[0], v[1]);
        hashes[i] = g[i].Hash(p);
    }
    return hashes;
}

void LSHIndex::Index(vector<array<float, DIMENSION>> &points) {

    points_ = points;
    for (auto &&table : hash_table_) {
        table.table.reserve(points_.size());
        for (unsigned int i = 0; i < points.size(); i++) {
            table.table[Hash(table.g, points[i])].push_back(i);
        }
    }
}

void LSHIndex::Update(int id, array<float, DIMENSION> &point) {

#ifdef TCHK_ELAPSED
    chrono::high_resolution_clock::time_point begin, end;
    chrono::nanoseconds elapsed;

    begin = chrono::high_resolution_clock::now();

#endif

    array<float, DIMENSION> &old_point = points_[id];
    array<float, DIMENSION> &new_point = point;

    for (auto &&table : hash_table_) {
        const array<uint32_t, K_FUNCS> old_hash = Hash(table.g, old_point);
        const array<uint32_t, K_FUNCS> new_hash = Hash(table.g, new_point);

        if (old_hash == new_hash) {
            continue;
        }

        table.table[old_hash].remove(id);
        table.table[new_hash].push_back(id);
    }

    // points_[id] = point;
    old_point = new_point;
    // for (auto &&table : hash_table_) {
    //     table.table[Hash(table.g, point)].push_back(id);

    //     // for (auto itr = table.table.begin(); itr != table.table.end();
    //     //      ++itr) {

    //     //     printf("key=%s: ", itr->first.c_str());
    //     //     for (auto &&l : itr->second) {
    //     //         printf("%d ", l);
    //     //     }
    //     //     puts("");
    //     // }
    // }

#ifdef TCHK_ELAPSED
    end = chrono::high_resolution_clock::now();

    elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);

    printf("update lsh: %lld.%09lld\n",
           elapsed.count() / 1000000000,
           elapsed.count() % 1000000000);
#endif
}

vector<int> LSHIndex::Query(array<float, DIMENSION> &q) {
    unordered_set<int> temp_candidate;
    // set<int> temp_candidate;
    // list<int> temp_candidate;

    for (auto &&table : hash_table_) {
        list<int> &match = table.table[Hash(table.g, q)];
        temp_candidate.insert(match.begin(), match.end());
        // temp_candidate.insert(
        // temp_candidate.end(), match.begin(), match.end());
    }
    // temp_candidate.sort();
    // temp_candidate.unique();

    // puts("Candidate:");
    // for (auto &&i : temp_candidate) {
    //     printf("%d ", i);
    // }
    // puts("");

    // puts("Hits:");

    // TODO ソートしてからradiusで枝刈りしたほうが早いかもしれない
    //
    vector<int> candidate;
    candidate.reserve(temp_candidate.size());
    for (auto &&i : temp_candidate) {
        float d;
        // point a, b;
        // a = point(q[0], q[1]);
        // b = point(points_[i][0], points_[i][1]);
        // d = L2Hash::Norm(a, b);

        array<float, 2> &a = q;
        array<float, 2> &b = points_[i];

        d = sqrt((a[0] - b[0]) * (a[0] - b[0]) +
                 (a[1] - b[1]) * (a[1] - b[1]));
        if (d <= radius_ && d != 0.0) {
            // printf("%d ", i);
            candidate.push_back(i);
        }
    }

    // candidate.shrink_to_fit();
    // puts("");
    return candidate;
}

bool LSHIndex::IsSameRadius(int radius) {
    if (radius_ == radius) {
        return true;
    } else {
        return false;
    }
}

} // namespace neighbor_search
