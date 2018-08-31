#include <set>

#include <lsh_index.hh>

namespace neighbor_search {

LSHIndex::LSHIndex(float w, int d, int k, int L, int radius)
    : w_(w), d_(d), k_(k), L_(0), radius_(radius) {
    hash_family_ = new L2HashFamily(w_, d_);
    Resize(L);
}
LSHIndex::~LSHIndex() {
}

void LSHIndex::Resize(int L) {
    if (L < L_) {
        hash_table_.resize(L);
    } else {
        vector<vector<L2Hash>> hash_func;
        for (int i = L_; i < L; i++) {
            hash_func.push_back(vector<L2Hash>());
            for (int j = 0; j < k_; j++) {
                int l = hash_func.size();
                hash_func[l - 1].push_back(hash_family_->CreateHashFunc());
            }
        }

        for (auto &&g : hash_func) {
            Table t;
            t.g = g;
            hash_table_.push_back(t);
        }
    }
}

string LSHIndex::Hash(vector<L2Hash> g, vector<float> v) {
    vector<int> hashes;
    for (auto &&h : g) {
        point p(v[0], v[1]);
        hashes.push_back(h.Hash(p));
    }

    // printf("hash=%s\n", hash_family_->Combine(hashes).c_str());
    return hash_family_->Combine(hashes);
}

void LSHIndex::Index(vector<vector<float>> points) {
    points_ = points;
    for (auto &&table : hash_table_) {
        for (unsigned int i = 0; i < points.size(); i++) {
            table.table[Hash(table.g, points[i])].push_back(i);
        }
    }
}

void LSHIndex::Update() {
}

vector<int> LSHIndex::Query(vector<float> q) {
    set<int> temp_candidate;

    vector<int> match;
    for (auto &&table : hash_table_) {
        match = table.table[Hash(table.g, q)];
        temp_candidate.insert(match.begin(), match.end());
    }

    puts("Candidate:");
    for (auto &&i : temp_candidate) {
        printf("%d ", i);
    }
    puts("");

    puts("Hits:");
    vector<int> candidate;
    for (auto &&i : temp_candidate) {
        point a, b;
        a = point(q[0], q[1]);
        b = point(points_[i][0], points_[i][1]);
        if (L2Hash::Norm(a, b) <= radius_ && L2Hash::Norm(a, b) != 0.0) {
            printf("%d ", i);
            candidate.push_back(i);
        }
    }

    return candidate;
}

} // namespace neighbor_search
