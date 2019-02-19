#include <list>
#include <vector>

int main(void) {

    std::list<int> l;

    std::vector<std::list<int>::pointer> l_ptr;

    l.push_back(1);
    l.push_back(2);
    l.push_back(3);
    l.push_back(4);
    l.push_back(5);
    l.push_back(6);

    for (auto &&i : l) {
        printf("i=%d\n", i);
        l_ptr.push_back(&i);
    }

    puts("");
    std::list<int>::iterator itr = l.begin();
    ++itr;
    ++itr;
    ++itr;
    ++itr;
    ++itr;
    ++itr;
    l.push_front(7);
    l_ptr.push_back(&(*itr));
    for (auto &&i : l) {
        printf("i=%d\n", i);
    }
    puts("");
    for (auto &&i : l_ptr) {
        printf("i=%d\n", *i);
    }
    puts("");

    l.erase(l.begin());
    for (auto &&i : l_ptr) {
        printf("i=%d\n", *i);
    }
}
