// MIT License

// Copyright (c) 2019 kosay

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SOCKET_H
#define SOCKET_H

#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <err.h>
#include <sys/errno.h>
#include <string.h>

#include <string>

#define DEBUG

#ifdef DEBUG
#define DGRAM_PERROR(func)                                          \
    do {                                                            \
        char s[BUFSIZ];                                             \
        memset(s, 0, BUFSIZ);                                       \
        snprintf(s, BUFSIZ, "%s(%d):%s", __FILE__, __LINE__, func); \
        perror(s);                                                  \
    } while (false)

// do {                                                                \
    //     if (debug) {                                                    \
    //         char s[BUFSIZ];                                             \
    //         memset(s, 0, BUFSIZ);                                       \
    //         snprintf(s, BUFSIZ, "%s(%d):%s", __FILE__, __LINE__, func); \
    //         perror(s);                                                  \
    //     }                                                               \
    // } while (false)
#else
#define DGRAM_PERROR(func) \
    do {                   \
    } while (false)
#endif

namespace neighbor_search {

class DGram {
   public:
    DGram() {
        fd_     = -1;
        family_ = 0;
        memset(&to_addr_, 0, sizeof(to_addr_));
        memset(&from_addr_, 0, sizeof(from_addr_));
    };
    ~DGram() {
        if (fd_ != -1) {
            close(fd_);
        }
    };

    int Open(const std::string &type, bool is_broadcast);

    bool Bind(const std::string &host, const std::string &port);
    bool SetTo(const std::string &dest_host, const std::string &dest_port);

    ssize_t SendTo(const void *buf, size_t length, int flag);
    ssize_t RecvFrom(void *buf, size_t length);

   private:
    int fd_;
    int family_;

    // char buf_[BUFSIZ];

    struct sockaddr_storage to_addr_;
    struct sockaddr_storage from_addr_;

    socklen_t recv_addr_length_;
};

} // namespace neighbor_search
#endif
