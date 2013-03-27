# -*- python -*-
#                           Package   : omniidl
# zeromq.py                 Created on: 2013/02/24
#                           Author    : Anne C. van Rossum
#
#    Copyright (C) 2013 Almende B.V.
#
#  This file is part of omniidl.
#
#  omniidl is free software; you can redistribute it and/or modify it
#  under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
#  02111-1307, USA.
#
# Description:
#   
#   ZeroMQ back-end for network abstractions 

"""ZeroMQ IDL compiler back-end."""

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

class ZeroMQVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    # The user is able to define its own data structures to be used on the channels 
    structList = []
    
    # Every method corresponds to a port/channel
    portList = []
    
    # Typedef list
    typedefList = []

    # Typedef list
    pragmaList = []
    
    # The module's name
    classname = ''
    
    # The namespace
    namespace = ''
     
    def __init__(self, st):
        self.st = st

##########################################################################################
# General functions
##########################################################################################

    def getPortValueName(self, node, param):
	portname = "port" + node.identifier()
        if param.paramType().kind() == 3:
		portValueName = portname + "Value";
        elif param.paramType().kind() == 21:
		portValueName = portname + "Values";
	else:
		portValueName = portname + "Value";
	return portValueName; 

    def getPortValue(self, param):
        if param.paramType().kind() == 3:
		portValue = "int";
        elif param.paramType().kind() == 21:
                param.paramType().accept(self)
                param_type = self.__result_type
                self.getSeqType(param_type)
                seq_type = self.__result_type                   
		portValue = "std::vector<" + seq_type + ">";
	else:
		portValue = "Unknown";
	return portValue; 

##########################################################################################
# Write functions
##########################################################################################
  
    def writeAll(self):
        self.writeFileComment()
        self.writeIncludes()
        self.writeNamespaceStart()
	self.writeStructs()
        for s in self.structList:
           self.writeStructDeclaration(s)
        self.writeTypedefs()
        self.writeInterface()
        self.writeNamespaceEnd()
        
    def writeFileComment(self):
        comment_text = '''/**
 * 456789------------------------------------------------------------------------------------------------------------120
 *
 * @brief Artificial Intelligence Module
 * @file ...
 * 
 * This file is created at Almende B.V. and Distributed Organisms B.V. It is open-source software and belongs to a
 * larger suite of software that is meant for research on self-organization principles and multi-agent systems where
 * learning algorithms are an important aspect.
 *
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless, we personally strongly object
 * against this software being used for military purposes, factory farming, animal experimentation, and "Universal
 * Declaration of Human Rights" violations.
 *
 * Copyright (c) 2013 Your Name <your@email.address>
 *
 * @author    ...
 * @date      Oct 24, 2012
 * @project   ...
 * @company   ...
 * @company   Distributed Organisms B.V.
 * @case      Artificial Intelligence Module
 */


/***********************************************************************************************************************
 * Remarks
 **********************************************************************************************************************/

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that... and install zmq from source
// see: http://zguide.zeromq.org/cpp:hwserver and http://zguide.zeromq.org/cpp:hwclient
// see: http://zguide.zeromq.org/c:hwserver

/***********************************************************************************************************************
 * Interface
 **********************************************************************************************************************/
'''
        print comment_text

    def writeIncludes(self):
        print "// general C/C++ headers"
        print "#include <vector>"
        print "#include <string>"
        print "#include <sstream>"
        print "#include <unistd.h>" # for getpid()
        print
        print "// middleware specific headers"
        print "#include <zmq.hpp>"
        print "#include <json_spirit_reader.h>"
	print
