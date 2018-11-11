#include <thread>

class C {
   public:
    // blah blah blah
    void run() {
        std::thread thd(&C::impl, this, 10);
        thd.join();
    }
    void impl(int n) {
        printf("%d\n", n);
        // do something
    }
};

int main() {
    C c;
    c.run();
    return 0;
}
