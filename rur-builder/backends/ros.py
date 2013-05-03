# -*- python -*-
#                           Package   : omniidl
# ros.py                   Created on: 2011/09/30
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
#   ROS back-end for network abstractions 

"""ROS IDL compiler back-end."""

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

class RosVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    # The user is able to define its own data structures to be used on the channels 
    structList = []
    
    # Every method corresponds to a port/channel
    portList = []
    
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
#        self.writeFileComment()
        self.writeIncludes()
        self.writeNamespaceStart()
        for s in self.structList:
           self.writeStructDeclaration(s)
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
        print "#include <iostream>"
        print "#include <string>"
        print
        print "// middleware specific headers"
        print "#include <ros/ros.h>"
        print "#include \"std_msgs/String.h\""
        print "#include \"std_msgs/Int16.h\""
        print
#        print "// namespaces and typedefs"
#        print "using namespace ros;"
        print

    def writeNamespaceStart(self):
        print "// recommended namespace: \"Rur\""
        print "// add \"using namespace Rur;\" to your .cpp file"
        print "namespace " + self.namespace + " {"
        print

    def writeNamespaceEnd(self):
        print "}"
        print

    def writeInterface(self):
        print "// The generated class. Do not modify or add class members"
        print "// Either derive from this class and implement Tick() or"
        print "// use a separate helper class to store state information."
        print "// All information for the operation of the module should "
        print "// be obtained over the defined ports"
        self.writeClassStart()
        self.st.inc_indent()
        
        for m in self.portList:
            self.writePortVectorAllocation(m)
            
        # Create blob with private dummy instances        
#        for s in self.structList:
#            self.writeStructInstance(s)
        
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
    def writeStructInstance(self, node): 
        name = node.scopedName()[-1]
        self.st.out( name + ' *dummy' + name + ";" )
    
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
        self.st.out("}")
        self.st.out("")

    def writeStructAllocation(self, node):
        name = node.scopedName()[-1]
        self.st.out( 'dummy' + name + " = new " + name + "();" )
        
    def writeClassStart(self):
        self.st.out( "class " + self.classname + " {" )
        self.st.out( "private:" )
        self.st.inc_indent()
        self.st.out( "ros::NodeHandle n;")
        self.st.out( "std::string module_id;")
        self.st.dec_indent()
        
    def writeClassEnd(self):
        self.st.out( "};" )
                
    def writeConstructor(self):
        self.st.out( "// The constructor needs to be called, also when you derive from this class" )
        self.st.out( self.classname + "() {" )
        self.st.inc_indent()
        
        for m in self.portList:
            self.writePortAllocation(m)
        self.st.dec_indent()
        self.st.out( "}" )

    def writeDestructor(self):
        self.st.out( "~" + self.classname + "() { }" )
        
    def writeInit(self):
        self.st.out( "// This is the function you will need to implement." )
        self.st.out( "void Tick() {}")
        self.st.out("")
        self.st.out( "// After construction you will need to call this function first" )
#        self.st.out( "// it opens the YARP ports" )
        self.st.out( "void Init(std::string module_id) {" )
        self.st.inc_indent()
#        for p in self.portList:
#            self.writePortInit(p)
        self.st.out("this->module_id = module_id;")
        self.st.out("int argc = 1;")
        self.st.out("char** argv = NULL;")
        self.st.out("ros::init(argc, argv, \"" + self.classname.lower() + "\");" )
        self.st.dec_indent()
        self.st.out( "}" )
    
    def writePorts(self):
        for m in self.portList:
            for p in m.parameters():
                param_name = p.identifier()
                self.__prefix = ""
                p.paramType().accept(self)
                param_type = self.__result_type
                portname = "port" + m.identifier()

                if p.is_in():
                  self.st.out( "inline " + param_type + " *read" + m.identifier() + "(bool blocking) {" )
                  self.st.inc_indent()
                  if p.paramType().kind() == 3:
#                     self.st.out( "Bottle *b = " + portname + "[index]->read();")
#                     self.st.out( portname + "Values[index] = b->get(0).asInt();")
                     self.st.out( "return &" + portname + "Values;") 
                  else:
                     self.st.out( "return " + portname + ".read();")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")
                  
                  if p.paramType().kind() == 3:
                      self.st.out( "void " + m.identifier() + "Callback(std_msgs::Int16 & msg, int index) " + "{" )
                      self.st.inc_indent()
                      self.st.out( portname + "Values[index] = msg;")
                      self.st.dec_indent()
                      self.st.out("}")
                      self.st.out("")
                  
                if p.is_out():
                  self.st.out( "inline bool write" + m.identifier() + "(const " + param_type + " " + param_name + ") {" )
                  self.st.inc_indent()
                  if p.paramType().kind() == 3:
                      self.st.out( "std_msgs::Int16 msg; msg.data = output;" )
