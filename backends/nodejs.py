# -*- python -*-
#                           Package   : omniidl
# nodejs.py                 Created on: 2013/04/01
#                           Author    : Bart van Vliet
#
#    Copyright (C) 2011 Almende B.V.
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
#   NodeJS back-end for network abstractions 

"""NodeJS IDL compiler back-end."""

# Example: https://github.com/kkaefer/node-cpp-modules
# Example: https://bravenewmethod.wordpress.com/2011/03/30/callbacks-from-threaded-node-js-c-extension/
# More info: http://nikhilm.github.io/uvbook/threads.html
# node::ObjectWrap::Ref - Keep your C++ objects around even if they aren't referenced.
# v8::Signature - Ensure that "this" always refers to your wrapped C++ object.
# TODO: add TryCatch to catch errors.
# TODO: add ThrowException to show errors
# TODO: add char support
# TODO: use ->IntegerValue() instead of ->NumberValue() for integers

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

sys.path.append('./helper')
from helper import rur

class StandardVisitor (rur.RurModule):

    def __init__(self, st):
        self.st = st

##########################################################################################
# Write functions
##########################################################################################

    def writeAll(self):
        self.writeFileComment()
        self.writeIncludeGuardStart()
        print "#include <string>"
        print "#include <vector>" # almost always needed
        print "#include <deque>"
        print "#include <node.h>"
        print "#include <pthread.h>"
        print ""
        self.writeNamespaceStart()

        for s in self.structList:
            self.writeStructDeclaration(s)
        self.writeTypedefs()
        
        self.writeInterface()
        
        self.writeNamespaceEnd()
        self.writeIncludeGuardEnd()

    def writeNamespaceStart(self):
        print "namespace " + self.namespace + " {"
        print

    def writeNamespaceEnd(self):
        print "}"
        print

    # Only accept typedef for sequences for now
    def writeTypedefs(self):        
        for t in self.typedefList:
            for d in t.declarators():
                a = t.aliasType()
                if isinstance(a,idltype.Sequence):
                    seq_type = self.seqToVec(a)
                    self.st.out("typedef " + seq_type + " " + d.identifier() + ";")
                    #self.st.out("typedef std::deque<" + seq_type + "> " + d.identifier() + "_buf;")
                #else:
                    #self.st.out("typedef std::deque<" + a + ">" + d.identifier() + "_buf;")
                #self.st.out("")
        self.st.out("")

    def writeInterface(self):
        self.writeClassStart()
        self.st.inc_indent()
        
        # Private part
        for p in self.portList:
            self.writeBufAllocation(p)
        for s in self.structList:
            self.writeStructInstance(s)
        self.writePrivateVars()
        self.writeNodeConstructor()
        self.writeNodeDestructor()
        self.writeNodePorts();
        
        self.st.dec_indent()

        # Protected part
        self.st.out("protected:")
        self.st.inc_indent()
        
        self.writePortsAsArray()
        
        self.st.dec_indent()
        self.st.out("")

        # Public part
        self.st.out("public:")
        self.st.inc_indent()
        
        self.writeConstructor()
        self.writeDestructor()
        
        self.writeInit()
        self.writeStructParam();
        self.writeRun();
        self.writeNodeRegister();
