#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <dgram.hh>

namespace neighbor_search {

ssize_t DGram::SendTo(const void *buf, size_t length, int flag) {
    ssize_t ret;
    if (family_ == AF_INET) {
        ret = sendto(fd_,
                     buf,
                     length,
                     flag,
                     reinterpret_cast<struct sockaddr *>(&to_addr_),
                     sizeof(struct sockaddr_in));
        if (ret == -1) {
            DGRAM_PERROR("sendto");
        }
    } else if (family_ == AF_INET6) {
        ret = sendto(fd_,
                     buf,
                     length,
                     flag,
                     reinterpret_cast<struct sockaddr *>(&to_addr_),
                     sizeof(struct sockaddr_in6));
        if (ret == -1) {
            DGRAM_PERROR("sendto");
        }
    } else {
        return -1;
    }
    return ret;
}

bool DGram::Bind(const std::string &host, const std::string &port) {
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (family_ == 0) {
        return false;
    }

    if (fd_ == -1) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family   = family_;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;

    error = getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        // if (debug) {
        fprintf(stderr,
                "%s(%d):getaddrinfo: %s\n",
                __FILE__,
                __LINE__,
                gai_strerror(error));
        // }
#endif
        return false;
    }

    if (bind(fd_, res->ai_addr, res->ai_addrlen) < 0) {
        DGRAM_PERROR("bind");
        close(fd_);
        fd_     = -1;
        family_ = 0;
        return false;
    }

    freeaddrinfo(res);
    return true;
}
ssize_t DGram::RecvFrom(void *buf, size_t length) {
    ssize_t ret;
    if (family_ == AF_INET) {
        recv_addr_length_ = sizeof(struct sockaddr_in);
        ret               = recvfrom(fd_,
                       buf,
                       length,
                       0,
                       reinterpret_cast<struct sockaddr *>(&from_addr_),
                       &recv_addr_length_);
    } else if (family_ == AF_INET6) {
        recv_addr_length_ = sizeof(struct sockaddr_in6);
        ret               = recvfrom(fd_,
                       buf,
                       length,
                       0,
                       reinterpret_cast<struct sockaddr *>(&from_addr_),
                       &recv_addr_length_);
    } else {
        return -1;
    }
    return ret;
}

int DGram::Open(const std::string &type, bool is_broadcast) {
    if (type.compare("AF_INET") == 0) {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET;
    } else if (type.compare("udp4") == 0) {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET;
    } else if (type.compare("UDP4") == 0) {
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET;
    } else if (type.compare("AF_INET6") == 0) {
        fd_ = socket(AF_INET6, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET6;
    } else if (type.compare("udp6") == 0) {
        fd_ = socket(AF_INET6, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET6;
    } else if (type.compare("UDP6") == 0) {
        fd_ = socket(AF_INET6, SOCK_DGRAM, 0);
        if (fd_ == -1) {
            DGRAM_PERROR("socket");
            return -1;
        }
        family_ = AF_INET6;
    } else {
        return -1;
    }

    int on = 1;
    if (setsockopt(fd_,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   reinterpret_cast<char *>(&on),
                   sizeof(on)) < 0) {
        DGRAM_PERROR("setsockopt");
        close(fd_);
        fd_     = -1;
        family_ = 0;
        return -1;
    }

    if (is_broadcast) {
        if (setsockopt(fd_,
                       SOL_SOCKET,
                       SO_BROADCAST,
                       reinterpret_cast<char *>(&on),
                       sizeof(on)) < 0) {
            DGRAM_PERROR("setsockopt");
            close(fd_);
            fd_     = -1;
            family_ = 0;
            return -1;
        }
    }

    /*
    // recvbuffer..
    int opt;
    socklen_t optlen = sizeof(opt);
    if (getsockopt(fd_, SOL_SOCKET, SO_RCVBUF, (void *)&opt, &optlen) <
    0) { DGRAM_PERROR("setsockopt"); close(fd_); fd_ = -1;
        family_ = 0;
    } else {
        //printf("REVBUF:%d\n", opt);
        ;
    }
    if (getsockopt(fd_, SOL_SOCKET,  SO_SNDBUF, (void *)&opt, &optlen) <
    0) { DGRAM_PERROR("setsockopt"); close(fd_); fd_ = -1;
        family_ = 0;
    } else {
        //printf("SNDBUF:%d\n", opt);
        ;
    }
    */

    return fd_;
}

bool DGram::SetTo(const std::string &dest_host,
                  const std::string &dest_port) {
    int error;
    struct addrinfo *res;
    struct addrinfo hints;

    if (family_ == 0) {
        return false;
    }

    memset(&hints, 0, sizeof(hints));

    hints.ai_family   = family_;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_ALL | AI_ADDRCONFIG;

    error = getaddrinfo(dest_host.c_str(), dest_port.c_str(), &hints, &res);
    if (error) {
#ifdef DEBUG
        // if (debug) {
        fprintf(stderr,
                "%s(%d):getaddrinfo: %s\n",
                __FILE__,
                __LINE__,
                gai_strerror(error));
        // }
#endif
        return false;
    }

    memcpy(&to_addr_, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    return true;
}
} // namespace neighbor_search