#                      self.st.out( "std::stringstream ss;" )
#                      self.st.out( "ss << output;" )
#                      self.st.out( "msg.data = ss.str();" )
#                     self.st.out( "Bottle &" + param_name + "Prepare = " + portname + "[index]->prepare();")
#                     self.st.out( param_name + "Prepare.clear();")
#                     self.st.out( param_name + "Prepare.addInt(" + param_name + ");")
#                  else:
#                     self.st.out( param_type + "& " + param_name + "Prepare = " + portname + "[index]->prepare();")
                  self.st.out( portname + ".publish(msg);")
                  self.st.out( "return true;")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")
                  
# We need a vector of ports or publishers or whatever is used by the middleware
# inputs are read by "subscribe", and written by "advertise" and "publish"  
    def writePortVectorAllocation(self, node):
        self.st.out("")
        for p in node.parameters():
            param_name = p.identifier()
            self.__prefix = "struct "
            p.paramType().accept(self)
            param_type = self.__result_type
            portname = "port" + node.identifier()
            # if we have basic data types (like int) we need to wrap it something known
            # in the middleware
            if p.paramType().kind() == 3:
                if p.is_in():
                   #self.st.out( "std::vector< " + param_type + "> " + portname + "Values;")
                   self.st.out( portname + "Values;")
                param_type = "std_msgs::Int16"
            if p.is_in():
                self.st.out( "ros::Subscriber " + portname + ";")
                #self.st.out( "std::vector< ros::Subscriber > " + portname + ";")
                #self.st.out( "std::vector< ros::Subscriber <" + param_type + ">* > " + portname + ";")
            else:
                self.st.out( "ros::Publisher " + portname + ";") 
                #self.st.out( "std::vector< ros::Publisher > " + portname + ";") 
                #self.st.out( "std::vector< ros::Publisher <" + param_type + ">* > " + portname + ";") 

    def writePortAllocation(self, node):
        self.st.out("")
        for p in node.parameters():
            param_name = p.identifier()
            self.extractNr(param_name)
            iterator = self.__result_type
            if iterator == '': 
                iterator = '1'
            
            self.__prefix = "struct "
            p.paramType().accept(self)
            param_type = self.__result_type
            
            self.st.out("{")
            self.st.inc_indent()
            if p.paramType().kind() == 3:
                param_type = "std_msgs::Int16"
                if p.is_in():
                   self.st.out( "port" + node.identifier() + "Values.push_back(-1);")
            channel = '/' + node.identifier().lower()
            if p.is_in():
                self.st.out( "std::string portName = \"" + channel + "\" + module_id;")
                self.st.out( "port" + node.identifier() + " = n.subscribe (portName.c_str(), 1000, boost::bind(&" + self.classname + "::" + node.identifier() + "Callback, this, _1, i) );")
                #self.st.out( "port" + node.identifier() + ".push_back(n.subscribe (portName.c_str(), 1000, boost::bind(&" + self.classname + "::" + node.identifier() + "Callback, this, _1, i) ) );")
            else:
                self.st.out( "std::string portName = \"" + channel + "\" + module_id;")
                self.st.out( "port" + node.identifier() + " = n.advertise <" + param_type + ">(portName.c_str(), 1000);")
                #self.st.out( "port" + node.identifier() + ".push_back(n.advertise <" + param_type + ">(portName.c_str(), 1000) );")
                #self.st.out( "port" + node.identifier() + ".push_back(n.advertise(portName.c_str(), 1000) );")
            self.st.dec_indent() 
            self.st.out("}")

    def writePortInit(self, node):
        self.st.out("")
#        for p in node.parameters():
#            param_name = p.identifier()
#            self.extractNr(param_name)
#            iterator = self.__result_type
#            if iterator == '': 
#                iterator = '1'
#            
#            self.__prefix = "struct "
#            p.paramType().accept(self)
#            param_type = self.__result_type
#            
#            it = "i" + node.identifier();
#            portname = "port" + node.identifier()
#            
#            if p.paramType().kind() == 3:
#                param_type = "Bottle"
#            self.st.out( "std::vector< BufferedPort <" + param_type + ">* >::iterator " + it + ";") 
#
#            self.st.out("for (" + it + " = " + portname + ".begin(); " + it + " != " + portname + ".end(); ++" + it + ") {")
#            self.st.inc_indent()
#            channel = '/' + self.classname.lower() + '/' + node.identifier().lower()
#            self.st.out( '(*' + it + ')->open("' + channel + '");')
#            self.st.dec_indent() 
#            self.st.out("}")
                  
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
            self.__result_type = "string"
        else:
            self.__result_type = "string<" + str(type.bound()) + ">"

    def visitWStringType(self, type):
        if type.bound() == 0:
            self.__result_type = "wstring"
        else:
            self.__result_type = "wstring<" + str(type.bound()) + ">"

    def visitSequenceType(self, type):
        type.seqType().accept(self)
        if type.bound() == 0:
            self.__result_type = self.__result_type + "*"
        else:
            self.__result_type = self.__result_type + "[" +\
                                 str(type.bound()) + "];"

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
    dv = RosVisitor(st)
    tree.accept(dv)
