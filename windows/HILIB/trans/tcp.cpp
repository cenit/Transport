#include "allincs.h"

#define	MAXLEN	    2048	/* max command buffer length */
#define	BACKLOG	    5		/* max outstanding connections	*/
#define	FLAGS	    0		/* flags argument, must be zero	*/
#ifdef WIN32
#define errno WSAGetLastError()
#endif

extern "C"
{
   int alreadybox;  // global
   char MBtitle[] = "Transport DACs via TCP/IP";
//   char MBtitle[] = "Set Point via TCP/IP";
//   char MBtitle[] = "Optima via TCP/IP";
//   char MBtitle[] = "Combi Control via TCP/IP";
//   char MBtitle[] = "Magnet Cycling via TCP/IP";
//   char MBtitle[] = "Interactive CAMAC Command via TCP/IP";
//   char MBtitle[] = "Beam Blocker Control via TCP/IP";
   char Domain[] = ".psi.ch";
}

void MyYield(void)
{
/*
   check if any window message is available and send this message to
   the destination application. Call this function to maintain window's
   nonpreemtive multitasking abilities. (By Stefan Ritt)
   This routine can be imported by FORTRAN DLLs.
*/
   MSG msg;

   while (PeekMessage(&msg, 0, 0, 0, TRUE))
   {  TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

int TCPConnect(SOCKET *sock, char *server, int port)
{
    WORD    wVersionRequested;
    WSADATA wsaData;
    int     err;
    SOCKET  l_socket;
    struct  sockaddr_in	l_socket_s;
    struct  sockaddr_in	r_socket_s;
    struct  hostent *localhost_p;
    struct  hostent *remothost_p;
    char    localhost_s[20];
    char    remothost_s[20];
    char    buffer[100];
    int     status;
    int     namelength;
    char    serverext[30];

    strcpy(serverext,server);
    strcat(serverext,Domain);

    alreadybox = FALSE;

    /*  Start WinSocket-DLL  */

    wVersionRequested = MAKEWORD(1,1);
    err = WSAStartup(wVersionRequested,&wsaData);
    if (err != 0)
    {
	MessageBox(NULL,">> WinSock not present or not responding <<",
                        MBtitle,MB_OK);
	return 0;
    }

    /*	Create a socket  */

    l_socket	=   socket ( AF_INET, SOCK_STREAM, 0 );
    if ( l_socket == (SOCKET)-1 )
    {
	MessageBox(NULL,">> Error Creating Socket <<",MBtitle,MB_OK);
	return 0;
    }

    /*	Try to get hostname via name server */

    l_socket_s.sin_family	=   AF_INET;
    l_socket_s.sin_port		=   htons ( 0 );
    gethostname(localhost_s,sizeof(localhost_s));
    localhost_p   =   (struct hostent *) gethostbyname ( localhost_s );
    if ( localhost_p == NULL ) // not found in name server
    {
        if (strchr(server,'.') == NULL)  /* no dot found */
        {
           MessageBox(NULL,">> Cannot get hostaddress of your PC <<",
	              MBtitle,MB_OK);
           return 0;
        }
        else  // for local tests: server == host (tcp-ip quadrupel as address)
        {
           l_socket_s.sin_addr.s_addr = inet_addr(server);
        }
    }
    else  // found in name server
    {
        l_socket_s.sin_addr.s_addr	=   0;
        memcpy((char *)&(l_socket_s.sin_addr),
               (char *)localhost_p->h_addr,
               localhost_p->h_length);
    }

    /*	bind socket to port  */

    status = bind ( l_socket, (LPSOCKADDR)&l_socket_s, sizeof(l_socket_s));

    if ( status == -1 )
    {
	sprintf(buffer,">> Error Binding Socket, errno = %d <<",errno);
        MessageBox(NULL,buffer,MBtitle,MB_OK);
        shutdown ( l_socket , 2 );
	return 0;
    }

    /*	Interpret 'server' as server name ... */

    if (strchr(server,'.') == NULL)  /* no dot found */
    {
       strcpy(remothost_s,serverext);
       remothost_p   =   (struct hostent *) gethostbyname ( remothost_s );
       if ( remothost_p == NULL )
       {
           MessageBox(NULL,">> Cannot get hostaddress <<",MBtitle,MB_OK);
           return 0;
       }
       r_socket_s.sin_family = AF_INET;
       r_socket_s.sin_port = htons ( (unsigned short)port );
       r_socket_s.sin_addr.s_addr = *(unsigned long *) *(remothost_p->h_addr_list);
    }

    /* ... or as internet address */

    else
    {
       r_socket_s.sin_family = AF_INET;
       r_socket_s.sin_port = htons ( (unsigned short)port );
       r_socket_s.sin_addr.s_addr = inet_addr(server);
    }

    /*  Connect to Server socket  */

    namelength = sizeof ( r_socket_s );
    status     = connect ( l_socket, (LPSOCKADDR)&r_socket_s, namelength );

    if ( status == -1)
    {
	if (!alreadybox)
	{
	   alreadybox = TRUE;
	   sprintf(buffer,
	    ">> Connection to server refused, errno = WSABASEERR+%d <<",
	    errno-WSABASEERR);
	   DoSound();
	   MessageBox(NULL,buffer,MBtitle, MB_ICONSTOP|MB_OK);
	}
        shutdown ( l_socket , 2 );
	return 0;
    }
    *sock = l_socket;
    return 1;
}

int TCPSend(SOCKET l_socket, char *command_s, char *r_buffer)
{
    int	   command_length;
    int	   count;
    char message[]  = "Disconnected from server, shall I try to reconnect ?";
    char message2[] = "Error while selecting socket.";
    char message3[] = "Timeout Error while receiving data.";

    if (strlen(command_s) > 0)
    {
//	   send command

           command_length = strlen(command_s) + 1;  // send also zero termination
	   count = send ( l_socket, command_s, command_length, FLAGS );
	   if ( count <= 0 )
	   {
		if (!alreadybox)
		{
		   alreadybox = TRUE;
		   DoSound();
		   if (MessageBox(NULL,message,
		      MBtitle,MB_ICONQUESTION|MB_YESNO) == IDYES)
		   {
		      TCPClose(l_socket);
		      return -1;
		   }
		   else
		      return 0;
		}
           }

//	   receive answer

	   int z = 0;
	   while(1)
	   {
#define NMAL 40
	      int msecs = 100;
	      if (msecs > 0)
	      {
		for (int j=0; j<NMAL; j++)  // total timeout time = msecs * NMAL
		{
		  fd_set readfds;
		  FD_ZERO(&readfds);
		  FD_SET(l_socket,&readfds);
		  struct timeval timeout;
		  timeout.tv_sec  = msecs / 1000;
		  timeout.tv_usec = (msecs % 1000) * 1000;
		  int status = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
		  if (status == -1)
		  {
		     DoSound();
		     MessageBox(NULL,message2,MBtitle,MB_ICONINFORMATION|MB_OK);
		     TCPClose(l_socket);
		     return -1;
		  }
		  if (!FD_ISSET(l_socket,&readfds)) // timeout occurred
		  {
		     if (j >= NMAL - 1)
		     {
			DoSound();
			MessageBox(NULL,message3,MBtitle,MB_ICONEXCLAMATION|MB_OK);
			TCPClose(l_socket);
			return -1;
		     }
		  }
		  else
		     MyYield();
		}
	      }

	      count = recv ( l_socket, &r_buffer[z], MAXLEN, FLAGS );
	      if ( count <= 0)
	      {
		if (!alreadybox)
		{
		   alreadybox = TRUE;
		   DoSound();
		   if (MessageBox(NULL,message,
		      MBtitle,MB_ICONQUESTION|MB_YESNO) == IDYES)
		   {
		      TCPClose(l_socket);
		      return -1;
		   }
		   else
		      return 0;
		}
	      }
	      else
		z += count;

	      if (r_buffer[z-1] == '\0')
		break;
	   }
           return 1;
    }
    return 0;
}

int TCPClose(SOCKET l_socket)
{
    closesocket( l_socket );
    return 1;
}

