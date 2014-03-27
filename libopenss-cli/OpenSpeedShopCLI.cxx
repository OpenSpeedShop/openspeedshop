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

#include "OpenSpeedShopCLI.hxx"

using namespace std;
using namespace rapidxml;

string &OpenSpeedShopCLI::trim(string &s) {
  s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
  s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
  return s;
}

OpenSpeedShopCLI::OpenSpeedShopCLI()
{
  m_windowID = initializeOSS();
}

OpenSpeedShopCLI::~OpenSpeedShopCLI()
{
  terminateOSS(m_windowID);
}

list<xml_node<> *> OpenSpeedShopCLI::processCommandResults(
    list<CommandResult *> commandResultList,
    memory_pool<> *memoryPool,
    const string &parentName)
{
  list<xml_node<> *> commandResults;

  list<CommandResult *>::iterator commandResultListIterator;
  for(commandResultListIterator = commandResultList.begin();
    commandResultListIterator != commandResultList.end();
    commandResultListIterator++) {

    CommandResult *commandResult = *commandResultListIterator;
    xml_node<> *commandResultNode;
    
    string nodeName(typeid(*commandResult).name());
    int start = nodeName.find("CommandResult_") + 14;
    nodeName = nodeName.substr(start);

    char *nodeNameString = NULL;
    if(parentName.compare("Headers") == 0) {
      // If it's a header string rename it to "Header"
      nodeNameString = memoryPool->allocate_string("Header");

    } else if(parentName.compare("Columns") == 0) {
      //MAGIC: If this is a column child, we have to do some magic to check the column type
      nodeNameString = memoryPool->allocate_string(nodeName.c_str());

      // Ignore empty strings (this is apparently equivalent to a NULL value in the OSS CLI)
      bool ignore = false;
      if(nodeName.compare("String") == 0) {
        ignore = commandResult->Form().empty();
      }

      //TODO: Add any more ignore-case conditions
      
      if(!ignore && _headerNodes.size() > 0 && _headerNodes.size() >= commandResults.size()) {
        xml_node<> *headerNode = _headerNodes.at(commandResults.size());
        xml_attribute<> *headerTypeAttribute = headerNode->first_attribute("columnType");

        if(!headerTypeAttribute) {
          // If the header type doesn't exist create it
          char *attributeValue = memoryPool->allocate_string(nodeName.c_str());
          headerNode->append_attribute(memoryPool->allocate_attribute("columnType", attributeValue));

        } else {
          // If the header type does exist, compare it, and see what we've got
          string headerType(headerTypeAttribute->value());

          // Set the header type to "Mixed" if we don't match the other types
          if(headerType.compare("Mixed") != 0 && headerType.compare(nodeName) != 0) {
            char *attributeValue = memoryPool->allocate_string("Mixed");
            headerTypeAttribute->value(attributeValue);
          }
        }
      } else {
        //TODO: Determine what to do if we have a size mismatch!
      }

    } else {
      nodeNameString = memoryPool->allocate_string(nodeName.c_str());
    }

    commandResultNode = memoryPool->allocate_node(node_element, nodeNameString);

    if(typeid(*commandResult) == typeid(CommandResult_Columns)) {
      // Get a list of the column nodes
      list<CommandResult *> columns;
      ((CommandResult_Columns *)commandResult)->Value(columns);
      list<xml_node<> *> childNodes = processCommandResults(columns, memoryPool, string(commandResultNode->name()));

      // Add them to this node as children
      bool killThisRow = true;
      list<xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        xml_node<> *child = *childNodesListIterator;

        //MAGIC: We need to test if this is a disparate CallStackEntry row
        string childName(child->name());
        if(childName.compare("String") != 0 && childName.compare("CallStackEntry") != 0) {
          // Anything but a String or CallStackEntry cancels the kill
          killThisRow = false;
        } else if(childName.compare("String") == 0) {
          // A string with a non-empty value will also cancel the kill
          string childValue(child->first_attribute("value")->value());
          if(!childValue.empty()) {
            killThisRow = false;
          }
        }

        commandResultNode->append_node(child);
      }

      // If we iterated over the children, and found this row to be a what we're testing for, move on to the next CommandResult
      if(killThisRow) { continue; }

    } else if(typeid(*commandResult) == typeid(CommandResult_Headers)) {
      // Get a list of header nodes
      list<CommandResult *> headers;
      ((CommandResult_Headers *)commandResult)->Value(headers);
      list<xml_node<> *> childNodes = processCommandResults(headers, memoryPool, string(commandResultNode->name()));

      // Add them to this node as children
      list<xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {

        // Save the pointer list locally, so we can add types to the headers later
        _headerNodes.push_back(*childNodesListIterator);
        
        commandResultNode->append_node(*childNodesListIterator);
      }

    } else if(typeid(*commandResult) == typeid(CommandResult_Function)) {
      CommandResult_Function *function = (CommandResult_Function *)commandResult;
      char *attributeValue = memoryPool->allocate_string(function->getName().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("value", attributeValue));

      /* NOTE: This may be the wrong way of going about it
       * See plugins/panels/StatsPanel/StatsPanel.cxx:11927 for more
       * information if we run into problems doing it this way. */
      set<Statement> statements = function->getDefinitions();

      if(!statements.empty()) {
        Statement statement = *statements.begin();  // There should only be one!

        attributeValue = memoryPool->allocate_string(statement.getPath().c_str());
        commandResultNode->append_attribute(memoryPool->allocate_attribute("path", attributeValue));

        ostringstream stringStream;
        stringStream << statement.getLine();
        attributeValue = memoryPool->allocate_string(stringStream.str().c_str());
        commandResultNode->append_attribute(memoryPool->allocate_attribute("line", attributeValue));
      }

    } else if(typeid(*commandResult) == typeid(CommandResult_Statement)) {
      CommandResult_Statement *statement = (CommandResult_Statement *)commandResult;

      char *attributeValue = memoryPool->allocate_string(statement->getPath().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("path", attributeValue));

      ostringstream stringStream;
      stringStream << statement->getLine();
      attributeValue = memoryPool->allocate_string(stringStream.str().c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("line", attributeValue));

    } else if(typeid(*commandResult) == typeid(CommandResult_CallStackEntry)) {
      // Get a list of the call stack entries
      CommandResult_CallStackEntry *callStackEntry = (CommandResult_CallStackEntry *)commandResult;
      vector<CommandResult *> *callStackEntryVector = callStackEntry->Value();
      list<CommandResult *> callStackEntryList(callStackEntryVector->rbegin(), callStackEntryVector->rend());
      list<xml_node<> *> childNodes = processCommandResults(callStackEntryList, memoryPool);

      /* //NOTE: It looks like the CallStackEntries aren't created as expected.  We get both a row-based hierarchy, and a 
                 "building" stack object.  We're looking for only the "full-stack" entry, and then removing the previous 
                 partial-stack rows, based on the count.  See the CommandResult_Columns elseif for more details on the 
                 culling process.
       */

      // Add them to this node as children
      list<xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        commandResultNode->append_node(*childNodesListIterator);
      }
      
    } else {
      string value = commandResult->Form();
      char *attributeValue = memoryPool->allocate_string(trim(value).c_str());
      commandResultNode->append_attribute(memoryPool->allocate_attribute("value", attributeValue));

    }

    commandResults.push_back(commandResultNode);
  }

  return commandResults;
}