#        print "// namespaces and typedefs"
#        print       

    def writeStructs(self):
        print '/**'
        print ' * Port name service record. This is like a domain name service record, but instead of containing an IP address and an'
        print ' * URI, it comes with a "name" that can be resolved as a "host", "port", and "pid". The name is something like "/write",'
        print ' * the host something like "127.0.0.1" or "dev.almende.com" (that is resolvable by dns), "port" is a TCP/UDP port, and'
        print ' * "pid" is the process identifier.'
        print ' */'
        print 'typedef struct psn_record_t {'
        print '	std::string name;'
        print '	std::string host;'
        print '	std::string port;'
        print '	std::string pid;'
        print '} pns_record;'
        print
        print '// Following structure makes it easier to store state information per socket'
        print 'typedef struct zmq_socket_ext_t {'
        print '	zmq::socket_t *sock;'
        print '	std::string name;'
        print '	bool ready;'
        print '} zmq_socket_ext;'
        print

    def writeNamespaceStart(self):
        print "// recommended namespace: \"rur\""
        print "// do not forget to add \"using namespace rur;\" to your .cpp file"
        print "namespace " + self.namespace + " {"
        print

    def writeNamespaceEnd(self):
        print "}"
        print

    def writePragmas(self):        
        for t in self.typedefList:
            for d in t.declarators():
                a = t.aliasType()
                if isinstance(a,idltype.Sequence):
                    self.visitSequenceTypeToVector(a)  
                    seq_type = self.__result_type
                    self.st.out("typedef " + seq_type + " " + d.identifier() + ";")
                self.st.out("")

    # Only accept typedef for sequences for now
    def writeTypedefs(self):        
        for t in self.typedefList:
            for d in t.declarators():
                a = t.aliasType()
                if isinstance(a,idltype.Sequence):
                    self.visitSequenceTypeToVector(a)  
                    seq_type = self.__result_type
                    self.st.out("typedef " + seq_type + " " + d.identifier() + ";")
                self.st.out("")

    def writeInterface(self):
        comment_text = '''/**
 * The generated class. Do not modify or add class members. Either derive from this class and implement Tick() or use a 
 * separate helper class to store state information. All information for the operation of the module should be obtained 
 * over the defined ports.
 */
'''
        print comment_text
        self.writeClassStart()
        self.st.inc_indent()
        
        for m in self.portList:
            self.writePortDeclaration(m)
            
        # Create blob with private dummy instances
        if self.structList != []:        
            self.st.out("")
            self.st.out("// User-defined structs (automatically allocated later)")
            for s in self.structList:
                 self.writeStructInstance(s)
        
        # end of private section    
        self.st.dec_indent()
        
        # begin of public section
        self.st.out("")
        self.st.out("public:")
        self.st.inc_indent()
        
        self.writeConstructor()
        self.st.out("")
        self.writeDestructor()
        self.st.out("")
        self.writeTick()
        self.st.out("")
        self.writeInit()
        self.st.out("")
        self.writeClose()
        self.st.out("")

        self.writeStructParam()

        self.st.dec_indent()
        self.st.out("protected:")
        self.st.inc_indent()
        
	self.st.out( "// the standard zeromq context object")
	self.st.out( "zmq::context_t *context;")
        self.st.out( "// the user-defined id of a module")
        self.st.out( "std::string module_id;")
        self.st.out( "// some default debug parameter")
        self.st.out( "char debug;")

        self.st.out("// All subsequent functions should be called from \"within\" this module" )
        self.st.out("// From either the Tick() routine itself, or Tick() in a derived class" )
        self.st.out("")
        self.writePorts()

        self.writeResolve()
        
        self.st.dec_indent()
        self.writeClassEnd()

    # Declare internal dummy instances for custom structs
    # we define an exception for the struct Param that can be used to store command-line parameters
    def writeStructInstance(self, node): 
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out( name + ' *cli' + name + ";" )
        else:
            self.st.out( name + ' *dummy' + name + ";" )
    
    # The struct definition itself 
    def writeStructDeclaration(self, node):
        self.st.out( "struct " + node.identifier() + " {" )

        self.st.inc_indent()

        for m in node.members():
            if m.constrType():
                m.memberType().decl().accept(self)

            m.memberType().accept(self)
            type = self.__result_type
            for d in m.declarators():
                membername = d.identifier()
                self.st.out(type + " " + membername + ";")

        self.st.dec_indent()
        self.st.out("};")
        self.st.out("")

    # Allocate structs
    # we define an exception for the struct Param that can be used to store command-line parameters    
    def writeStructAllocation(self, node):
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out( 'cli' + name + " = new " + name + "();" )
        else:
            self.st.out( 'dummy' + name + " = new " + name + "();" )

    # Dedicate setter and getter for Param struct
    def writeStructParam(self):
    	for s in self.structList:
    		name = s.scopedName()[-1]
    		if name == 'Param':
    			self.st.out('// Function to get Param struct (to subsequently set CLI parameters)')
    			self.st.out('inline Param *GetParam() { return cliParam; };')
    			self.st.out("")
        
    def writeClassStart(self):
        self.st.out( "class " + self.classname + " {" )
        self.st.out( "private:" )
        self.st.inc_indent()
	self.st.out( "// the socket over which is communicated with the name server")
	self.st.out( "zmq::socket_t *ns_socket;")
	self.st.out( "// standard control socket over which commands arrive to connect to some port for example")
	self.st.out( "zmq::socket_t *cmd_socket;")
	self.st.out( "// standard control socket over which commands arrive to connect to some port for example")
	self.st.out( "std::vector<zmq_socket_ext*> zmq_sockets;")
        self.st.dec_indent()
        
    def writeClassEnd(self):
        self.st.out( "};" )

    # Write constructors plus comments                
    def writeConstructor(self):
        self.st.out( "// The constructor needs to be called, also when you derive from this class" )
        self.st.out( self.classname + "(): ns_socket(NULL), cmd_socket(NULL) {" )
        self.st.inc_indent()
        self.st.out( "context = new zmq::context_t(1);")
	self.st.out( "debug = 0;")
        # We allocate all user-defined structs
        for s in self.structList:
            self.writeStructAllocation(s)
        
        # We allocate all YARP ports
        for m in self.portList:
            self.writePortAllocation(m)
        self.st.dec_indent()
        self.st.out( "}" )

    def writeDestructor(self):
        self.st.out( "~" + self.classname + "() {" )
        self.st.inc_indent()
        for m in self.portList:
            self.writePortDestruction(m)
