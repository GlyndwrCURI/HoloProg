#include "stdafx.h"
#include "mySocket.h"

BOOL bReceivingMessageTotalJobs;

char cmd0[] = "JOBS_LIST_REQUEST|\n";
char cmd1[] = "STATUS_REQUEST|\n";
char cmd2[] = "SET_REPETITION_RATE";             
char cmd3[] = "GET_REPETITION_RATE|\n"; 
char cmd4[] = "START_PRINTING"; //|{2AF550CA-6DF8-4CBE-906C-79E95DBD4AB7}*0*0"
char cmd5[] = "STOP_PRINTING|\n";
char *ComandsTable[6] = {cmd0,cmd1,cmd2,cmd3,cmd4,cmd5};

myLog winLog; //winLog is the file syslog.txt
char szCantConnectHelpMes[] = "Does serverConfig.txt exist / contain Display PC's IP address?\nIs network cable plugged in?\nIs Display PC waiting for an incoming call?\nIs a firewall blocking you?\nIs this PC's LAN enabled?";

void readServerConfig(string& serverIPAddr){
 string serverConfigFile = "serverConfig.txt";   
 checkFileExistence(serverConfigFile);
 ifstream serverConfig(serverConfigFile.c_str());
 // read server's IP address
 getline(serverConfig,serverIPAddr);
 serverConfig.close();
}

void checkFileExistence(const string& fileName){
 ifstream file(fileName.c_str());
 if (!file) { 
    ofstream serverConfigF(fileName.c_str());
    serverConfigF << "192.168.0.200";
    serverConfigF.close();
    cout << "WARNING I created a file called " << fileName.c_str() 
         << "and put a default IP address in"<< endl;
    cout << "        it. You may need to edit the address!!" << endl;
    winLog << "WARNING, created a default " << fileName.c_str() << endl;
//    cout << "Cannot continue:" << fileName << " does NOT exist!" << endl;
//    exit(1);
   }
 file.close();
}

//********************************** SendMessToDisplayPC() ****************************//
int SendMessToDisplayPC(HWND hWnd, HWND hCout, int *nRowCount, int nButtonPressedZB){
 winLog  << endl << "Starting SendMessToDisplayPC";
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"Starting SendMessToDisplayPC...");
 (*nRowCount)++;
 SendMessage( hCout, LB_SETTOPINDEX, (*nRowCount) - 5,0);

 myTcpSocket::initialize();   // Initialize the winsock library
 // get client's information (assume neither the name nor the address is given)
 winLog << endl << "Retrieve the localHost [CLIENT] name and address:" << endl;
 myHostInfo clientInfo;
 string clientName = clientInfo.getHostName();
 string clientIPAddress = clientInfo.getHostIPAddress();
 cout << "CPC Name / Address:    " << clientName << " / " << clientIPAddress << endl;
 winLog << "        ==> Name: " << clientName << endl;
 winLog << "        ==> Address: " << clientIPAddress << endl;
 //get server's IP address and name
 //string serverIPAddress = "192.168.0.200";  //Now read from serverConfig.txt
 string serverIPAddress;
 readServerConfig(serverIPAddress);
 winLog << endl;
 winLog << "Retrieve the remoteHost [SERVER] name and address (from serverConfig.txt):" << endl;
 winLog << "        ==> the given address is " << serverIPAddress << endl;

 myHostInfo serverInfo(serverIPAddress,NAME);
 string serverName = serverInfo.getHostName();
 cout << "DisplayPC's Address (configurable by editing serverConfig.txt) currently says:" << endl;
 cout << "DisplayPC's Address: " << serverIPAddress << endl;
 winLog << "        ==> Name: " << serverName << endl;
 winLog << "        ==> Address: " << serverIPAddress << endl;

 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"DisplayPC's Address is configurable by editing serverConfig.txt, currently this says:");
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)serverIPAddress.c_str());
 (*nRowCount) += 2;
 SendMessage( hCout, LB_SETTOPINDEX, (*nRowCount) - 5,0);
 // create the socket for client
 myTcpSocket myClient(DEFAULT_SERVER_PORT); //when myClient is destroyed WSACleanup is called by the destructor 
 // to deregister 'myClient' from the Windows Sockets implementation, and allow the implementation to free any resources.
 cout << myClient;
 winLog << endl << "Client configuration: " << endl;
 winLog << myClient;

 // connect to the server.
 cout   << "Connecting to the server [" << serverName << "]" << "... Next line should say 'Connected'" << endl;
 winLog << "Connecting to the server [" << serverName << "]" << "... Next line should say 'Connected'" << endl;
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"Connecting to the server... Next line should say 'Connected'");
 (*nRowCount)++;
 SendMessage( hCout, LB_SETTOPINDEX, (*nRowCount) - 5,0);
 //Sleep(1000);
 if( myClient.connectToServer(serverIPAddress,NAME) ){
    SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"CANNOT CONNECT!!!!!");
    *nRowCount++;
    MessageBox(hWnd, szCantConnectHelpMes, "Couldn't Connect!", MB_OK);
    // Error is written by connectToServer()
    return 71;
   }

 cout   << "Connected" << endl;
 winLog << "Connected" << endl;
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"Connected");
 (*nRowCount)++;
 SendMessage( hCout, LB_SETTOPINDEX, (*nRowCount) - 5,0);

