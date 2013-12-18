////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-13 The Krell Institute. All Rights Reserved.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
////////////////////////////////////////////////////////////////////////////////

/** @file
 *
 * Definition of the cbtf Instrumentor namespace.
 *
 */

#include "FEThread.hxx"
#include "Callbacks.hxx"

#include <KrellInstitute/CBTF/BoostExts.hpp>
#include <KrellInstitute/CBTF/Component.hpp>
#include <KrellInstitute/CBTF/SignalAdapter.hpp>
#include <KrellInstitute/CBTF/Type.hpp>
#include <KrellInstitute/CBTF/ValueSink.hpp>
#include <KrellInstitute/CBTF/ValueSource.hpp>
#include <KrellInstitute/CBTF/XML.hpp>
#include <KrellInstitute/Core/AddressBuffer.hpp>
#include <KrellInstitute/Core/LinkedObjectEntry.hpp>
#include <boost/thread.hpp>

using namespace OpenSpeedShop::Framework;
using namespace KrellInstitute::CBTF;
using namespace KrellInstitute::Core;

/**
 * main thread for the example collection tool.
 */


FEThread::FEThread()
{
}

void FEThread::start( const std::string& collector, const unsigned int& numBE,
	     bool& finished)
{

    // default to users current directory.
    // do not create the .cbtf directory as we did when using $HOME
    char const* cur_dir = getenv("PWD");
    
    // create a default for topology file.
    std::string default_topology(cur_dir), topology;
    default_topology += "/cbtf_topology";
    topology = default_topology;
    //
    // create a default for connections file.
    std::string default_connections(cur_dir),connections;
    default_connections += "/attachBE_connections";

    connections = default_connections;

    dm_thread = boost::thread(&FEThread::run, this, topology, connections,
			      collector, numBE, finished);
}

void FEThread::start( const std::string& topology, const std::string& connections,
		      const std::string& collector, const unsigned int& numBE,
		      bool& finished)
{
    dm_thread = boost::thread(&FEThread::run, this, topology, connections,
			      collector, numBE, finished);
}

void FEThread::join()
{
    dm_thread.join();
}