#        self.st.out("delete cliParam;" ) 
        self.st.dec_indent()
        self.st.out( "}" )   

    def writeTick(self):
        self.st.out( "// This is the function you will need to implement. Do not forget to call this parent function." )
        self.st.out( "void Tick() {")
        self.st.inc_indent()
        self.st.out("HandleCommand();")
#       self.st.out("")
        self.st.dec_indent()
        self.st.out("}")
        self.st.out("")
        self.st.out( "bool Stop(); ")
#      self.st.out("")
        
    def writeInit(self):
        self.st.out( "// After construction you will need to call this function first. It binds all the sockets." )
        self.st.out( "void Init(std::string module_id) {" )
        self.st.inc_indent()
        self.st.out( "this->module_id = module_id;" )
        self.st.out("")	
        self.st.out( "std::cout << \"Connecting to name server...\" << std::endl;")
        self.st.out( "ns_socket = new zmq::socket_t(*context, ZMQ_REQ);")
        self.st.out("try {")
        self.st.out( "  ns_socket->connect(\"tcp://127.0.0.1:10101\"); // port to connect to, REQ/REP")
        self.st.out("} catch (zmq::error_t & e) {")	
        self.st.out("   std::cerr << \"Error: Could not connect to the name server: = \" << e.what() << std::endl;")	
        self.st.out("}")	
        self.st.out( "cmd_socket = new zmq::socket_t(*context, ZMQ_REP);")
        self.st.out("")	
        self.st.out( "{")
        self.st.inc_indent()
        self.st.out( "pns_record record;")
        self.st.out( "std::stringstream ss; ss.clear(); ss.str(\"\");" )
        self.st.out( "ss << getpid();" )
        self.st.out( "record.name = \"/resolve/\" + ss.str() + \"/control\";")
        self.st.out( "record.pid = ss.str();" )
        self.st.out( "Resolve(record);" )
        self.st.out( "ss.clear(); ss.str(\"\");" )
        self.st.out( "ss << \"tcp://\" << record.host << \":\" << record.port; ")
        self.st.out( "std::string sock = ss.str(); ")
        self.st.out( "std::cout << \"Bind to socket \" << sock << std::endl; ")
        self.st.out( "cmd_socket->bind(sock.c_str());")
        self.st.dec_indent()
        self.st.out( "}")
        self.st.out("")	
        for p in self.portList:
            self.writePortInit(p)
        self.st.dec_indent()
        self.st.out( "}" )   

    def writeClose(self):
        self.st.out( "// Before destruction you will need to call this function first. It closes the zeromq sockets." )
        self.st.out( "void Close() {" )
        self.st.inc_indent()
        self.st.out( "ns_socket->close();" )	
        self.st.out( "cmd_socket->close();" )	
        for p in self.portList:
            self.writePortClose(p)
        self.st.dec_indent()
        self.st.out( "}" )

    def writeResolve(self):
        function_body = '''  /**
   * The resolve function can be called by modules to get a new socket (and if you want host name and port). It can also
   * be used by the connector, to bind to these previously set up sockets.
   */
  void Resolve(pns_record & record) {
	std::cout << "Acquire TCP/IP port for " << record.name << std::endl;
	std::string reqname = record.name + ':' + record.pid;
	zmq::message_t request (reqname.size() + 1);
	memcpy ((void *) request.data (), reqname.c_str(), reqname.size());
	ns_socket->send(request);

	zmq::message_t reply;
	if (!ns_socket->recv (&reply)) return;
	size_t msg_size = reply.size();
	char* address = new char[msg_size+1];
	memcpy (address, (void *) reply.data(), msg_size);
	address[msg_size] = \'\\0\';
	std::string json = std::string(address);
	std::cout << "Received " << json << std::endl;
	delete [] address;

	// get json object
	bool valid;
	json_spirit::Value value;
	if (!json_spirit::read(json, value)) {
		valid = false;
		std::cerr << "Not a json value" << std::endl;
		return;
	}
	if (value.type() != json_spirit::obj_type) {
		std::cerr << "[1] Unexpected object type \"" << Value_type_str[value.type()] << "\"";
		std::cerr << " instead of \"" << Value_type_str[json_spirit::obj_type] << "\"" << std::endl;
		return;
	}

	// a "json_spirit object" is - by default - a vector of json pairs
	json_spirit::Object obj = value.get_obj();

	for( json_spirit::Object::size_type i = 0; i != obj.size(); ++i ) {
		const json_spirit::Pair& pair = obj[i];
		const std::string& key = pair.name_;
		const json_spirit::Value& value = pair.value_;

		if (key == "identifier") {
			// same thing
		} else if (key == "server") {
			record.host = value.get_str();
		} else if (key == "port") {
			record.port = value.get_str();
		} else if (key == "pid") {
			record.pid = value.get_str();
		}
	}
  }

  void SendAck(zmq::socket_t *s, bool state) {
	std::cout << "Send ACK" << std::endl;
	SendRequest(s, state, true, "ACK");
  }

  bool ReceiveAck(zmq::socket_t *s, bool & state, bool blocking) {
	int reply_size = 0;
	char *reply = GetReply(s, state, blocking, reply_size);
	if (reply == NULL) return false;
	std::string req = std::string(reply);
	delete [] reply;
	if (req.find("ACK") != std::string::npos) {
		if (debug) std::cout << "Got ACK, thanks!" << std::endl;
		return true;
	}
	std::cerr << "Error: got \\"" << req << "\\", no ACK, state compromised" << std::endl;
	return false;
  }

  char* GetReply(zmq::socket_t *s, bool & state, bool blocking, int & reply_size) {
	zmq::message_t reply;
	char* result = NULL;
	reply_size = 0;
	try {
		if (blocking)
			state = s->recv(&reply);
		else
			state = s->recv(&reply, ZMQ_DONTWAIT);
	} catch (zmq::error_t &e) {
		std::cout << "Error: received zmq::error_t " << e.what() << std::endl;
	}
	if (state) {
		size_t msg_size = reply.size();
		result = new char[msg_size+1];
		memcpy (result, (void *) reply.data(), msg_size);
		result[msg_size] = \'\\0\';
		reply_size = msg_size;
		//std::cout << "Result: \\"" << std::string(result) << "\\"" << std::endl;
	}
	return result;
  }

  void SendRequest(zmq::socket_t *s, bool & state, bool blocking, std::string str) {
	if (state) {
		zmq::message_t request(str.size()+1);
		memcpy((void *) request.data(), str.c_str(), str.size());
		if (debug) std::cout << "Send request: " << str << std::endl;
		if (blocking)
			state = s->send(request);
		else
			state = s->send(request, ZMQ_DONTWAIT);
	} else {
		std::cout << "Send nothing (still waiting to receive) " << std::endl;
	}
  }

  void HandleCommand() {
    int reply_size = -1;
    bool state = false;
    char *reply = GetReply(cmd_socket, state, false, reply_size);
    if (reply == NULL) return;
    if (reply_size < 2) std::cerr << "Error: Reply is not large for magic header + command string" << std::endl;
    char magic_value = reply[0];
    reply[reply_size-1] = \'\\0\';
    if (magic_value == 0x01) { // connect to command...
        std::string name = std::string(reply+1);
        int pos = name.find("->");
        if (pos == std::string::npos) {
           std::cerr << "Error: no -> separator in connect command" << std::endl;
        }
        std::string source = name.substr(0, pos);
        std::string target = name.substr(pos+2); // todo: 
        std::cout << "Connect from " << source << " to " << target << std::endl;
    	Connect(source, target);
    } else {
        std::cerr << "Error: Unknown command!" << std::endl;
    }
    SendAck(cmd_socket, true);
    delete [] reply;
  }

  void Connect(std::string source, std::string target) {
    zmq::socket_t *s = GetSocket(source);
    pns_record t_record;
    t_record.name = "/resolve" + target;
    Resolve(t_record);
    std::stringstream ss; ss.clear(); ss.str("");
    ss << "tcp://" << t_record.host << ":" << t_record.port; 
    std::string sock = ss.str(); 
    std::cout << "Connect to socket " << sock << std::endl; 
    try {
        s->connect(sock.c_str());
    } catch (zmq::error_t &e) {
        std::cerr << "Error: Could not connect to " << target << ", because: " << e.what() << std::endl;
    }
  }

  zmq::socket_t* GetSocket(std::string name) {
	for (int i = 0; i < zmq_sockets.size(); ++i) {
		if (zmq_sockets[i]->name.find(name) != std::string::npos) return zmq_sockets[i]->sock;
	}
	std::cerr << "Error: socket name could not be found!" << std::endl;
	return NULL;
	//assert(false); // todo, get the previously registered socket by name
  }
'''
	print function_body

    # For every port we define what the port name will be. We used to have an interator to
    # enable multiple ports with incrementing indices, but this does make things less 
    # transparent to the user. So, if you have two microphones, you will need to create two
    # functions: 
    #   void MicrophoneLeft(in long mic_left)
    #   void MicrophoneRight(in long mic_right)
    # The ports and values are private to the class.
    def writePortDeclaration(self, node):
        self.st.out("")
        for p in node.parameters():
            param_name = p.identifier()
            self.__prefix = ""
            p.paramType().accept(self)
            param_type = self.__result_type
            portname = "port" + node.identifier()
            if p.is_in():
            	portValue = self.getPortValue(p)
            	portValueName = self.getPortValueName(node,p)
            	self.st.out("// private storage for " + portValueName)
            	self.st.out(portValue + " " + portValueName + ";")
            param_type = "Bottle"

            self.st.out("// the port " + portname + " itself") 
            self.st.out( "zmq_socket_ext " + portname + ";")

    # In the constructor we allocate the port, most often we will need a new BufferedPort with a 
    # Bottle as parameter. In case of a sequence we need to allocate a corresponding vector
    def writePortAllocation(self, node):
        for p in node.parameters():
            param_name = p.identifier()
            self.extractNr(param_name)
            iterator = self.__result_type
            if iterator == '': 
                iterator = '1'
            
            self.__prefix = ""
            p.paramType().accept(self)
            param_type = self.__result_type

            portname = "port" + node.identifier()
            
            if p.paramType().kind() == 3:
                param_type = "Bottle"
                if p.is_in():
                    self.st.out(portname + "Value = 0;") 
            if p.paramType().kind() == 21:
                #if p.is_in():
                #    self.getSeqType(param_type)
                #    seq_type = self.__result_type
                #    self.st.out(portname + "Values(NULL);")  # no allocation needed anymore
                param_type = "Bottle"

            if p.is_in():
              self.st.out( portname + ".sock = new zmq::socket_t(*context, ZMQ_REP);")
              self.st.out( portname + ".ready = true;")
            if p.is_out():
              self.st.out( portname + ".sock = new zmq::socket_t(*context, ZMQ_REQ);")
              self.st.out( portname + ".ready = true;")

            self.st.out( "zmq_sockets.push_back(&" + portname + ");")

    # In the constructor we allocate the port, most often we will need a new BufferedPort with a 
    # Bottle as parameter. In case of a sequence we need to allocate a corresponding vector
    def writePortDestruction(self, node):
        for p in node.parameters():
            self.__prefix = ""
            p.paramType().accept(self)
            param_type = self.__result_type