//   I could Send:                     Replies
//CLIENT_JOBS_LIST_REQUEST             1 + 0 or more
//CLIENT_STATUS_REQUEST                1
//CLIENT_SET_REPETITION_RATE           1?
//CLIENT_GET_REPETITION_RATE           1
//CLIENT_START_PRINTING                ?
//CLIENT_STOP_PRINTING                 ?
//   SEND a message to Server:
 CString strFormattedCommand;
 if(nButtonPressedZB == 0){       //CLIENT_JOBS_LIST_REQUEST
    strFormattedCommand = ComandsTable[nButtonPressedZB];
    bReceivingMessageTotalJobs = 1;
   }
 else if(nButtonPressedZB == 2){      //SET_REPETITION_RATE
    int iRepRate = 1;                        //m_RepRate.GetCurSel();
    iRepRate *= 10;
    iRepRate += 10;
    strFormattedCommand.Format("%s%c%d%c", ComandsTable[nButtonPressedZB], COMMANDS_DELIMITER, iRepRate,PACKET_END_CHARACTER);
   }
 else if(nButtonPressedZB == 4){ //START_PRINTING
     //"START_PRINTING|{2AF550CA-6DF8-4CBE-906C-79E95DBD4AB7}*22*120"
    char szJOB_ID[]    = "{2AF550CA-6DF8-4CBE-906C-79E95DBD4AB7}";
    char szHOLOLINE[]  = "22";
    char szHOLOPIXEL[] = "120";
    strFormattedCommand.Format("%s%c%s*%s*%s%c", ComandsTable[nButtonPressedZB], COMMANDS_DELIMITER, szJOB_ID, szHOLOLINE, szHOLOPIXEL, PACKET_END_CHARACTER);
   }
 else
   strFormattedCommand = ComandsTable[nButtonPressedZB];
 if (send(myClient.getSocketId(), stdstring(strFormattedCommand).c_str(),stdstring(strFormattedCommand).length(),0) == SOCKET_ERROR){
    SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"Something went wrong with Sending CLIENT_STATUS_REQUEST!!!");
    (*nRowCount)++;
    cout << endl << "Something went wrong with Sending CLIENT_STATUS_REQUEST";
    cout << endl << "WSAGetLastError: " << WSAGetLastError() << endl << "<Hit any key to Finish>" << endl;
    return 101;
   }

//******************************* MESSAGE WAS COMPLETELY SENT ****************************//
//***************************** NOW WAIT FOR AN ACKNOWLEDGEMENT **************************//
//receive message from server
 string messageFromServer = "";
//This bit should be the acknowledgement from DisplayPC that all data was received and checksum'd OK
 cout << endl <<"NOW I'M WAITING FOR SOMETHING!" << endl << endl;
 cout   << endl <<"**Reply from DisplayPC:" << endl << endl;
 winLog << endl <<"**Reply from DisplayPC:" << endl << endl;
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)"**Reply from DisplayPC:");
 (*nRowCount) += 2;
 Sleep(150);
 myClient.recieveMessage(messageFromServer);

 cout   <<  messageFromServer << endl;
 winLog <<  messageFromServer << endl;
 SendMessage( hCout, LB_ADDSTRING,0,(DWORD)(LPSTR)messageFromServer.c_str());
 SendMessage( hCout, LB_SETTOPINDEX, (*nRowCount) - 5,0);
 return 0;
}