InputLineObject *OpenSpeedShopCLI::getInputLineObject(int windowID, const char *command)
{
  static int sequence = 0;

  InputLineObject *inputLineObject = new InputLineObject(windowID, string(command));
  inputLineObject->SetSeq(++sequence);
  inputLineObject->SetStatus(ILO_IN_PARSER);

  ParseResult *parseResult = new ParseResult();
  p_parse_result = parseResult;     //GLOBAL: YACC methods require this

  if(!(yyin = tmpfile())) {         //GLOBAL: YACC methods require this
    throw ServerException("Failed to open temporary file");
  }

  fprintf(yyin, command);
  rewind(yyin);

  if(yyparse()) {
    fclose(yyin);                   // Can't do anything about errors here
    throw ServerException("Failed to parse with YACC");
  }

  if(fclose(yyin)) {
    throw ServerException("Failed to close temporary file");
  }

  if(parseResult->syntaxError()) {
    throw ServerException("Syntax error");
  }

  Current_ILO = inputLineObject;    //GLOBAL: CommandObject constructor requires this
  CommandObject *commandObject = new CommandObject(parseResult, true);

  if (parseResult->getCommandType() == CMD_HEAD_ERROR) {
    throw ServerException("Error in CommandObject");
  }

  SS_Execute_Cmd(commandObject);

  while(!inputLineObject->Semantics_Complete()) {
    //TODO: Add methods for timeout and canceling the operation

    if(inputLineObject->What() == ILO_ERROR) {
      if(inputLineObject) inputLineObject->Set_Results_Used();
      throw ServerException("Error in inputLineObject processing");
    }

    sleep(1);
  }

  return commandObject->Clip();
}


