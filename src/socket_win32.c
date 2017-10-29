#include<stdio.h>
#include<errno.h>
#include"socket.h"

int socket_open(int family,int type,int protocol)
{
	int n;
	if((n=socket(family,type,protocol))<0)
	{
		perror("socket open error");
	}
	return n;
}

void socket_close(int fd)
{
	if(close(fd)==-1)
	{
		perror("socket close error");
	}
	return;
}

int socket_accept(int fd,struct sockaddr *addr,socklen_t *len)
{
	int n;
	again:
		if((n=accept(fd,addr,len))<0)
		{
			if(errno==EINTR)
			{
				goto again;
			}
			else
			{
				perror("socket accept error");
			}
		}
		return n;
}


void socket_bind(int fd,const struct sockaddr *addr,socklen_t len)
{
	if(bind(fd,addr,len)<0)
	{
		perror("socket bind error");
	}
	return;
}

void socket_connect(int fd, const struct sockaddr *addr, socklen_t len)
{
    if (connect(fd, addr, len) < 0) {
        perror("socket connect error");
    }
    return;
}

void socket_listen(int fd, int backlog)
{
    if (listen(fd, backlog) < 0) {
        perror("socket listen error");
    }
    return ;
}

ssize_t socket_read(int fd, void *buf, size_t nbytes)
{
    ssize_t n;
again:
    if ((n = read(fd, buf, nbytes)) == -1) {
        if (errno == EINTR) {
            goto again;
        } else {
            return -1;
        }
    }

    return n;
}

ssize_t socket_readn(int fd,void *buf,size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *p;
	
	p=buf;
	nleft=n;
	while(nleft>0)
	{
		if((nread=read(fd,buf,nleft))<0)
		{
			if(errno=EINTR)
			{
				nread=0;
			}
			else
			{
				return -1;
			}
		
		}
		else if(nread==0)
		{
			break;
		}
		nleft-=nread;
		p+=nread;
	}
	return n-nleft;
}


ssize_t socket_write(int fd, const void *buf, size_t nbytes)
{
    ssize_t n;
again:
    if ((n = write(fd, buf, nbytes)) == -1) {
        if (errno == EINTR) {
            goto again;
        } else {
            return -1;
        }
    }
    return n;
}

ssize_t socket_writen(int fd, const void *buf, size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *p;

    p = buf;
    nleft = n;

    while (nleft > 0) {
        if ((nwritten = write(fd, buf, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR) {
                nwritten = 0;
            } else {
                return -1;
            }
        }
        nleft -= nwritten;
        p += nwritten;
    }
    return n;
}


ssize_t socket_recvfrom(int fd,void *buf,size_t len,int flags,struct sockaddr *src_addr,socklen_t *addr_len)
{
	int n=recvfrom(fd,buf,len,flags,src_addr,addr_len);
	if(n==-1)
   {
	perror("socket recvfrom error");
   }
    return n;

}

ssize_t socket_sendto(int fd, const void *buf, size_t len, int flags,
    const struct sockaddr *dst_addr, socklen_t addr_len)
{
    int n = sendto(fd, buf, len, flags, dst_addr, addr_len);
    if (n == -1) {
        perror("socket sendto error");
    }
    return n;
}

static const char *inet_ntop_v4(const void *src,char *dst,size_t size)
{
	const char digits[]="0123456789";
	int i;
	struct in_addr *addr=(struct in_addr *)src;
	u_long a=ntohl(addr->s_addr);
	const char *orig_dst=dst;
	
	if(size<INET_ADDRSTRLEN)
	{
		errno=ENOSPC;
		return NULL;
	}
	
	for(i=0;i<4;++i)
	{
		int n=(a>>(24-i*8))&0xFF;
		int non_zerop=0;
		
		if(non_zerop||n/100>0)
		{
			*dst++=digits[n/100];
			n%=100;
			non_zerop=1;
		}
		if(non_zerop||n/10>0)
		{
			*dst++=digits[n/10];
			n%=10;
			non_zerop=1; 
		}
		*dst++=digits[n];
		if(i!=3)
		{
			*dst++='.';
		}
	}
	*dst++='\0';
	return orig_dst;
}

const char *inet_ntop(int af,const void *src,char *dst,size_t size)
{
	switch(af)
	{
		case AF_INET:
			return inet_ntop_v4(src,dst,size);
		default:
			return NULL; 
	}
}
