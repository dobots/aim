# -*- python -*-
#                           Package   : omniidl
# standard.py               Created on: 2011/09/30
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
#   Standard back-end for network abstractions 

"""Standard IDL compiler back-end."""

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

class StandardVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

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
        print "#include <string>"
        print "#include <vector>" # almost always needed
        print
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
 * Copyright (c) 2012 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @company Almende B.V.
 */
'''
        print comment_text

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
                    self.visitSequenceTypeToVector(a)  
                    seq_type = self.__result_type
                    self.st.out("typedef " + seq_type + " " + d.identifier() + ";")
                self.st.out("")
                      

    def writeInterface(self):
        self.writeClassStart()
        self.st.inc_indent()
        
        for p in self.portList:
            self.writeDummyAllocation(p)
        
        # Create blob with private dummy instances        
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
        
#        for c in node.callables():
#            self.methodList.append(c)
        self.writePorts();
        
        self.st.dec_indent()
        self.writeClassEnd()

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
    
    # Declare internal dummy instances for custom structs
    # we define an exception for the struct Param that can be used to store command-line parameters
    def writeStructInstance(self, node): 
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out( name + ' *cli' + name + ";" )
        else:
            self.st.out( name + ' *dummy' + name + ";" )

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

    def writeClassEnd(self):
        self.st.out( "};" )
                
    def writeConstructor(self):
        self.st.out( self.classname + "() {" )
        self.st.inc_indent()
        for s in self.structList:
            self.writeStructAllocation(s)
        self.st.dec_indent()
        self.st.out( "}" )

    def writeDestructor(self):
        self.st.out( "~" + self.classname + "() { }" )
        
    def writeInit(self):
        self.st.out( "void Tick();" )
        self.st.out( "" )
        self.st.out( "bool Stop();" )	
        self.st.out( "" )
        self.st.out( "void Init(std::string & name) { }" )
    
    def writePorts(self):
        for m in self.portList:
            for p in m.parameters():
                param_name = p.identifier()
                self.__prefix = ""
                p.paramType().accept(self)
                param_type = self.__result_type
                portname = "port" + m.identifier()

                if p.is_in():
                  self.st.out( "inline " + param_type + " *read" + m.identifier() + "() {" ) 
                  self.st.inc_indent()
                  self.st.out( "return &dummy" + m.identifier() + ";")
                  self.st.dec_indent()
                  self.st.out("}")
                  self.st.out("")
                  
                if p.is_out():
                  self.st.out( "inline void write" + m.identifier() + "(const " + param_type + " " + param_name + ") {" )
                  self.st.out("}")
                  self.st.out("")
                
    def writeDummyAllocation(self, node):
        self.st.out("")
        for p in node.parameters():
            if p.is_in():
                param_name = p.identifier()
                self.__prefix = ""
                p.paramType().accept(self)
                param_type = self.__result_type
                self.st.out( param_type + " dummy" + node.identifier() + ";")  
  
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

    # Visit the interface and add to portList
    def visitInterface(self, node):
    	#self.st.out("Visit interface")
        self.classname = node.identifier()
        for c in node.callables():
            self.portList.append(c)

    def visitTypedef(self, node):
        self.typedefList.append(node)

	# The mapping from IDL types to C/C++ types, see e.g. that "long" becomes "int"
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
    dv = StandardVisitor(st)
    tree.accept(dv)
