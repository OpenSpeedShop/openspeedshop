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

#include "Direct.hxx"

#include <iostream>
#include <sstream>
#include <string>
#include <sys/types.h>

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"

#include "OpenSpeedShopCLI.hxx"

//#define DIRECTDEBUG

using namespace std;
using namespace rapidxml;

string Direct::execute(const string &command)
{
    try {

        // Parse the command
        xml_document<> commandDocument;
        commandDocument.parse<0>(commandDocument.allocate_string(command.c_str()));

        xml_node<> *commandNode = commandDocument.first_node("Command");
        string commandText(commandNode->value());
        string commandType(commandNode->first_attribute("type")->value());
        char *commandID(commandNode->first_attribute("id")->value());

#ifdef DIRECTDEBUG
        cout << __FILE__ << ":" << __LINE__ << "\tRecieved commandText: \"" << commandText << "\""
                                            << " commandType: \""           << commandType << "\""
                                            << " commandID: \""             << commandID   << "\""
                                            << endl;
#endif

        // Build the response document
        xml_document<> responseDocument;
        xml_node<> *responseNode = responseDocument.allocate_node(node_element, "Response");
        responseNode->append_attribute(responseDocument.allocate_attribute("commandID", commandID));
        responseDocument.append_node(responseNode);

        // Deal with a socket server command
        if(commandType == "Server") {
            xml_node<> *serverResponse = responseDocument.allocate_node(node_element, "ServerResponse");

            if(commandText == "version") {
                serverResponse->append_attribute(responseDocument.allocate_attribute("version", "DirectConnection_0.1.dev"));
            } else if(commandText == "exit") {
                //Ignore, there's nothing to do
            }

            responseNode->append_node(serverResponse);

        } else if(commandType == "OpenSpeedShopCLI") {
            commandText += '\n';
            xml_node<> *cliResponse = this->_cli->execute(commandText, &responseDocument);
            responseNode->append_node(cliResponse);
        }

        // Send the result back to the client
#ifdef DIRECTDEBUG
        cout << "responseDocument \"\n" << responseDocument << "\n\"" << endl;
#endif
        ostringstream responseString;
        responseString << responseDocument;

        return responseString.str();

    } catch(...) {
        cerr << __FILE__ << ":" << __LINE__ << "\tError caught" << endl;
    }

    return NULL;
}

Direct::Direct() :
    _cli(new OpenSpeedShopCLI())
{
}

Direct::~Direct()
{
    if(_cli) {
        delete _cli;
    }
}

