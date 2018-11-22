#include <thread>
#include <chrono>
#include <iostream>

class Buffer {
   private:
    void ReadString(int n);

   public:
    void Start(int n);
};

void Buffer::Start(int n) {
    std::thread th0(&Buffer::ReadString, this, 0);
    std::thread th1(&Buffer::ReadString, this, 1);
    // std::thread th([this]() { this->ReadString(); });
    th0.join();
    th1.join();
}

void Buffer::ReadString(int n) {
    for (int i = 0; i < 100; i++) {
        if (n == 0) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));
            printf("%d\n", i);
        } else {
            std::this_thread::sleep_for(std::chrono::nanoseconds(2));
            printf("\t%d\n", i);
        }
    }
}

int main(void) {

    Buffer buf;
    buf.Start(0);

    return 0;
}
