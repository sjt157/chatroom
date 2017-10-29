#ifndef SOCKET_H
#define SOCKET_H

#if defined(__WIN32__)

#include <winsock2.h>

#define INET_ADDRSTRLEN 16

typedef int socklen_t;

const char *inet_ntop(int af, const void *src, char *dst, size_t size);

#else /* defined(__linux__) */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#endif /* defined(__WIN32__) */

#include <unistd.h>

int socket_open(int family, int type, int protocol);
void socket_close(int fd);
int socket_accept(int fd, struct sockaddr *addr, socklen_t *len);
void socket_bind(int fd, const struct sockaddr *addr, socklen_t len);
void socket_connect(int fd, const struct sockaddr *addr, socklen_t len);
void socket_listen(int fd, int backlog);
ssize_t socket_read(int fd, void *buf, size_t nbytes);
ssize_t socket_readn(int fd, void *buf, size_t n);
ssize_t socket_write(int fd, const void *buf, size_t nbytes);
ssize_t socket_writen(int fd, const void *buf, size_t n);
ssize_t socket_recvfrom(int fd, void *buf, size_t len, int flags,
    struct sockaddr *src_addr, socklen_t *addr_len);
ssize_t socket_sendto(int fd, const void *buf, size_t len, int flags,
    const struct sockaddr *dst_addr, socklen_t addr_len);

#endif /* SOCKET_H */