#            if p.paramType().kind() == 21:
#                if p.is_in():
#                    self.getSeqType(param_type)
#                    seq_type = self.__result_type
#                    self.st.out("delete port" + node.identifier() + "Values;")
            self.st.out("delete port" + node.identifier() + ".sock;" ) 

    # In the Init() routine we open the port and if necessary set the default values of the corresponding
    # data structures 
    def writePortInit(self, node):
        for p in node.parameters():
            param_name = p.identifier()
            self.extractNr(param_name)
            
            self.__prefix = "struct "
            p.paramType().accept(self)
            param_type = self.__result_type
            
            portname = "port" + node.identifier()

            self.st.out("{")
            self.st.inc_indent()
       
            if p.is_out():
              self.st.out( "// outgoing port, function as server" );
            if p.is_in():
              self.st.out( "// incoming port, function as client" );
       
            self.st.out( "std::stringstream portName; portName.str(""); portName.clear();" )
            self.st.out( "portName << \"" + '/' + self.classname.lower() + '\" << module_id << \"/' + node.identifier().lower() + "\";") 
            self.st.out( portname + ".name = portName.str();")
            self.st.out( "std::string resolve = \"/resolve\" + portName.str();" );
            self.st.out( "pns_record record;" )
            self.st.out( "record.name = resolve;" )
            self.st.out( "std::stringstream ss; ss.clear(); ss.str(\"\");" )
            self.st.out( "ss << getpid();" )
            self.st.out( "record.pid = ss.str();" )
            self.st.out( "Resolve(record);" )

            if p.is_in():
              self.st.out( "ss.clear(); ss.str(\"\");" )
              self.st.out( "ss << \"tcp://\" << record.host << \":\" << record.port; ")
              self.st.out( "std::string sock = ss.str(); ")
              self.st.out( "std::cout << \"Bind to socket \" << sock << std::endl; ")
              self.st.out( portname + ".sock->bind(sock.c_str());")
		
            self.st.dec_indent()
            self.st.out("}")

    def writePortClose(self, node):
        for p in node.parameters():
            param_name = p.identifier()
            self.extractNr(param_name)
            
            self.__prefix = "struct "
            p.paramType().accept(self)
            param_type = self.__result_type
            
            portname = "port" + node.identifier()
            
            if p.paramType().kind() == 3:
                param_type = "Bottle"

            self.st.out( portname + ".sock->close();")

    # The ports themselves will become again functions, like readInput() or writeOutput()
    # The result of this function will be a list of such functions
    # All these functions will be "protected" and can be accessed only by the class or its parent.
    def writePorts(self):
        for m in self.portList:
            for p in m.parameters():
                param_name = p.identifier()
                self.__prefix = ""
                p.paramType().accept(self)
                param_type = self.__result_type
                portname = "port" + m.identifier()

                if p.is_in():
                  self.st.out( "/**" )
                  self.st.out( " * The \"read" + param_name + "\" function receives stuff over a zeromq REP socket. It works as a client. It is better not" )
                  self.st.out( " * to run it in blocking mode, because this would make it impossible to receive message on other ports (under which " )
                  self.st.out( " * the /pid/control port). The function returns NULL if there is no new item available." )
                  self.st.out( " */" )

                  # function signature
                  portValue = self.getPortValue(p);
                  portValueName = self.getPortValueName(m, p);
                  self.st.out( "inline " + portValue + "* read" + m.identifier() + "(bool blocking=false) {" ) 

                  # function content 
                  self.st.inc_indent()
                  self.st.out( "// For now only int return values are supported" )
                  self.st.out( "int reply_size = -1;")
                  self.st.out( "char *reply = GetReply(" + portname + ".sock, " + portname + ".ready, blocking, reply_size);")
                  self.st.out( "if (!" + portname + ".ready || !reply) return NULL;" )
                  self.st.out( "SendAck(" + portname + ".sock, " + portname + ".ready);")
                  self.st.out( "if (reply_size < 3) std::cerr << \"Error: Reply is not large enough to store an integer!\" << std::endl;")
                  self.st.out( "std::stringstream ss; ss.clear(); ss.str(\"\");")
                  self.st.out( "ss << std::string(reply);")
                  if p.paramType().kind() == 21:
                      #self.st.out( "for (int i = 0; i < " + portValueName + ".size(); ++i) ss >> " + portValueName + "[i];")
                       self.st.out( "assert(false);  // todo: cast char* array to vector of int")
