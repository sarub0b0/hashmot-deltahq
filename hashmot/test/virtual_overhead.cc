#include <iostream>
#include <chrono>

using namespace std;

class VirtualClass {
   public:
    virtual ~VirtualClass() {
        return;
    };

    virtual void Loop() {
        return;
    };
};

class Foo : public VirtualClass {
   public:
    Foo();
    ~Foo();

    void Loop();
};

class Bar {
   public:
    Bar();
    ~Bar();

    void Loop();
};

Foo::Foo() {
}
Foo::~Foo() {
}
void Foo::Loop() {
    // int a = 0;
    // for (int i = 0; i < 10000000; i++) {
    //     a++;
    // }
}

Bar::Bar() {
}
Bar::~Bar() {
}
void Bar::Loop() {
    // int a = 0;
    // for (int i = 0; i < 10000000; i++) {
    //     a++;
    // }
}

void Loop() {
    // int a = 0;
    // for (int i = 0; i < 10000000; i++) {
    //     a++;
    // }
}
int main(void) {
    VirtualClass *vc = new VirtualClass();
    Foo *foo         = new Foo();
    Bar *bar         = new Bar();

    cout << "main loop\n";

    chrono::high_resolution_clock::time_point start =
        chrono::high_resolution_clock::now();

    vc->Loop();
    chrono::high_resolution_clock::time_point end =
        chrono::high_resolution_clock::now();

    cout << "\telapsed: "
         << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
         << "\n";

    cout << "foo loop\n";

    start = chrono::high_resolution_clock::now();

    foo->Loop();

    end = chrono::high_resolution_clock::now();

    cout << "\telapsed: "
         << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
         << "\n";

    cout << "virtual foo loop\n";
    vc = foo;

    start = chrono::high_resolution_clock::now();

    vc->Loop();

    end = chrono::high_resolution_clock::now();

    cout << "\telapsed: "
         << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
         << "\n";

    cout << "bar loop\n";

    start = chrono::high_resolution_clock::now();

    bar->Loop();

    end = chrono::high_resolution_clock::now();

    cout << "\telapsed: "
         << chrono::duration_cast<chrono::nanoseconds>(end - start).count()
         << "\n";
    return 0;
}

