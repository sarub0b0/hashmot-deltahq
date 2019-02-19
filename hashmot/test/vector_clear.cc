#include <iostream>
#include <vector>
using namespace std;
int main(int argc, char const *argv[]) {
    vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    cout << "size=" << v.size() << " capacity=" << v.capacity() << endl;

    v.clear();
    cout << "size=" << v.size() << " capacity=" << v.capacity() << endl;
    // clear しても capacityは変化しません。

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    cout << "size=" << v.size() << " capacity=" << v.capacity() << endl;

    v.clear();
    cout << "size=" << v.size() << " capacity=" << v.capacity() << endl;
    v.shrink_to_fit(); // 新しいコンパイラが必要です。
    cout << "size=" << v.size() << " capacity=" << v.capacity() << endl;
    // ここで、capacityが変化します。
    return 0;
}
