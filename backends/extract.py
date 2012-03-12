# -*- python -*-
#                           Package   : omniidl
# extract.py                   Created on: 2011/09/30
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
#   extract back-end for network abstractions 

"""extract IDL compiler back-end."""

from omniidl import idlast, idltype, idlutil, idlvisitor, output
import sys, string

class ExtractVisitor (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

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
  
    # We write all relevant info to a file that subsequently can be parsed by other programs
    # For now we write port names
    def writeAll(self):
        for m in self.portList:
            self.writePort(m)
        for s in self.structList:
            self.writeStruct(s)

    # Of every port we only write "in portname" and "out portname", the port names are 
    # extracted from the defined methods. For example "void Input(in long data_in);"
    # becomes readInput and internally the used channel name is "input" (lower case).
    def writePort(self, node):
        for p in node.parameters():
            param_name = p.identifier()
            self.extractNr(param_name)
            iterator = self.__result_type
            if iterator == '': 
                iterator = '1'
            
            self.__prefix = "struct "
            p.paramType().accept(self)
            param_type = self.__result_type
            
            it = "i" + node.identifier();
            portname = "port" + node.identifier()

            if p.is_in():
               prefix = "in "
            if p.is_out():
               prefix = "out "
#            if p.is_inout():
#               prefix = "inout "

            channel = node.identifier().lower()
            nof_ports = int(iterator)
            for i in range(nof_ports) :
               self.st.out( prefix + channel + str(i) )

    def writeStruct(self, node):
        name = node.scopedName()[-1]
        if name == 'Param':
            for m in node.members():
                m.memberType().accept(self)
                type = self.__result_type

            #    self.st.out('param '+ m)
                for d in m.declarators():
                    membername = d.identifier()
                    self.st.out("param " + type + " " + membername)




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
    dv = ExtractVisitor(st)
    tree.accept(dv)
