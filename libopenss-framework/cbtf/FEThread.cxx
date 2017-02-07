////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012-15 The Krell Institute. All Rights Reserved.
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

namespace {
    void suspend()
    {
        struct timespec wait;
        wait.tv_sec = 0;
        wait.tv_nsec = 500 * 1000 * 1000;
        while(nanosleep(&wait, &wait));
    }

    bool is_debug_timing_enabled = (getenv("CBTF_TIME_CLIENT_EVENTS") != NULL);
}

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
    default_topology += "/cbtfAutoTopology";
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
#ifndef NDEBUG
    if (is_debug_timing_enabled) {
        std::cerr << Time::Now()
        << " cbtf instrumentor FE thread starts cbtf network."
        << std::endl;
    }
#endif

    std::string xmlfile(collector);
    xmlfile += ".xml";
    registerXML(boost::filesystem::path(CBTF_KRELL_XMLDIR) / xmlfile);


    Component::registerPlugin(
        boost::filesystem::path(CBTF_LIB_DIR) / "KrellInstitute/CBTF/BasicMRNetLaunchers.so");
    
    Component::Instance network = Component::instantiate(
        Type(collector)
        );
    
    Component::Instance launcher = Component::instantiate(
        Type("BasicMRNetLauncherUsingBackendAttach")
        );

    std::map<std::string, Type> outputs = network->getOutputs();

    boost::shared_ptr<ValueSource<unsigned int> > backend_attach_count =
        ValueSource<unsigned int>::instantiate();
    Component::Instance backend_attach_count_component = 
        boost::reinterpret_pointer_cast<Component>(backend_attach_count);
    Component::connect(
        backend_attach_count_component, "value", launcher, "BackendAttachCount"
        );


    boost::shared_ptr<ValueSource<boost::filesystem::path> > backend_attach_file =
        ValueSource<boost::filesystem::path>::instantiate();
    Component::Instance backend_attach_file_component = 
        boost::reinterpret_pointer_cast<Component>(backend_attach_file);
    Component::connect(
        backend_attach_file_component, "value", launcher, "BackendAttachFile"
        );    


    boost::shared_ptr<ValueSource<boost::filesystem::path> > topology_file =
        ValueSource<boost::filesystem::path>::instantiate();
    Component::Instance topology_file_component = 
        boost::reinterpret_pointer_cast<Component>(topology_file);
    Component::connect(
        topology_file_component, "value", launcher, "TopologyFile"
        );
    Component::connect(launcher, "Network", network, "Network");

    boost::shared_ptr<ValueSink<bool> > threads_finished =
					ValueSink<bool>::instantiate();
    Component::Instance threads_finished_output_component =
            boost::reinterpret_pointer_cast<Component>(threads_finished);
    Component::connect(network, "threads_finished",
				threads_finished_output_component, "value");

    boost::shared_ptr<ValueSink<bool> > symbols_finished =
					ValueSink<bool>::instantiate();
    if (outputs.find("symbols_finished") != outputs.end()) {
	Component::Instance symbols_finished_output_component =
            boost::reinterpret_pointer_cast<Component>(symbols_finished);
	Component::connect(network, "symbols_finished",
			symbols_finished_output_component, "value");
    }

    Component::registerPlugin(
        boost::filesystem::path(CBTF_KRELL_LIB_DIR) / "libcbtf-messages-converters-base.so");
    Component::registerPlugin(
        boost::filesystem::path(CBTF_KRELL_LIB_DIR) / "libcbtf-messages-base.so");
    Component::registerPlugin(
        boost::filesystem::path(CBTF_KRELL_LIB_DIR) / "libcbtf-messages-perfdata.so");



    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_CreatedProcess> > > created_process;
    if (outputs.find("created_process_xdr_output") != outputs.end()) {
	created_process =
	SignalAdapter<boost::shared_ptr<CBTF_Protocol_CreatedProcess > >::instantiate();
	Component::Instance created_process_output_component =
			boost::reinterpret_pointer_cast<Component>(created_process);
	created_process->Value.connect(Callbacks::createdProcess);
	Component::connect(network, "created_process_xdr_output",
			created_process_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_AttachedToThreads > > > attached_to_threads;
    Component::Instance attached_to_threads_output_component;
    if (outputs.find("attached_to_threads_xdr_output") != outputs.end()) {
	attached_to_threads =
	SignalAdapter<boost::shared_ptr<CBTF_Protocol_AttachedToThreads > >::instantiate();
	attached_to_threads_output_component =
        		boost::reinterpret_pointer_cast<Component>(attached_to_threads);
	attached_to_threads->Value.connect(Callbacks::attachedToThreads);
	Component::connect(network, "attached_to_threads_xdr_output",
			attached_to_threads_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_Blob > > > perfdata_blob;
    Component::Instance perfdata_blob_output_component;
    if (outputs.find("perfdata_xdr_output") != outputs.end()) {
	perfdata_blob =
	SignalAdapter<boost::shared_ptr<CBTF_Protocol_Blob > >::instantiate();
	perfdata_blob_output_component =
        		boost::reinterpret_pointer_cast<Component>(perfdata_blob);
	perfdata_blob->Value.connect(Callbacks::performanceData);
	Component::connect(network, "perfdata_xdr_output",
			perfdata_blob_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_LoadedLinkedObject > > > loaded_linked_object;
    Component::Instance loaded_linked_object_output_component;
    if (outputs.find("loaded_linkedobject_xdr_output") != outputs.end()) {
	loaded_linked_object =
	SignalAdapter<boost::shared_ptr<CBTF_Protocol_LoadedLinkedObject > >::instantiate();
	loaded_linked_object_output_component =
        		boost::reinterpret_pointer_cast<Component>(loaded_linked_object);
	loaded_linked_object->Value.connect(Callbacks::loadedLinkedObject);
	Component::connect(network, "loaded_linkedobject_xdr_output",
			loaded_linked_object_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_LinkedObjectGroup > > > linked_object_group;
    Component::Instance linked_object_group_output_component;
    if (outputs.find("linkedobjectgroup_xdr_output") != outputs.end()) {
	linked_object_group =
	SignalAdapter<boost::shared_ptr<CBTF_Protocol_LinkedObjectGroup > >::instantiate();
	linked_object_group_output_component =
			boost::reinterpret_pointer_cast<Component>(linked_object_group);
	linked_object_group->Value.connect(Callbacks::linkedObjectGroup);
	Component::connect(network, "linkedobjectgroup_xdr_output",
			linked_object_group_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <LinkedObjectEntryVec > > linkedobjectentryvec_object;
    Component::Instance linkedobjectentryvec_output_component;
    if (outputs.find("linkedobjectentryvec_output") != outputs.end()) {
	linkedobjectentryvec_object =
	SignalAdapter<LinkedObjectEntryVec >::instantiate();
	linkedobjectentryvec_output_component =
			boost::reinterpret_pointer_cast<Component>(linkedobjectentryvec_object);
	linkedobjectentryvec_object->Value.connect(Callbacks::linkedObjectEntryVec);
	Component::connect(network, "linkedobjectentryvec_output",
			linkedobjectentryvec_output_component, "value");
    }

    boost::shared_ptr<SignalAdapter
		     <AddressBuffer > > addressbuffer_object;
    Component::Instance addressbuffer_output_component;
    if (outputs.find("addressbuffer_output") != outputs.end()) {
	addressbuffer_object = SignalAdapter<AddressBuffer >::instantiate();
	addressbuffer_output_component =
			boost::reinterpret_pointer_cast<Component>(addressbuffer_object);
	addressbuffer_object->Value.connect(Callbacks::addressBuffer);
	Component::connect(network, "addressbuffer_output",
			addressbuffer_output_component, "value");
    }

    // Test for the symboltable_xdr_output and connect it if it is
    // defined in the network.
    //
    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_SymbolTable > > > symboltable;
    Component::Instance symboltable_output_component;
    if (outputs.find("symboltable_xdr_output") != outputs.end()) {
	symboltable = SignalAdapter<
	boost::shared_ptr<CBTF_Protocol_SymbolTable > >::instantiate();
        symboltable_output_component =
			boost::reinterpret_pointer_cast<Component>(symboltable);
	symboltable->Value.connect(Callbacks::symbolTable);
	Component::connect(network, "symboltable_xdr_output",
			symboltable_output_component, "value");
    }

    // Test for the maxfunctionvalues and connect if it is
    // defined in the network.
    //
    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_FunctionThreadValues > > > maxfunctionvalues;
    Component::Instance maxfunctionvalues_output_component;
    if (outputs.find("maxfunctionvalues_xdr_out") != outputs.end()) {
	maxfunctionvalues = SignalAdapter<
	boost::shared_ptr<CBTF_Protocol_FunctionThreadValues > >::instantiate();
        maxfunctionvalues_output_component =
			boost::reinterpret_pointer_cast<Component>(maxfunctionvalues);
	maxfunctionvalues->Value.connect(Callbacks::maxFunctionValues);
	Component::connect(network, "maxfunctionvalues_xdr_out",
			maxfunctionvalues_output_component, "value");
    }

    // Test for the minfunctionvalues and connect if it is
    // defined in the network.
    //
    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_FunctionThreadValues > > > minfunctionvalues;
    Component::Instance minfunctionvalues_output_component;
    if (outputs.find("minfunctionvalues_xdr_out") != outputs.end()) {
	minfunctionvalues = SignalAdapter<
	boost::shared_ptr<CBTF_Protocol_FunctionThreadValues > >::instantiate();
        minfunctionvalues_output_component =
			boost::reinterpret_pointer_cast<Component>(minfunctionvalues);
	minfunctionvalues->Value.connect(Callbacks::minFunctionValues);
	Component::connect(network, "minfunctionvalues_xdr_out",
			minfunctionvalues_output_component, "value");
    }

    // Test for the avgfunctionvalues and connect if it is
    // defined in the network.
    //
    boost::shared_ptr<SignalAdapter
		     <boost::shared_ptr
		     <CBTF_Protocol_FunctionAvgValues > > > avgfunctionvalues;
    Component::Instance avgfunctionvalues_output_component;
    if (outputs.find("avgfunctionvalues_xdr_out") != outputs.end()) {
	avgfunctionvalues = SignalAdapter<
	boost::shared_ptr<CBTF_Protocol_FunctionAvgValues > >::instantiate();
        avgfunctionvalues_output_component =
			boost::reinterpret_pointer_cast<Component>(avgfunctionvalues);
	avgfunctionvalues->Value.connect(Callbacks::avgFunctionValues);
	Component::connect(network, "avgfunctionvalues_xdr_out",
			avgfunctionvalues_output_component, "value");
    }


    *backend_attach_count = numBE;
    *backend_attach_file = connections;
    *topology_file = topology;

    // Pass number of backends expected to component network.
    std::map<std::string, Type> inputs = network->getInputs();

    if (inputs.find("numBE") != inputs.end()) {
	boost::shared_ptr<ValueSource<int> > numberBackends =
						ValueSource<int>::instantiate();
	Component::Instance numberBackends_component =
			boost::reinterpret_pointer_cast<Component>(numberBackends);
	Component::connect(numberBackends_component, "value", network, "numBE");
	*numberBackends = numBE;
    }

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
        std::cerr << Time::Now() << " cbtf instrumentor FE cbtf network running."
        << std::endl;
    }
#endif


    // component network emit a threads_finished signal when
    // all ltwt BE's and all threads attached to said backends
    // have terminated.  We can fall out of this thread and
    // join in main to finish up and terminate.
    bool threads_done = false;
    while (true) {
	threads_done = *threads_finished;
	suspend();
	if (threads_done) {
	    finished = true;
	    break;
	}
    }

    // FIXME: The condition above is no sufficient to allow
    // completation of all possble work done in the connections
    // that are made via the SignalAdapters. eg. the min,max,avg
    // messages may not have completed by the time this thread
    // receives the threads_finished notification.  So we may
    // need to send additional notifications. One possible solution
    // is to have the last callback (as of 05-3102015 this would
    // be the avg function values) send a notification that all
    // metrics are done.  But this creates a dependency on the
    // ordering of message traffic out of the collector.xml
    // distributed component networks.
    //
    // This sleep is a workaround to allow the thread running the
    // distributed component network some time to finish handling
    // any remaining message traffic before exiting here.  Without
    // this sleep the mrnet network begins to shutdown to early.
    sleep(3);

#ifndef NDEBUG
    if (is_debug_timing_enabled) {
        std::cerr << Time::Now() << " cbtf instrumentor FE cbtf network finished."
        << std::endl;
    }
#endif
}