xml_node<> *OpenSpeedShopCLI::execute(string command, memory_pool<> *memoryPool)
{
  xml_node<> *cliResponseNode =  memoryPool->allocate_node(node_element, memoryPool->allocate_string("OpenSpeedShopCLI"));

  try {
    InputLineObject *inputLineObject = getInputLineObject(m_windowID, command.c_str());

    /* It appears that there is always only one CommandObject returned, even though
     * it is in a list.  We'll process it properly, just in case: bits are cheap */
    list<CommandObject *> commandObjectList = inputLineObject->CmdObj_List();
    list<CommandObject *>::iterator commandObjectListIterator;
    for(commandObjectListIterator = commandObjectList.begin();
      commandObjectListIterator != commandObjectList.end();
      commandObjectListIterator++) {

      xml_node<> *commandObjectNode = memoryPool->allocate_node(node_element, memoryPool->allocate_string("CommandObject"));

      list<xml_node<> *> childNodes = processCommandResults((*commandObjectListIterator)->Result_List(), memoryPool);

      _headerNodes.clear();   // Wipe the header cache

      list<xml_node<> *>::iterator childNodesListIterator;
      for(childNodesListIterator = childNodes.begin();
          childNodesListIterator != childNodes.end();
          childNodesListIterator++) {
        commandObjectNode->append_node(*childNodesListIterator);
      }

      cliResponseNode->append_node(commandObjectNode);
    }

    if(inputLineObject) {
      delete inputLineObject;
      Current_ILO = inputLineObject = NULL;
    }

  } catch (exception &error) {
    char *errorWhat = memoryPool->allocate_string(error.what());
    cliResponseNode->append_node(memoryPool->allocate_node(node_element, "Exception", errorWhat));
  } catch (string error) {
    char *errorWhat = memoryPool->allocate_string(error.c_str());
    cliResponseNode->append_node(memoryPool->allocate_node(node_element, "Exception", errorWhat));
  } catch(...) {
    cliResponseNode->append_node(memoryPool->allocate_node(node_element, "Exception", "Unknown"));
  }

  return cliResponseNode;
}


// Keeps count of the "windows" that we have open (used in initialize and terminate)
int OpenSpeedShopCLI::m_windowCount = 0;

int OpenSpeedShopCLI::initializeOSS()
{
  if(!m_windowCount++) {
    Openss_Basic_Initialization();
  }

  pid_t pid = getpid();
  char hostName[256];
  gethostname(&*hostName,255);
  int windowID = Embedded_Window("SocketServerInterface", &*hostName, pid, 0, false);

  pcli_load_scripting_messages();

  return windowID;
}

void OpenSpeedShopCLI::terminateOSS(int windowID)
{
  /* For some reason this is already called when main() returns. Though everywhere I look in
   * Open|SpeedShop's code, these are called.  Maybe this only applies for Embedded_Windows? */

//  Window_Termination(windowID);  // Probably not necessary I think all of the windows are destroyed in Commander_Termination()

//  if(!--m_windowCount) {
//    Openss_Basic_Termination();
//  }

}

