/*******************************************************************************
** Copyright (c) 2012-2014 Argo Navis Technologies LLC.  All Rights Reserved.
** Copyright (c) 2014 Krell Institute  All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#include "SocketServer.hxx"

using namespace std;
using namespace rapidxml;

SocketServer::SocketServer(int port, const char *address)
{

  if(address != NULL) {
    cout << __FILE__ << ":" << __LINE__ << "\tStarting server on " << address << ":" << port << endl;
  } else {
    cout << __FILE__ << ":" << __LINE__ << "\tStarting server on 0.0.0.0:" << port << endl;
  }

  if(!_socket.start(port, address)) {
    if(address != NULL) {
      cerr << __FILE__ << ":" << __LINE__ << "\tError code returned from Socket.start(" << port << ", " << address << ")" << endl;
    } else {
      cerr << __FILE__ << ":" << __LINE__ << "\tError code returned from Socket.start(" << port << ", NULL)" << endl;
    }
    return;
  }

  for(;;) {
    Socket clientConnection;
    _socket.accept(clientConnection);

    pid_t childPID = fork();
    if(childPID < 0) {
      cerr << __FILE__ << ":" << __LINE__ << "\tForking error: " << childPID << endl;
      return;
    } else if(childPID > 0) {
      cerr << __FILE__ << ":" << __LINE__ << "\tChild process spawned: " << childPID << endl;
    } else {
      sleep(1); //DEBUG:

      bool exitNow = false;

      string command;
      while(clientConnection.recv(command)) {
        try {
          // Parse the command
          xml_document<> commandDocument;
          commandDocument.parse<0>(commandDocument.allocate_string(command.c_str()));

          xml_node<> *commandNode = commandDocument.first_node("Command");
          string commandText(commandNode->value());
          string commandType(commandNode->first_attribute("type")->value());
          char *commandID(commandNode->first_attribute("id")->value());

          cerr << __FILE__ << ":" << __LINE__ << "\tRecieved commandText: \"" << commandText << "\""
                                              << " commandType: \""           << commandType << "\""
                                              << " commandID: \""             << commandID   << "\"" 
                                              << endl;

          // Build the response document
          xml_document<> responseDocument;
          xml_node<> *responseNode = responseDocument.allocate_node(node_element, "Response");
          responseNode->append_attribute(responseDocument.allocate_attribute("commandID", commandID));
          responseDocument.append_node(responseNode);

          // Deal with a socket server command
          if(commandType == "Server") {
            xml_node<> *serverResponse = responseDocument.allocate_node(node_element, "ServerResponse");

            if(commandText == "version") {
              serverResponse->append_attribute(responseDocument.allocate_attribute("version", "ServerConnection_0.1.dev"));
            } else if(commandText == "exit") {
              exitNow = true;
            }

            responseNode->append_node(serverResponse);

          // Deal with FileSystem requests
          } else if(commandType == "FileSystem") {
            int index = commandText.find_first_of(' ');
            string fileSystemCommand = commandText.substr(0, index);
            string fileSystemArguments = commandText.substr(index+1, commandText.length()-index-1);
            
            cerr << __FILE__ << ":" << __LINE__ << "\tfileSystemCommand: \"" << fileSystemCommand << "\"; fileSystemArguments: \"" << fileSystemArguments << "\"" << endl;
          
            xml_node<> *fileSystemResponse = responseDocument.allocate_node(node_element, "FileSystem");

            if(fileSystemCommand == "dirStat") {
              fileSystemResponse->append_node( _fileSystem.dirStat(fileSystemArguments, &responseDocument) );
            } else if(fileSystemCommand == "catFile") {
              fileSystemResponse->append_node( _fileSystem.catFile(fileSystemArguments, &responseDocument) );
            } else if(fileSystemCommand == "fileExists") {
              fileSystemResponse->append_node( _fileSystem.fileExists(fileSystemArguments, &responseDocument) );
            }
            
            responseNode->append_node(fileSystemResponse);

          // Deal with an OpenSpeedShopCLI command
          } else if(commandType == "OpenSpeedShopCLI") {
            commandText += '\n';
            xml_node<> *cliResponse = _cli.execute(commandText, &responseDocument);
            responseNode->append_node(cliResponse);

          }


          // Send the result back to the client
          cout << "responseDocument \"\n" << responseDocument << "\n\"" << endl;
          ostringstream responseString;
          responseString << responseDocument;
          clientConnection.send(responseString.str());

          if(exitNow) break;

        } catch(...) {
          cerr << __FILE__ << ":" << __LINE__ << "\tError caught" << endl;
        }
      }

      cerr << __FILE__ << ":" << __LINE__ << "\tClosing connection" << endl;
      clientConnection.close();

      cerr << __FILE__ << ":" << __LINE__ << "\tExiting forked child" << endl;
      exit(0);
    }
  }
  _socket.close();
}

SocketServer::~SocketServer()
{
}