#        for c in node.callables():
#            self.methodList.append(c)
        self.writePorts();
        
        self.writeClassEnd()

    # The struct definition itself 
    def writeStructDeclaration(self, node):
        self.st.out( "struct " + node.identifier() + " {" )
        self.st.inc_indent()
        for m in node.members():
            if m.constrType():
                m.memberType().decl().accept(self)
            type = self.getType(m.memberType())
            for d in m.declarators():
                membername = d.identifier()
                self.st.out(type + " " + membername + ";")

        self.st.dec_indent()
        self.st.out("};")
        self.st.out("")

    # Declare internal dummy instances for custom structs
    # we define an exception for the struct Param that can be used to store command-line parameters
    def writeStructInstance(self, node): 
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out( name + ' *cli' + name + ";" )
            self.st.out("")
        else:
            self.st.out( name + ' *dummy' + name + ";" )
            self.st.out("")

    # Allocate structs
    # we define an exception for the struct Param that can be used to store command-line parameters    
    def writeStructAllocation(self, node):
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out("cli" + name + " = new " + name + "();")
        else:
            self.st.out("dummy" + name + " = new " + name + "();")

    def writePrivateVars(self):
        self.st.out("pthread_t moduleThread;")
        self.st.out("bool DestroyFlag;")
        self.st.out("pthread_mutex_t destroyMutex;")
        self.st.out("")

    def writePrivateVarsInitiation(self):
        self.st.out("DestroyFlag = false;")

    # Dedicate setter and getter for Param struct
    def writeStructParam(self):
        for s in self.structList:
            name = s.scopedName()[-1]
            if name == 'Param':
                self.st.out('// Function to get Param struct (to subsequently set CLI parameters)')
                self.st.out('inline Param *GetParam() { return cliParam; };')
                self.st.out("")

    def writeClassStart(self):
        self.st.out( "class " + self.classname + " : public node::ObjectWrap {" )
        self.st.out( "private:" )

    def writeClassEnd(self):
        self.st.dec_indent()
        self.st.out( "};" )

    def writeConstructor(self):
        self.st.out( self.classname + "();" )
        self.st.out("")

    def writeDestructor(self):
        self.st.out( "~" + self.classname + "();" )
        self.st.out("")

    def writeRun(self):
        self.st.out("void Run();")
        self.st.out("")

    def writeInit(self):
        self.st.out("// Overwrite this function with your own code")
        self.st.out("virtual void Tick() {}")
        self.st.out("")
        self.st.out("// Overwrite this function with your own code")
        self.st.out("bool Stop() { return false; }")
        self.st.out("")
        self.st.out("// Extend this with your own code, first call " + self.classname + "::Init(name); ")
        self.st.out("void Init(std::string& name);")
        self.st.out("")
        
    def writeNodeRegister(self):
        self.st.out("// Function template for NodeJS, do not use in your own code")
        self.st.out("static void NodeRegister(v8::Handle<v8::Object> exports);")
        self.st.out("")
    
    def writeNodeConstructor(self):
        self.st.out("static v8::Handle<v8::Value> NodeNew(const v8::Arguments& args);")
        self.st.out("")

    def writeNodeDestructor(self):
        self.st.out("static v8::Handle<v8::Value> NodeDestroy(const v8::Arguments& args);")
        self.st.out("")
        self.st.out("bool Destroy();")
        self.st.out("")

    def writePorts(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            #self.writePortFunctionSignature(p) Can't use this now
            if port_direction == rur.Direction.IN:
                self.st.out("// Read from this function and assume it means something")
                self.st.out(param_type + " *read" + port_name + "(bool blocking_dummy=false);")
            if port_direction == rur.Direction.OUT:
                self.st.out("// Write to this function and assume it ends up at some receiving module")
                self.st.out("bool write" + port_name + "(const " + param_type + " " + param_name + ");")
            self.st.out("")

    def writeNodePorts(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                # Port IN, so in javascript you write to it
                self.st.out("// Function to be used in NodeJS, not in your C++ code")
                self.st.out("static v8::Handle<v8::Value> NodeWrite" + port_name + "(const v8::Arguments& args);")
                self.st.out("")
                
            if port_direction == rur.Direction.OUT: 
                # Port OUT, so in javascript you read from it
                self.st.out("// Function to be used in NodeJS, not in your C++ code")
                self.st.out("static v8::Handle<v8::Value> NodeRegRead" + port_name + "(const v8::Arguments& args);")
                self.st.out("")
                
                # The callback function that calls the javascript callback
                self.st.out("// Function to be used internally, not in your C++ code")
                self.st.out("static void CallBack" + port_name + "(uv_async_t *handle, int status);")
                self.st.out("")

    def writePortsAsArray(self):
        names = [];
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                names.append("\"read" + port_name + "\"") 
            if port_direction == rur.Direction.OUT: 
                names.append( "\"write" + port_name + "\"") 
        self.st.out("static const int channel_count = " + str(len(names)) + ";")
        # bug: http://stackoverflow.com/questions/9900242/error-with-constexprgcc-error-a-brace-enclosed-initializer-is-not-allowed-h
        # self.st.out("const char* const channel[" + str(len(names)) + "] = {" + ', '.join(names) + "};")
        self.st.out("const char* channel[" + str(len(names)) + "];")

    def writeBufAllocation(self, node):
        port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(node)
        if port_direction == rur.Direction.IN:
            #self.st.out(param_type + "_buf readBuf" + port_name + ";")
            self.st.out("std::deque<" + param_type + "> readBuf" + port_name + ";")
            self.st.out(param_type + " readVal" + port_name  + ";")
            self.st.out("pthread_mutex_t readMutex" + port_name + ";")
            self.st.out("")
        if port_direction == rur.Direction.OUT:
            #self.st.out(param_type + "_node writeBuf" + port_name + ";")
            self.st.out("std::deque<" + param_type + "> writeBuf" + port_name + ";")
            self.st.out("v8::Persistent<v8::Function> nodeCallBack" + port_name + ";")
            self.st.out("uv_async_t async" + port_name + ";")
            self.st.out("pthread_mutex_t writeMutex" + port_name + ";")
            self.st.out("")

# Initialize this parser
def run(tree, args):
    st = output.Stream(sys.stdout, 2)
    dv = StandardVisitor(st)
    tree.accept(dv)
    # And then write everything to the to-be-generated header file
    dv.writeAll()

