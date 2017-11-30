extern "C"
{
  int TCPConnect(SOCKET *, char *, int);
  int TCPSend(SOCKET, char *, char *);
  int TCPClose(SOCKET);
  VOID DoSound(VOID);
}
