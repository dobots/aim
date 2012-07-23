# -*- python -*-
#                           Package   : omniidl
# yarp.py                   Created on: 2011/09/30
#                           Author    : Anne C. van Rossum
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
#   YARP back-end for network abstractions 

"""YARP IDL compiler back-end."""

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

class YarpVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    # The user is able to define its own data structures to be used on the channels 
    structList = []
    
    # Every method corresponds to a port/channel
    portList = []
    
    # Typedef list
    typedefList = []
    
    # The module's name
    classname = ''
    
    # The namespace
    namespace = ''
     
    def __init__(self, st):
        self.st = st


##########################################################################################
# Write functions
##########################################################################################
  
    def writeAll(self):
        self.writeFileComment()
        self.writeIncludes()
        self.writeNamespaceStart()
        for s in self.structList:
           self.writeStructDeclaration(s)
        self.writeTypedefs()
        self.writeInterface()
        self.writeNamespaceEnd()
        
    def writeFileComment(self):
        comment_text = '''/**
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright (c) 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @company Almende B.V.
 */
'''
        print comment_text

    def writeIncludes(self):
        print "// general C/C++ headers"
        print "#include <vector>"
        print "#include <string>"
        print "#include <sstream>"
        print
        print "// middleware specific headers"
        print "#include <yarp/os/BufferedPort.h>"
        print "#include <yarp/os/Network.h>"
        print "#include <yarp/os/Bottle.h>"
        print
        print "// namespaces and typedefs"
        print "using namespace yarp::os;"
        print       

    def writeNamespaceStart(self):
        print "// recommended namespace: \"rur\""
        print "// do not forget to add \"using namespace rur;\" to your .cpp file"
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
                    self.visitSequenceTypeToVector(a)  
                    seq_type = self.__result_type
                    self.st.out("typedef " + seq_type + " " + d.identifier() + ";")
                self.st.out("")

    def writeInterface(self):
        print "// The generated class. Do not modify or add class members"
        print "// Either derive from this class and implement Tick() or"
        print "// use a separate helper class to store state information."
        print "// All information for the operation of the module should "
        print "// be obtained over the defined ports"
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
        self.writeInit()
        self.st.out("")

        self.writeStructParam();

        self.st.dec_indent()
        self.st.out("protected:")
        self.st.inc_indent()
        
        self.st.out("// All subsequent functions should be called from \"within\" this module" )
        self.st.out("// From either the Tick() routine itself, or Tick() in a derived class" )
        self.st.out("")
        self.writePorts();
        
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
        self.st.out( "Network yarp;")
        self.st.out( "std::string module_id;")
        self.st.dec_indent()
        
    def writeClassEnd(self):
        self.st.out( "};" )

    # Write constructors plus comments                
    def writeConstructor(self):
        self.st.out( "// The constructor needs to be called, also when you derive from this class" )
        self.st.out( self.classname + "() {" )
        self.st.inc_indent()
        
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
        self.st.dec_indent()
        self.st.out( "}" )   
        
        
    def writeInit(self):
        self.st.out( "// This is the function you will need to implement." )
        self.st.out( "void Tick(); ")
        self.st.out("")
        self.st.out( "// After construction you will need to call this function first" )
        self.st.out( "// it opens the YARP ports" )
        self.st.out( "void Init(std::string module_id) {" )
        self.st.inc_indent()
        self.st.out( "this->module_id = module_id;" )
        self.st.out("")	
        for p in self.portList:
            self.writePortInit(p)
        self.st.dec_indent()
        self.st.out( "}" )   

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
            if p.paramType().kind() == 3:
                if p.is_in():
                    self.st.out("// private storage for " + portname + "Value")
            #        self.st.out("std::vector<" + param_type + "> " + portname + "Values;")
                    self.st.out("int " + portname + "Value;")
                param_type = "Bottle"
            elif p.paramType().kind() == 21:
                if p.is_in():
                    self.getSeqType(param_type)
                    seq_type = self.__result_type                   
                    self.st.out("// private storage for " + portname + "Values;")
                    self.st.out( "std::vector<" + seq_type + "> *" + portname + "Values;")
                param_type = "Bottle"
            #self.st.out("Type = " + str(p.paramType().kind()) +  " for " + param_type)
                
            #self.st.out( "std::vector< BufferedPort <" + param_type + ">* > " + portname + ";")
            self.st.out("// the port " + portname + " itself") 
            self.st.out( "BufferedPort<" + param_type + "> *" + portname + ";")

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
            
            if p.paramType().kind() == 3:
                param_type = "Bottle"
            if p.paramType().kind() == 21:
                if p.is_in():
                    self.getSeqType(param_type)
                    seq_type = self.__result_type
                    self.st.out("port" + node.identifier() + "Values = new std::vector<" + seq_type + ">();")
                param_type = "Bottle"
            self.st.out( "port" + node.identifier() + " = new BufferedPort<" + param_type + ">();")

    # In the constructor we allocate the port, most often we will need a new BufferedPort with a 
    # Bottle as parameter. In case of a sequence we need to allocate a corresponding vector
    def writePortDestruction(self, node):
        for p in node.parameters():
            self.__prefix = ""
            p.paramType().accept(self)
            param_type = self.__result_type
            if p.paramType().kind() == 21:
                if p.is_in():
                    self.getSeqType(param_type)
                    seq_type = self.__result_type
                    self.st.out("delete port" + node.identifier() + "Values;")
            self.st.out("delete port" + node.identifier() + ";" ) 

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
            
            if p.paramType().kind() == 3:
                param_type = "Bottle"

            self.st.out("{")
            self.st.inc_indent()
            self.st.out( "std::stringstream portName; portName.str(""); portName.clear();" )
            self.st.out( "portName << \"" + '/' + self.classname.lower() + '\" << module_id << \"/' + node.identifier().lower() + "\";") # << " + itIndex + ";")
            self.st.out( portname + "->open(portName.str().c_str());")
            self.st.dec_indent()
            self.st.out("}")

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
                  # function signature
                  if p.paramType().kind() == 21: # sequence
                     self.getSeqType(param_type)
                     seq_type = self.__result_type
                     self.st.out( "// Remark: caller is responsible for evoking vector.clear()" )
                     self.st.out( "inline std::vector<" + seq_type + "> *read" + m.identifier() + "(bool blocking=true) {" ) 
                  else:
                     self.st.out( "inline " + param_type + " *read" + m.identifier() + "(bool blocking=true) {" )
                     
                  # function content 
                  self.st.inc_indent()
                  if p.paramType().kind() == 3:
                     self.st.out( "Bottle *b = " + portname + "->read(blocking);") 
                     self.st.out( "if (b != NULL) { " )
                     self.st.inc_indent()
                     self.st.out( portname + "Value = b->get(0).asInt();") 
                     self.st.out( "return &" + portname + "Value;") 
                     self.st.dec_indent()
                     self.st.out("}")
                     self.st.out("return NULL;")
                  elif p.paramType().kind() == 21: # sequence
                     self.st.out( "Bottle *b = " + portname + "->read(blocking);")
                     self.st.out( "if (b != NULL) { " )
                     self.st.inc_indent()
                     self.st.out( "for (int i = 0; i < b->size(); ++i) {")
                     self.st.inc_indent()
                     # Here we have YARP specific data types
                     if seq_type == "int":
                         asValue = "asInt"
                     elif seq_type == "float":
                         asValue = "asDouble" # floats will be communicated as doubles
                     elif seq_type == "double":
                         asValue = "asDouble"
                     self.st.out( portname + "Values->push_back(b->get(i)." + asValue + "());")
                     self.st.dec_indent()
                     self.st.out("}")
                     self.st.dec_indent()
                     self.st.out("}")
                     self.st.out( "return " + portname + "Values;")
                  else:
                     self.st.out( "return " + portname + "[index]->read(blocking);")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")
                  
                if p.is_out():
                  # function signature
                  if p.paramType().kind() == 21: # sequence
                      self.getSeqType(param_type)
                      seq_type = self.__result_type
                      self.st.out( "inline void write" + m.identifier() + "(const " + param_type + " &" + param_name + ") {" )
                  else:
                      self.st.out( "inline void write" + m.identifier() + "(const " + param_type + " " + param_name + ") {" )
                  
                  # function content
                  self.st.inc_indent()
                  if p.paramType().kind() == 3:
                     self.st.out( "Bottle &" + param_name + "Prepare = " + portname + "->prepare();")
                     self.st.out( param_name + "Prepare.clear();")
                     self.st.out( param_name + "Prepare.addInt(" + param_name + ");")
                  elif p.paramType().kind() == 21: # sequence
                     self.st.out( "Bottle &" + param_name + "Prepare = " + portname + "->prepare();")
                     self.st.out( param_name + "Prepare.clear();")
                     self.st.out("for (int i = 0; i < output.size(); ++i) {")
                     self.st.inc_indent()
                     if seq_type == "int":
                         capValue = "Int"
                     elif seq_type == "float":
                         capValue = "Double" # float's will be communicated as doubles
                     elif seq_type == "double":
                         capValue = "Double"
                     self.st.out( param_name + "Prepare.add" + capValue + "(output[i]);")                     
                     self.st.dec_indent()
                     self.st.out("}")
                  else:
                     self.st.out( param_type + "& " + param_name + "Prepare = " + portname + "->prepare();")
                  self.st.out( portname + "->write();")
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
    dv = YarpVisitor(st)
    tree.accept(dv)