#                      self.st.out( "ss >> " + portValueName + ";")
                  else:
                      self.st.out( "ss >> " + portValueName + ";")
#                  self.st.out( portname + "Value = (reply[0]) + (reply[1] << 8); // little-endianness")
#                  self.st.out( " std::cout << \"Values\" << (reply[0]) << \" and \" << (reply[1]) << std::endl; " )
                  self.st.out( "delete [] reply;")
                  self.st.out( "return &" + portValueName + ";")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")
                  
                if p.is_out():
                  self.st.out( "/**" )
                  self.st.out( " * The " + param_name + " function sends stuff over a zeromq REQ socket. It works as a server. It cannot be blocking because this" )
                  self.st.out( " * would make it impossible to receive message on other ports (under which the /pid/control port). It could have been" )
                  self.st.out( " * blocking if it is known if it is connected to a REP port (but the connected() function is apparently not meant for" )
                  self.st.out( " * that)." )
                  self.st.out( " */" )

                  # function signature
                  if p.paramType().kind() == 21: # sequence
                      self.getSeqType(param_type)
                      seq_type = self.__result_type
                      self.st.out( "inline bool write" + m.identifier() + "(const " + param_type + " &" + param_name + ") {" )
                  else:
                      self.st.out( "inline bool write" + m.identifier() + "(const " + param_type + " " + param_name + ") {" )
                  
                  # function content
                  self.st.inc_indent()
                  self.st.out( "// For now only int return values are supported" )
 #                 self.st.out("if (!ReceiveGeneralRequest(" + portname + ", " + portname + "Ready, false)) return false;")
                  self.st.out( "std::stringstream ss; ss.clear(); ss.str(\"\");")
                  self.st.out( "ss << " + param_name + "; // very dirty, no endianness, etc, just use the stream operator itself") 
