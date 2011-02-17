/* sigpipe-server.c
 *
 * Copyright (c) 2000 Sean Walton and Macmillan Publishers.  Use may be in
 * whole or in part in accordance to the General Public License (GPL).
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
*/

/*****************************************************************************/
/*** sigpipe-server.c                                                      ***/
/***                                                                       ***/
/*** Demonstrates how to generate a SIGPIPE error with a socket.           ***/
/*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>

#define DEFT_QSOCKET_MAX_CLNT_NUMS	  (5)
#define DEFT_SOCKET_LISTEN_PORT		  (9999)
#define DEFT_SOCKET_LISTEN_IP		  INADDR_ANY	/* 0.0.0.0 */

typedef struct _qsocket {
  /// socket fd;
  int srv_sd;
  int clnt_sd[DEFT_QSOCKET_MAX_CLNT_NUMS];
 
  fd_set rfds;

  /// server: ip:port;
  struct sockaddr_in srv_addr;

  /// clnt[]: ip:port;
  struct sockaddr_in clnt_addr[DEFT_QSOCKET_MAX_CLNT_NUMS];

} qsocket_type;

static qsocket_type qsock_core;

/*---------------------------------------------------------------------*/
/*--- sig_pipe - SIGPIPE                                            ---*/
/*---------------------------------------------------------------------*/
void sig_pipe(int signum)
{
	printf("got it!\n");
	signal(SIGPIPE, sig_pipe);
}

/*---------------------------------------------------------------------*/
/*--- initialize qsock                                              ---*/
/*---------------------------------------------------------------------*/
int init_qsock(qsocket_type* qs_ptr)
{
	qsocket_type *qs = NULL;
	int	ret = 0;

	if (qs_ptr == NULL)
	{
	  ret = -1;
	  goto ERR;
	}
	qs = qs_ptr;	

	// Create socket fd
	ret = qs->srv_sd = socket(PF_INET, SOCK_STREAM, 0);
	if ( ret < 0 )
	{
	  perror("Socket");
	  //abort();
	  goto ERR;
	}

	/// setup Addr and port
	qs->srv_addr.sin_family = PF_INET;
	qs->srv_addr.sin_port = htons(DEFT_SOCKET_LISTEN_PORT);
	qs->srv_addr.sin_addr.s_addr = htonl(DEFT_SOCKET_LISTEN_IP);

	// Bind socket addr and port
	ret = bind(qs->srv_sd, (struct sockaddr*)&(qs->srv_addr), 
		sizeof(struct sockaddr_in));
	if ( ret != 0 )
	{
	  perror("Bind");
	  //abort();
	  goto ERR;
	}

	// starting to listen qclnt connecting.
	ret = listen(qs->srv_sd, DEFT_SOCKET_LISTEN_PORT);
	if( ret != 0 )
	{
	  perror("Listen");
	  goto ERR;
	}
	FD_ZERO(&(qs->rfds));
	FD_SET(qs->srv_sd, &(qs->rfds));

ERR:
	return ret;
}

/*---------------------------------------------------------------------*/
/*--- accept qconnct                                                ---*/
/*---------------------------------------------------------------------*/
void accecpt_qconnct()
{
}

/*---------------------------------------------------------------------*/
/*--- req qclnt                                                 ---*/
/*---------------------------------------------------------------------*/
void req_qclnt()
{
}

/*---------------------------------------------------------------------*/
/*--- rsp qclnt                                                 ---*/
/*---------------------------------------------------------------------*/
void rsp_qclnt()
{
}

/*---------------------------------------------------------------------*/
/*--- close qconnct                                                 ---*/
/*---------------------------------------------------------------------*/
void close_qconnct()
{
}

/*---------------------------------------------------------------------*/
/*--- main - allow client to connect and then terminate.            ---*/
/*---------------------------------------------------------------------*/
int main(int argc, char* argv[])
{	
	struct sockaddr_in srv_addr;
	struct sockaddr_in clnt_addr;
	
	int srv_sd, byte_count, addr_size;
	int clnt_sd;
	char buffer[1024];
	fd_set rfds;
	int readfd, testfd;
	
	/// signal(SIGPIPE, sig_pipe);

	/** 1. OPEN SOCKET **/
	if ( (srv_sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
		abort();
	}

	/** 2. BIND ADDR&PORT **/
	srv_addr.sin_family = /*AF_INET*/PF_INET;
	srv_addr.sin_port = htons(9999);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if ( bind(srv_sd, (struct sockaddr*)&srv_addr, sizeof(srv_addr)) != 0 )
	{
		perror("Bind");
		abort();
	}

	/** 3. LISTEN **/
	listen(srv_sd, 5);
	FD_ZERO(&rfds);
	FD_SET(srv_sd, &rfds);
	
	while (1)
	{	
		int result;
		int nfds = 0;
		unsigned int port;

		nfds = MAX(nfds, srv_sd) +1;
		
		result = select(nfds, &rfds, (fd_set*) 0, (fd_set*)0, (struct timeval*)0);
		if( result < 0 )
		{
		  perror("Select");
		  exit(-1);
		}
		

		/** 4. WAITING & ACCEPT CLIENT **/
		if( FD_ISSET(srv_sd, &rfds) )
		{
		  printf("Server waiting...\n");
		  clnt_sd = accept(srv_sd, 
			  (struct sockaddr*)&srv_addr, (socklen_t*)&addr_size);

		  port = ntohs(srv_addr.sin_port);
		  printf("Connected: %s:%u\n",
			  inet_ntoa(srv_addr.sin_addr), port);

		  /** 5. TRANSMIT DATA **/
		  byte_count = recv(clnt_sd, buffer, sizeof(buffer), 0);
		  if (byte_count == 0)
		  {	/// recv nothing
		  }
		  else
		  {
			  printf("%s\n", buffer);
			  memset((void*)buffer, (int)0x0, (size_t)sizeof(buffer));
		  }
		  sleep(1);
		  {
			  char *end = "EXIT";
			  byte_count = strlen(end);
			  printf("Conn exited.\n");
			  send(clnt_sd, (const void*)end, (size_t)byte_count, 0);
		  }

		  /** 6. CLOSE CLIENT CONNETION **/
		  close(clnt_sd);
		}
	}	/* end of while(1) */

	/** 7. CLOSE SOCKET **/
	close(srv_sd);
	exit(0);
}