void FEThread::run(const std::string& topology, const std::string& connections,
	   const std::string& collector, const unsigned int& numBE,
	   bool& finished)
  {
    //std::cerr << "FEThread::run for collector " << collector << std::endl;
    std::string xmlfile(collector);
    xmlfile += ".xml";
    registerXML(filesystem::path(CBTF_TOOLS_XMLDIR) / xmlfile);


    Component::registerPlugin(
        filesystem::path(CBTF_LIB_DIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so");
    
    Component::Instance network = Component::instantiate(
        Type(collector)
        );
    
    Component::Instance launcher = Component::instantiate(
        Type("BasicMRNetLauncherUsingBackendAttach")
        );

    shared_ptr<ValueSource<unsigned int> > backend_attach_count =
        ValueSource<unsigned int>::instantiate();
    Component::Instance backend_attach_count_component = 
        reinterpret_pointer_cast<Component>(backend_attach_count);
    Component::connect(
        backend_attach_count_component, "value", launcher, "BackendAttachCount"
        );


    shared_ptr<ValueSource<filesystem::path> > backend_attach_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance backend_attach_file_component = 
        reinterpret_pointer_cast<Component>(backend_attach_file);
    Component::connect(
        backend_attach_file_component, "value", launcher, "BackendAttachFile"
        );    


    shared_ptr<ValueSource<filesystem::path> > topology_file =
        ValueSource<filesystem::path>::instantiate();
    Component::Instance topology_file_component = 
        reinterpret_pointer_cast<Component>(topology_file);
    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );
    Component::connect(launcher, "Network", network, "Network");

    shared_ptr<ValueSink<bool> > done = ValueSink<bool>::instantiate();
    Component::Instance done_output_component =
            reinterpret_pointer_cast<Component>(done);
    Component::connect(network, "output", done_output_component, "value");

    Component::registerPlugin(
        filesystem::path(CBTF_LIB_DIR) / "libcbtf-messages-converters-base.so");
    Component::registerPlugin(
        filesystem::path(CBTF_LIB_DIR) / "libcbtf-messages-base.so");

    std::map<std::string, Type> outputs = network->getOutputs();

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_CreatedProcess> > > created_process;
    if (outputs.find("created_process_xdr_output") != outputs.end()) {
    created_process = SignalAdapter<boost::shared_ptr<CBTF_Protocol_CreatedProcess > >::instantiate();
    Component::Instance created_process_output_component =
            reinterpret_pointer_cast<Component>(created_process);
    created_process->Value.connect(Callbacks::createdProcess);
    Component::connect(network, "created_process_xdr_output",
		      created_process_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_AttachedToThreads > > > attached_to_threads =
        SignalAdapter<boost::shared_ptr<CBTF_Protocol_AttachedToThreads > >::instantiate();
    Component::Instance attached_to_threads_output_component =
            reinterpret_pointer_cast<Component>(attached_to_threads);
    attached_to_threads->Value.connect(Callbacks::attachedToThreads);
    Component::connect(network, "attached_to_threads_xdr_output",
		      attached_to_threads_output_component, "value");

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_Blob > > > perfdata_blob =
        SignalAdapter<boost::shared_ptr<CBTF_Protocol_Blob > >::instantiate();
    Component::Instance perfdata_blob_output_component =
            reinterpret_pointer_cast<Component>(perfdata_blob);
    perfdata_blob->Value.connect(Callbacks::performanceData);
    Component::connect(network, "perfdata_xdr_output",
		      perfdata_blob_output_component, "value");

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_LoadedLinkedObject > > > loaded_linked_object;
    if (outputs.find("loaded_linkedobject_xdr_output") != outputs.end()) {
    loaded_linked_object = SignalAdapter<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject > >::instantiate();
    Component::Instance loaded_linked_object_output_component =
            reinterpret_pointer_cast<Component>(loaded_linked_object);
    loaded_linked_object->Value.connect(Callbacks::loadedLinkedObject);
    Component::connect(network, "loaded_linkedobject_xdr_output",
		      loaded_linked_object_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_LinkedObjectGroup > > > linked_object_group =
        SignalAdapter<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup > >::instantiate();
    Component::Instance linked_object_group_output_component =
            reinterpret_pointer_cast<Component>(linked_object_group);
    linked_object_group->Value.connect(Callbacks::linkedObjectGroup);
    Component::connect(network, "linkedobjectgroup_xdr_output",
		      linked_object_group_output_component, "value");

    boost::shared_ptr<SignalAdapter
		     <LinkedObjectEntryVec > > linkedobjectentryvec_object =
        SignalAdapter<LinkedObjectEntryVec >::instantiate();
    Component::Instance linkedobjectentryvec_output_component =
            reinterpret_pointer_cast<Component>(linkedobjectentryvec_object);
    linkedobjectentryvec_object->Value.connect(Callbacks::linkedObjectEntryVec);
    Component::connect(network, "linkedobjectentryvec_output",
		      linkedobjectentryvec_output_component, "value");

    boost::shared_ptr<SignalAdapter
		     <AddressBuffer > > addressbuffer_object =
        SignalAdapter<AddressBuffer >::instantiate();
    Component::Instance addressbuffer_output_component =
            reinterpret_pointer_cast<Component>(addressbuffer_object);
    addressbuffer_object->Value.connect(Callbacks::addressBuffer);
    Component::connect(network, "addressbuffer_output",
		      addressbuffer_output_component, "value");

    // Test for the symboltable_xdr_output and connect it if it is
    // defined in the network. At this time only the CUDA collector
    // has an active symboltable_xdr_output.
    //
    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_SymbolTable > > > symboltable;
    if (outputs.find("symboltable_xdr_output") != outputs.end()) {
	symboltable = SignalAdapter<
	boost::shared_ptr<CBTF_Protocol_SymbolTable > >::instantiate();
        Component::Instance symboltable_output_component =
            reinterpret_pointer_cast<Component>(symboltable);
        symboltable->Value.connect(Callbacks::symbolTable);
        Component::connect(network, "symboltable_xdr_output",
		      symboltable_output_component, "value");
    }

    *backend_attach_count = numBE;
    *backend_attach_file = connections;
    *topology_file = topology;


    int i;
    while (true) {

	finished = *done;
	if (finished) break;
    }
}