#                  self.st.out( "ss << (" + param_name + "& 0xFF) << (" + param_name + ">> 8); // little-endian") 
                  self.st.out( "bool state = " + portname + ".ready;")
                  self.st.out( "SendRequest(" + portname + ".sock, state, false, ss.str());")
                  self.st.out( "if (state) " + portname + ".ready = false;")
                  self.st.out( "if (!" + portname + ".ready) {")
                  self.st.inc_indent()
                  self.st.out( "bool ack_state = true;")
                  self.st.out( "ReceiveAck(" + portname + ".sock, ack_state, true);")
                  self.st.out( "if (ack_state) {")
                  self.st.inc_indent()
                  self.st.out(  portname + ".ready = true;")
                  self.st.out( "return true;")
                  self.st.dec_indent()
                  self.st.out( "}")
                  self.st.dec_indent()
                  self.st.out( "}")
                  self.st.out( "return false;")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")

    def extractNr(self, str):
        nr = filter(lambda x: x.isdigit(), str)
        self.__result_type = nr 
        
##########################################################################################
# Visitor functions
##########################################################################################

    # Use the already build abstract syntax tree and visit all nodes
    def visitAST(self, node):
        # First visit all of then nodes
        for n in node.declarations():
           n.accept(self)
        # And then write everything to the to-be-generated header file
        self.writeAll()

    # The module entity is "misused" as a namespace declaration
    def visitModule(self, node):
        self.namespace = idlutil.ccolonName(node.scopedName())
        for n in node.definitions():
            n.accept(self)
 
    # We will just add the structs to a list, to be handled later
    def visitStruct(self, node):
        self.structList.append(node)
        
    def visitInterface(self, node):
        self.classname = node.identifier()
        for c in node.callables():
            self.portList.append(c)

    def visitTypedef(self, node):
        self.typedefList.append(node)

    def visitPragma(self, node):
        self.pragmaList.append(node)

    ttsMap = {
        idltype.tk_void:       "void",
        idltype.tk_short:      "short",
        idltype.tk_long:       "int",
        idltype.tk_ushort:     "unsigned short",
        idltype.tk_ulong:      "unsigned long",
        idltype.tk_float:      "float",
        idltype.tk_double:     "double",
        idltype.tk_boolean:    "boolean",
        idltype.tk_char:       "char",
        idltype.tk_octet:      "octet",
        idltype.tk_any:        "any",
        idltype.tk_TypeCode:   "CORBA::TypeCode",
        idltype.tk_Principal:  "CORBA::Principal",
        idltype.tk_longlong:   "long long",
        idltype.tk_ulonglong:  "unsigned long long",
        idltype.tk_longdouble: "long double",
        idltype.tk_wchar:      "wchar"
        }

    def visitBaseType(self, type):
        self.__result_type = self.ttsMap[type.kind()]

    def visitStringType(self, type):
        if type.bound() == 0:
            self.__result_type = "std::string"
        else:
            self.__result_type = "std::string<" + str(type.bound()) + ">"

    def visitWStringType(self, type):
        if type.bound() == 0:
            self.__result_type = "wstring"
        else:
            self.__result_type = "wstring<" + str(type.bound()) + ">"

    def visitSequenceType(self, type):
        type.seqType().accept(self)
        if type.bound() == 0:
            self.__result_type = self.__result_type + "*"
            #self.__result_type = self.__result_type + "[]"
        else:
            self.__result_type = self.__result_type + "[" +\
                                 str(type.bound()) + "];"

    def visitSequenceTypeToVector(self, type):
        type.seqType().accept(self)
        self.__result_type = "std::vector<" + self.__result_type + ">"

    def visitRawSequenceType(self, type):
        type.seqType().accept(self)
        self.__result_type = self.__result_type

    # returns "float" for "typedef sequence<float> float_seq;" given "float_seq"  
    def getSeqType(self, type):
        for t in self.typedefList:
            for d in t.declarators():
                if (d.identifier() == type):
                    self.visitRawSequenceType(t.aliasType())  
                    self.__result_type = self.__result_type
                             
    def visitFixedType(self, type):
        if type.digits() > 0:
            self.__result_type = "fixed<" + str(type.digits()) + "," +\
                                 str(type.scale()) + ">"
        else:
            self.__result_type = "fixed"

    def visitDeclaredType(self, type):
        self.__result_type = self.__prefix + type.decl().scopedName()[-1]


def run(tree, args):
    st = output.Stream(sys.stdout, 2)
    dv = ZeroMQVisitor(st)
    tree.accept(dv)

