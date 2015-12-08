#include "stdafx.h"
#include "mySocket.h"

#include <algorithm>

extern BOOL bReceivingMessageTotalJobs;
extern CTestNetClientDlg dlg;
//char properarea[1020];
//extern int Num2Collect; //**temporarily made global

const int MSG_HEADER_LEN = 2;
const int MSG_HEADER_LENp1 = 3;

mySocket::mySocket(int pNumber)
{
    portNumber = pNumber;
    blocking = 1;

    try
    {
        if ( (socketId=socket(AF_INET,SOCK_STREAM,0)) == -1)
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "";
                detectErrorOpenWinSocket(&errorCode,errorMsg);
                myException openWinSocketException(errorCode,errorMsg);
                throw openWinSocketException;
            #endif

            #ifdef UNIX
                myException openUnixSocketException(0,"unix: error getting host by name");
                throw openUnixSocketException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }

    /* 
       set the initial address of client that shall be communicated with to 
       any address as long as they are using the same port number. 
       The clientAddr structure is used in the future for storing the actual
       address of client applications with which communication is going
       to start
    */
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons((u_short)portNumber);
}
    
void mySocket::setDebug(int debugToggle)
{
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_DEBUG,(char *)&debugToggle,sizeof(debugToggle)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "DEBUG option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

void mySocket::setReuseAddr(int reuseToggle)
{
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_REUSEADDR,(char *)&reuseToggle,sizeof(reuseToggle)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "REUSEADDR option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
} 

void mySocket::setKeepAlive(int aliveToggle)
{
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_KEEPALIVE,(char *)&aliveToggle,sizeof(aliveToggle)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "ALIVE option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
} 

void mySocket::setLingerSeconds(int seconds)
{
    struct linger lingerOption;
    
    if ( seconds > 0 )
    {
        lingerOption.l_linger = (u_short)seconds;
        lingerOption.l_onoff = 1;
    }
    else lingerOption.l_onoff = 0;
     
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,sizeof(struct linger)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "LINGER option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

void mySocket::setLingerOnOff(bool lingerOn)
{
    struct linger lingerOption;

    if ( lingerOn ) lingerOption.l_onoff = 1;
    else lingerOption.l_onoff = 0;

    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,sizeof(struct linger)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "LINGER option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

void mySocket::setSendBufSize(int sendBufSize)
{
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_SNDBUF,(char *)&sendBufSize,sizeof(sendBufSize)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "SENDBUFSIZE option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
} 

void mySocket::setReceiveBufSize(int receiveBufSize)
{
    try 
    {
        if ( setsockopt(socketId,SOL_SOCKET,SO_RCVBUF,(char *)&receiveBufSize,sizeof(receiveBufSize)) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "RCVBUF option:";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

void mySocket::setSocketBlocking(int blockingToggle)
{
    if (blockingToggle)
    {
        if (getSocketBlocking()) return;
        else blocking = 1;
    }
    else
    {
        if (!getSocketBlocking()) return;
        else blocking = 0;
    }

    try 
    {
        #ifdef WINDOWS_XP
            if (ioctlsocket(socketId,FIONBIO,(unsigned long *)&blocking) == -1)
            {
                int errorCode;
                string errorMsg = "Blocking option: ";
                detectErrorSetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            }
        #endif

        #ifdef UNIX
            if (ioctl(socketId,FIONBIO,(char *)&blocking) == -1)
            {
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            }
        #endif
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

int mySocket::getDebug()
{
    int myOption;
    int myOptionLen = sizeof(myOption);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_DEBUG,(char *)&myOption,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get DEBUG option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    
    return myOption;
}

int mySocket::getReuseAddr()
{
    int myOption;        
    int myOptionLen = sizeof(myOption);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_REUSEADDR,(char *)&myOption,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get REUSEADDR option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }

    return myOption;
}

int mySocket::getKeepAlive()
{
    int myOption;        
    int myOptionLen = sizeof(myOption);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_KEEPALIVE,(char *)&myOption,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get KEEPALIVE option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    return myOption;    
}

int mySocket::getLingerSeconds()
{
    struct linger lingerOption;
    int myOptionLen = sizeof(struct linger);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get LINER option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }

    return lingerOption.l_linger;
}

bool mySocket::getLingerOnOff()
{
    struct linger lingerOption;
    int myOptionLen = sizeof(struct linger);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_LINGER,(char *)&lingerOption,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get LINER option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }

    if ( lingerOption.l_onoff == 1 ) return true;
    else return false;
}

int mySocket::getSendBufSize()
{
    int sendBuf;
    int myOptionLen = sizeof(sendBuf);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_SNDBUF,(char *)&sendBuf,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get SNDBUF option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    return sendBuf;
}    

int mySocket::getReceiveBufSize()
{
    int rcvBuf;
    int myOptionLen = sizeof(rcvBuf);

    try 
    {
        if ( getsockopt(socketId,SOL_SOCKET,SO_RCVBUF,(char *)&rcvBuf,&myOptionLen) == -1 )
        {
            #ifdef WINDOWS_XP
                int errorCode;
                string errorMsg = "get RCVBUF option: ";
                detectErrorGetSocketOption(&errorCode,errorMsg);
                myException socketOptionException(errorCode,errorMsg);
                throw socketOptionException;
            #endif

            #ifdef UNIX
                myException unixSocketOptionException = new myException(0,"unix: error getting host by name");
                throw unixSocketOptionException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    return rcvBuf;
}

#ifdef WINDOWS_XP
void mySocket::detectErrorOpenWinSocket(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("Successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem or the associated service provider has failed.");
    else if ( *errCode == WSAEAFNOSUPPORT )
        errMsg.append("The specified address family is not supported.");
    else if ( *errCode == WSAEINPROGRESS )
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
    else if ( *errCode == WSAEMFILE )
        errMsg.append("No more socket descriptors are available.");
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("No buffer space is available. The socket cannot be created.");
    else if ( *errCode == WSAEPROTONOSUPPORT )
        errMsg.append("The specified protocol is not supported.");
    else if ( *errCode == WSAEPROTOTYPE )
        errMsg.append("The specified protocol is the wrong type for this socket.");
    else if ( *errCode == WSAESOCKTNOSUPPORT )
        errMsg.append("The specified socket type is not supported in this address family.");
    else errMsg.append("unknown problems!");
}

void mySocket::detectErrorSetSocketOption(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEFAULT )
        errMsg.append("optval is not in a valid part of the process address space or optlen parameter is too small.");
    else if ( *errCode == WSAEINPROGRESS )
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
    else if ( *errCode == WSAEINVAL )
        errMsg.append("level is not valid, or the information in optval is not valid.");
    else if ( *errCode == WSAENETRESET )
        errMsg.append("Connection has timed out when SO_KEEPALIVE is set.");
    else if ( *errCode == WSAENOPROTOOPT )
        errMsg.append("The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations).");
    else if ( *errCode == WSAENOTCONN )
        errMsg.append("Connection has been reset when SO_KEEPALIVE is set.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else errMsg.append("unknown problem!");
}

void mySocket::detectErrorGetSocketOption(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEFAULT )
        errMsg.append("One of the optval or the optlen parameters is not a valid part of the user address space, or the optlen parameter is too small.");
    else if ( *errCode == WSAEINPROGRESS )
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
    else if ( *errCode == WSAEINVAL )
        errMsg.append("The level parameter is unknown or invalid.");
    else if ( *errCode == WSAENOPROTOOPT )
        errMsg.append("The option is unknown or unsupported by the indicated protocol family.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");

    else errMsg.append("unknown problems!");
}

#endif

ostream& operator<<(ostream& io,mySocket& s)
{
    string flagStr = "";

    io << endl;
    io << "Summary of socket settings:" << endl;
    io << "   Socket Id:     " << s.getSocketId() << endl;
    io << "   port #:        " << s.getPortNumber() << endl;
    io << "   debug:         " << (flagStr = s.getDebug()? "true":"false" ) << endl;
    io << "   reuse addr:    " << (flagStr = s.getReuseAddr()? "true":"false" ) << endl;
    io << "   keep alive:    " << (flagStr = s.getKeepAlive()? "true":"false" ) << endl;
    io << "   send buf size: " << s.getSendBufSize() << endl;
    io << "   recv bug size: " << s.getReceiveBufSize() << endl;
    io << "   blocking:      " << (flagStr = s.getSocketBlocking()? "true":"false" ) << endl;
    io << "   linger on:     " << (flagStr = s.getLingerOnOff()? "true":"false" ) << endl;
    io << "   linger seconds: " << s.getLingerSeconds() << endl;
    io << endl;
    return io;
}

myTcpSocket::~myTcpSocket()
{
#ifdef WINDOWS_XP
    // Close the winsock library
    winLog << endl << "Socket shut down ...";		
    try 
    {
        if (WSACleanup())
        {
            myException cleanupException(0,"Error: calling WSACleanup()");
            throw cleanupException;
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    winLog << "successful" << endl<< endl<< endl;
#endif
}

void myTcpSocket::initialize()
{
#ifdef WINDOWS_XP
    WSADATA wsaData;
//  winLog << endl << "system started ..." << endl;
    winLog << endl << "Initialize the winsock library ... ";
    try 
    {
        if (WSAStartup(0x101, &wsaData))
        {
            myException initializationException(0,"Error: calling WSAStartup()");
            throw initializationException;
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
    winLog << "successful" << endl;
#endif
}

void myTcpSocket::bindSocket()
{
    try
    {
        if (bind(socketId,(struct sockaddr *)&clientAddr,sizeof(struct sockaddr_in))==-1)
        {
            #ifdef WINDOWS_XP
                int errorCode = 0;
                string errorMsg = "error calling bind(): \n";
                detectErrorBind(&errorCode,errorMsg);
                myException socketBindException(errorCode,errorMsg);
                throw socketBindException;
            #endif
            #ifdef UNIX
                myException unixSocketBindException(0,"unix: error calling bind()");
                throw unixSocketBindException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}

#ifdef WINDOWS_XP

void myTcpSocket::detectErrorBind(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEADDRINUSE )
    {
        errMsg.append("A process on the machine is already bound to the same\n");
        errMsg.append("fully-qualified address and the socket has not been marked\n"); 
        errMsg.append("to allow address re-use with SO_REUSEADDR. For example,\n");
        errMsg.append("IP address and port are bound in the af_inet case");
    }
    else if ( *errCode == WSAEADDRNOTAVAIL )
        errMsg.append("The specified address is not a valid address for this machine.");
    else if ( *errCode == WSAEFAULT )
    {
        errMsg.append("The name or the namelen parameter is not a valid part of\n");
        errMsg.append("the user address space, the namelen parameter is too small,\n");
        errMsg.append("the name parameter contains incorrect address format for the\n");
        errMsg.append("associated address family, or the first two bytes of the memory\n");
        errMsg.append("block specified by name does not match the address family\n");
        errMsg.append("associated with the socket descriptor s.");
    }
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
        errMsg.append("service provider is still processing a callback function.");
    }
    else if ( *errCode == WSAEINVAL )
        errMsg.append("The socket is already bound to an address. ");
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("Not enough buffers available, too many connections.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else errMsg.append("unknown problems!");
}

void myTcpSocket::detectErrorRecv(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEFAULT )
        errMsg.append("The buf parameter is not completely contained in a valid part of the user address space.");
    else if ( *errCode == WSAENOTCONN )
        errMsg.append("The socket is not connected.");
    else if ( *errCode == WSAEINTR )
        errMsg.append("The (blocking) call was canceled through WSACancelBlockingCall.");
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
        errMsg.append("service provider is still processing a callback function.");
    }
    else if ( *errCode == WSAENETRESET )
    {
        errMsg.append("The connection has been broken due to the keep-alive activity\n");
        errMsg.append("detecting a failure while the operation was in progress.");
    }
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else if ( *errCode == WSAEOPNOTSUPP )
    {
        errMsg.append("MSG_OOB was specified, but the socket is not stream-style\n");
        errMsg.append("such as type SOCK_STREAM, out-of-band data is not supported\n");
        errMsg.append("in the communication domain associated with this socket, or\n");
        errMsg.append("the socket is unidirectional and supports only send operations.");
    }
    else if ( *errCode == WSAESHUTDOWN )
    {
        errMsg.append("The socket has been shut down; it is not possible to recv on a\n");
        errMsg.append("socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.");
    }
    else if ( *errCode == WSAEWOULDBLOCK )
        errMsg.append("The socket is marked as nonblocking and the receive operation would block.");
    else if ( *errCode == WSAEMSGSIZE )
        errMsg.append("The message was too large to fit into the specified buffer and was truncated.");
    else if ( *errCode == WSAEINVAL )
    {
        errMsg.append("The socket has not been bound with bind, or an unknown flag\n");
        errMsg.append("was specified, or MSG_OOB was specified for a socket with\n");
        errMsg.append("SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.");
    }
    else if ( *errCode == WSAECONNABORTED )
    {
        errMsg.append("The virtual circuit was terminated due to a time-out or\n");
        errMsg.append("other failure. The application should close the socket as it is no longer usable.");
    }
    else if ( *errCode == WSAETIMEDOUT )
    {
        errMsg.append("The connection has been dropped because of a network\n");
        errMsg.append("failure or because the peer system failed to respond.");
    }
    else if ( *errCode == WSAECONNRESET )
    {
        errMsg.append("The virtual circuit was reset by the remote side executing a\n");
        errMsg.append("\"hard\" or \"abortive\" close. The application should close\n");
        errMsg.append("the socket as it is no longer usable. On a UDP datagram socket\n");
        errMsg.append("this error would indicate that a previous send operation\n");
        errMsg.append("resulted in an ICMP \"Port Unreachable\" message.");
    }
    else errMsg.append("unknown problems!");
}

void myTcpSocket::detectErrorConnect(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEADDRINUSE )
    {
        errMsg.append("The socket's local address is already in use and the socket\n");
        errMsg.append("was not marked to allow address reuse with SO_REUSEADDR. This\n");
        errMsg.append("error usually occurs when executing bind, but could be delayed\n");
        errMsg.append("until this function if the bind was to a partially wild-card\n");
        errMsg.append("address (involving ADDR_ANY) and if a specific address needs\n");
        errMsg.append("to be committed at the time of this function.");
    }
    else if ( *errCode == WSAEINTR )
        errMsg.append("The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or\n");
        errMsg.append("the service provider is still processing a callback function.");
    }
    else if ( *errCode == WSAEALREADY )
    {
        errMsg.append("A nonblocking connect call is in progress on the specified socket.\n");
        errMsg.append("Note In order to preserve backward compatibility, this error is\n");
        errMsg.append("reported as WSAEINVAL to Windows Sockets 1.1 applications that\n");
        errMsg.append("link to either WINSOCK.DLL or WSOCK32.DLL.");
    }
    else if ( *errCode == WSAEADDRNOTAVAIL )
        errMsg.append("The remote address is not a valid address (such as ADDR_ANY).");
    else if ( *errCode == WSAEAFNOSUPPORT )
        errMsg.append("Addresses in the specified family cannot be used with this socket.");
    else if ( *errCode == WSAECONNREFUSED )
        errMsg.append("The attempt to connect was forcefully rejected.");
    else if ( *errCode == WSAEFAULT )
    {
        errMsg.append("The name or the namelen parameter is not a valid part of\n");
        errMsg.append("the user address space, the namelen parameter is too small,\n");
        errMsg.append("or the name parameter contains incorrect address format for\n");
        errMsg.append("the associated address family.");
    }
    else if ( *errCode == WSAEINVAL )
    {
        errMsg.append("The parameter s is a listening socket, or the destination\n");
        errMsg.append("address specified is not consistent with that of the constrained\n");
        errMsg.append("group the socket belongs to.");
    }
    else if ( *errCode == WSAEISCONN )
        errMsg.append("The socket is already connected (connection-oriented sockets only).");
    else if ( *errCode == WSAENETUNREACH )
        errMsg.append("The network cannot be reached from this host at this time.");
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("No buffer space is available. The socket cannot be connected.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else if ( *errCode == WSAETIMEDOUT )
        errMsg.append("Attempt to connect timed out without establishing a connection.");
    else if ( *errCode == WSAEWOULDBLOCK )
    {
        errMsg.append("The socket is marked as nonblocking and the connection\n");
        errMsg.append("cannot be completed immediately.");
    }
    else if ( *errCode == WSAEACCES )
    {
        errMsg.append("Attempt to connect datagram socket to broadcast address failed\n");
        errMsg.append("because setsockopt option SO_BROADCAST is not enabled.");
    }
    else errMsg.append("unknown problems!");
}

void myTcpSocket::detectErrorAccept(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this FUNCTION.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEFAULT )
        errMsg.append("The addrlen parameter is too small or addr is not a valid part of the user address space.");
    else if ( *errCode == WSAEINTR )
        errMsg.append("A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the\n");
        errMsg.append("service provider is still processing a callback function.");
    }
    else if ( *errCode == WSAEINVAL )
        errMsg.append("The listen function was not invoked prior to accept.");
    else if ( *errCode == WSAEMFILE )
        errMsg.append("The queue is nonempty upon entry to accept and there are no descriptors available.");
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("No buffer space is available.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else if ( *errCode == WSAEOPNOTSUPP )
        errMsg.append("The referenced socket is not a type that supports connection-oriented service.");
    else if ( *errCode == WSAEWOULDBLOCK )
        errMsg.append("The socket is marked as nonblocking and no connections are present to be accepted.");
    else errMsg.append("unknown problems!");
}

void myTcpSocket::detectErrorListen(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEADDRINUSE )
    {
        errMsg.append("The socket's local address is already in use and the socket was\n");
        errMsg.append("not marked to allow address reuse with SO_REUSEADDR. This error\n");
        errMsg.append("usually occurs during execution of the bind function, but could\n");
        errMsg.append("be delayed until this function if the bind was to a partially\n");
        errMsg.append("wild-card address (involving ADDR_ANY) and if a specific address\n");
        errMsg.append("needs to be \"committed\" at the time of this function.");
    }
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress, or the service\n");
        errMsg.append("provider is still processing a callback function.");
    }
    else if ( *errCode == WSAEINVAL )
        errMsg.append("The socket has not been bound with bind.");
    else if ( *errCode == WSAEISCONN )
        errMsg.append("The socket is already connected.");
    else if ( *errCode == WSAEMFILE )
        errMsg.append("No more socket descriptors are available.");
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("No buffer space is available.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else if ( *errCode == WSAEOPNOTSUPP )
        errMsg.append("The referenced socket is not of a type that supports the listen operation.");
    else errMsg.append("unknown problems!");
}

void myTcpSocket::detectErrorSend(int* errCode,string& errMsg)
{
    *errCode = WSAGetLastError();

    if ( *errCode == WSANOTINITIALISED )
        errMsg.append("A successful WSAStartup must occur before using this function.");
    else if ( *errCode == WSAENETDOWN )
        errMsg.append("The network subsystem has failed.");
    else if ( *errCode == WSAEACCES )
    {
        errMsg.append("The requested address is a broadcast address,\n");
        errMsg.append("but the appropriate flag was not set. Call setsockopt\n");
        errMsg.append("with the SO_BROADCAST parameter to allow the use of the broadcast address.");
    }
    else if ( *errCode == WSAEINTR )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call was canceled\n");
        errMsg.append("through WSACancelBlockingCall.");
    }
    else if ( *errCode == WSAEINPROGRESS )
    {
        errMsg.append("A blocking Windows Sockets 1.1 call is in progress,\n");
        errMsg.append("or the service provider is still processing a callback function.");
    }
    else if ( *errCode == WSAEFAULT )
    {
        errMsg.append("The buf parameter is not completely contained in a\n");
        errMsg.append("valid part of the user address space.");
    }
    else if ( *errCode == WSAENETRESET )
    {
        errMsg.append("The connection has been broken due to the keep-alive\n");
        errMsg.append("activity detecting a failure while the operation was in progress.");
    }
    else if ( *errCode == WSAENOBUFS )
        errMsg.append("No buffer space is available.");
    else if ( *errCode == WSAENOTCONN )
        errMsg.append("The socket is not connected.");
    else if ( *errCode == WSAENOTSOCK )
        errMsg.append("The descriptor is not a socket.");
    else if ( *errCode == WSAEOPNOTSUPP )
    {
        errMsg.append("MSG_OOB was specified, but the socket is not stream-style\n");
        errMsg.append("such as type SOCK_STREAM, out-of-band data is not supported\n");
        errMsg.append("in the communication domain associated with this socket,\n");
        errMsg.append("or the socket is unidirectional and supports only receive operations.");
    }
    else if ( *errCode == WSAESHUTDOWN )
    {
        errMsg.append("The socket has been shut down; it is not possible to send\n");
        errMsg.append("on a socket after shutdown has been invoked with how set\n");
        errMsg.append("to SD_SEND or SD_BOTH.");
    }
    else if ( *errCode == WSAEWOULDBLOCK )
        errMsg.append("The socket is marked as nonblocking and the requested operation would block.\n");
    else if ( *errCode == WSAEMSGSIZE )
    {
        errMsg.append("The socket is message oriented, and the message is larger\n");
        errMsg.append("than the maximum supported by the underlying transport.");
    }
    else if ( *errCode == WSAEHOSTUNREACH )
        errMsg.append("The remote host cannot be reached from this host at this time.");
    else if ( *errCode == WSAEINVAL )
    {
        errMsg.append("The socket has not been bound with bind, or an unknown flag\n");
        errMsg.append("was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
    }
    else if ( *errCode == WSAECONNABORTED )
    {
        errMsg.append("The virtual circuit was terminated due to a time-out or \n");
        errMsg.append("other failure. The application should close the socket as it is no longer usable.");
    }
    else if ( *errCode == WSAECONNRESET )
    {
        errMsg.append("The virtual circuit was reset by the remote side executing a \"hard\" \n");
        errMsg.append("or \"abortive\" close. For UPD sockets, the remote host was unable to\n");
        errMsg.append("deliver a previously sent UDP datagram and responded with a\n");
        errMsg.append("\"Port Unreachable\" ICMP packet. The application should close\n");
        errMsg.append("the socket as it is no longer usable.");
    }
    else if ( *errCode == WSAETIMEDOUT )
    {
        errMsg.append("The connection has been dropped, because of a network failure\n");
        errMsg.append("or because the system on the other end went down without notice.");
    }
    else errMsg.append("unknown problems!");
}

#endif

//now returns zero for success 
int myTcpSocket::connectToServer(string& serverNameOrAddr,hostType hType)
{ 
    /*
       when this method is called, a client socket has been built already,
       so we have the socketId and portNumber ready.

       a myHostInfo instance is created, no matter how the server's name is
       given (such as www.yuchen.net) or the server's address is given (such
       as 169.56.32.35), we can use this myHostInfo instance to get the
       IP address of the server
    */

    myHostInfo serverInfo(serverNameOrAddr,hType);
    
    // Store the IP address and socket port number
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr(serverInfo.getHostIPAddress());
    serverAddress.sin_port = htons((u_short)portNumber);

    // Connect to the given address
    try
    {
        if (connect(socketId,(struct sockaddr *)&serverAddress,sizeof(serverAddress)) == -1)
        {
            #ifdef WINDOWS_XP
                int errorCode = 0;
                string errorMsg = "error calling connect():\n";
                detectErrorConnect(&errorCode,errorMsg);
                myException socketConnectException(errorCode,errorMsg);
                throw socketConnectException;
            #endif

            #ifdef UNIX
                myException unixSocketConnectException(0,"unix: error calling connect()");
                throw unixSocketConnectException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        //exit(1);  //mj
        return 1;
    }
 return 0;
}
/*
I've commented this function out to remove the compiler warning,
   C4239: nonstandard extension used : 'argument' : conversion from 'std::basic_string<_Elem,_Traits,_Ax>' to 'std::string &'
The function isn't used anyway.


myTcpSocket* myTcpSocket::acceptClient(string& clientHost)
{
    int newSocket;   // the new socket file descriptor returned by the accept systme call

    // the length of the client's address
    int clientAddressLen = sizeof(struct sockaddr_in);
    struct sockaddr_in clientAddress;    // Address of the client that sent data

    // Accepts a new client connection and stores its socket file descriptor
    try 
    {
        if ((newSocket = accept(socketId, (struct sockaddr *)&clientAddress,&clientAddressLen)) == -1)
        {
            #ifdef WINDOWS_XP
                int errorCode = 0;
                string errorMsg = "error calling accept(): \n";
                detectErrorAccept(&errorCode,errorMsg);
                myException socketAcceptException(errorCode,errorMsg);
                throw socketAcceptException;
            #endif

            #ifdef UNIX
                myException unixSocketAcceptException(0,"unix: error calling accept()");
                throw unixSocketAcceptException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        return NULL;
    }
    
    // Get the host name given the address
    char *sAddress = inet_ntoa((struct in_addr)clientAddress.sin_addr);
    myHostInfo clientInfo(string(sAddress),ADDRESS);
    char* hostName = clientInfo.getHostName();
    clientHost += string(hostName);
    
    // Create and return the new myTcpSocket object
    myTcpSocket* retSocket = new myTcpSocket();
    retSocket->setSocketId(newSocket);
    return retSocket;
} */

void myTcpSocket::listenToClient(int totalNumPorts)
{
    try 
    {
        if (listen(socketId,totalNumPorts) == -1)
        {
            #ifdef WINDOWS_XP
                int errorCode = 0;
                string errorMsg = "error calling listen():\n";
                detectErrorListen(&errorCode,errorMsg);
                myException socketListenException(errorCode,errorMsg);
                throw socketListenException;
            #endif

            #ifdef UNIX
                myException unixSocketListenException(0,"unix: error calling listen()");
                throw unixSocketListenException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }
}       

int myTcpSocket::sendMessage(string& message)
{
    int numBytes;  // the number of bytes sent

    /* 
       for each message to be sent, add a header which shows how long this message
       is. This header, regardless how long the real message is, will always be
       of the length MSG_HEADER_LEN.
    */

    char msgLength[MSG_HEADER_LENp1];
    sprintf_s(msgLength,MSG_HEADER_LENp1,"%6d",message.size());
    string sendMsg = string(msgLength);
    sendMsg += message;

    // Sends the message to the connected host
    try 
    {
        numBytes = send(socketId,sendMsg.c_str(),sendMsg.size(),0);
        if (numBytes == -1)
        {
            #ifdef WINDOWS_XP
                int errorCode = 0;
                string errorMsg = "error calling send():\n";
                detectErrorSend(&errorCode,errorMsg);
                myException socketSendException(errorCode,errorMsg);
                throw socketSendException;
            #endif

            #ifdef UNIX
                myException unixSocketSendException(0,"unix: error calling send()");
                throw unixSocketSendException;
            #endif
        }
    }
    catch(myException& excp)
    {
        excp.response();
        exit(1);
    }

    return numBytes;
}

void myTcpSocket::recieveMessage(string& message) {
 int numBytes;  //The number of bytes currently received from recv
 int nNoofJobs;
 basic_string <char>::size_type iPosS; //Index to the position of something we're looking for in the string (start of string)
 basic_string <char>::size_type iPosE; //Index to the position of something we're looking for in the string (  end of string)
 char charMsg[MAX_RECV_LEN+1];
 BOOL bLastCharIsNotNewLine = TRUE;    //Flag to indicate whether or not we have received a '\n' character (at the end of the buffer).
 BOOL bNotGotNewLineYet = TRUE;        //This is just for the first packet for JOBS_LIST_REQUEST
 try {
    if(bReceivingMessageTotalJobs){       //If this is set we are expecting multiple packets (because JOBS_LIST_REQUEST was sent).  We need to 
       bReceivingMessageTotalJobs = FALSE;// collect the number of jobs from the first packet - then we know how many packets to expect
       while( bNotGotNewLineYet ) {
          numBytes = recv(socketId,charMsg,MAX_RECV_LEN,0);//It is not guaranteed that this will return with the whole of the first packet the first
          if (numBytes == -1) {                            // time it is called
             int errorCode = 0;
             string errorMsg = "error calling recv1():\n";
             detectErrorRecv(&errorCode,errorMsg);
             myException socketRecvException(errorCode,errorMsg);
             throw socketRecvException;
            }
          charMsg[numBytes]=0;
          message += charMsg;
          for(int look4NewLine=0; look4NewLine < numBytes; look4NewLine++)
            if(charMsg[look4NewLine] == '\n'){
               look4NewLine = numBytes;    //break out of for loop
               bNotGotNewLineYet = FALSE;  //break out of while( bNotGotNewLineYet ) loop
             //As we have at least one packet.. We can safely extract the NoofJobs from it b4 going on to receive each job
               iPosE = message.find_first_of ('\n', 0);
               int extBytes = 0; //Extract the bytes between the Command delimiter and the Packet delimiter
               for(iPosS = message.find_first_of ('|', 0) +1 ; iPosS <= iPosE; iPosS++)
                 charMsg[extBytes++] = message[iPosS];
               charMsg[extBytes] = 0;
               nNoofJobs = atoi( charMsg );
              }
         }
       //nNoofJobs is set. I guess it could be zero, in which case just return
       if(nNoofJobs){//There is at least one more packet expected... but have we already got it/them in the buffer?  The algorithm will be: Count how many
                     // '\n' s we  have got, if this equals nNoofJobs, then we have received all the JobRecs plus the Header rec (and can then process
                     // them all). If it is < nNoofJobs, then keep calling recv until we have them all.
          int result = count(message.begin(), message.end(), '\n') ;
          while(result != nNoofJobs+1){
             numBytes = recv(socketId,charMsg,MAX_RECV_LEN,0);
             if (numBytes == -1) {
                int errorCode = 0;
                string errorMsg = "error calling recv2():\n";
                detectErrorRecv(&errorCode,errorMsg);
                myException socketRecvException(errorCode,errorMsg);
                throw socketRecvException;
               }
             charMsg[numBytes] = 0;
             message += charMsg;
             result = count(message.begin(), message.end(), '\n') ;
            }
//basic_string <char>::iterator strp_Iter;
//for(strp_Iter = message.begin(); strp_Iter < message.end(); strp_Iter++){
          //char szNoofJobsString[500];
          //int idx2PositionOf1stCharInNextRec2bProcessed = 0;
          //int nLengthOfRec;
          //iPosS = message.begin();
          iPosE = message.find_first_of ('\n', 0);
          iPosS = 0;
          for(int eachPacket = 0; eachPacket < nNoofJobs+1; eachPacket++){//Send the Header and the Jobrecs for parsing
             int extBytes = 0; //Extract the bytes between the Command delimiter and the Packet delimiter
             for( ; iPosS <= iPosE; iPosS++)
               charMsg[extBytes++] = message[iPosS];
             charMsg[extBytes] = '\n';
             dlg.MessageCallbackNotify( charMsg, extBytes ); //extBytes is same the length of the message
             if(eachPacket < nNoofJobs){ //for the final packet, we cannot calculate the start & end positions for the next packet, becuase the isn't one
                iPosS = iPosE + 1;
                iPosE = message.find_first_of ('\n', iPosS);
               }
            }
          return;
         }
       else
         return; //There were no Jobs
      }
    //This part deals with all other [single packet] messages received from the server (i.e. NOT JOBS_LIST_REQUESTs)
    while ( bLastCharIsNotNewLine ) {
       numBytes = recv(socketId,charMsg,MAX_RECV_LEN,0);
       if (numBytes == -1) {
          int errorCode = 0;
          string errorMsg = "error calling recv3():\n";
          detectErrorRecv(&errorCode,errorMsg);
          myException socketRecvException(errorCode,errorMsg);
          throw socketRecvException;
         }
       if(charMsg[numBytes - 1] == '\n'){
          bLastCharIsNotNewLine = FALSE;
          charMsg[numBytes - 1] = 0;
          message += charMsg;
          dlg.MessageCallbackNotify( message, message.size() );   //Pass it over for Parsing
         }
       else
         //message.append(string(charMsg));
         message += charMsg;
      }
   }
 catch(myException& excp){
    MessageBox(NULL,"mj myTcpSocket exception occurred",NULL,MB_OK);
    if ( excp.getErrCode() == WSAECONNRESET ) {
        //cout   << "!! your party has shut down the connection... \n";
        //winLog << "!! your party has shut down the connection... \n";
        return;
       }
     excp.response();
     exit(1);
   }
 return;
}
