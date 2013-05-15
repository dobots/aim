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
        print "#include <string>"
        print "#include <vector>" # almost always needed
        print
	
        self.writeNamespaceStart()
        for s in self.structList:
           self.writeStructDeclaration(s)
        self.writeTypedefs()
        self.writeInterface()
        self.writeNamespaceEnd()


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

	# protected part        
        self.st.out("")
        self.st.out("protected:")
        self.st.inc_indent()
	self.writePortsAsArray()
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
        names = [];
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                  names.append("\"read" + port_name + "\"") 
                  
            if port_direction == rur.Direction.OUT: 
                  names.append( "\"write" + port_name + "\"") 

        # bug: http://stackoverflow.com/questions/9900242/error-with-constexprgcc-error-a-brace-enclosed-initializer-is-not-allowed-h
        self.st.out("const char* const channel[" + str(len(names)) + "] = {" + ', '.join(names) + "};")
        
        for p in self.portList:
            self.writeDummyInitiation(p)
        for s in self.structList:
            self.writeStructAllocation(s)
        self.st.dec_indent()
        self.st.out( "}" )

    def writeDestructor(self):
        self.st.out( "~" + self.classname + "() { }" )
        
    def writeInit(self):
        self.st.out( "void Tick() {} " )
        self.st.out( "" )
        self.st.out( "bool Stop() { return false; }" )	
        self.st.out( "" )
        self.st.out( "void Init(std::string & name) { }" )
    
    def writePorts(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)

            if port_direction == rur.Direction.IN:
                  self.st.out( "// Read from this function and assume it means something")
                  self.writePortFunctionSignature(p)
                  self.st.out( "return &dummy" + port_name + ";")
                  self.writeFunctionEnd()
                  self.st.out("")
                  
            if port_direction == rur.Direction.OUT: 
                  self.st.out( "// Write to this function and assume it ends up at some receiving module")
                  self.writePortFunctionSignature(p)
                  self.st.out( "return true;")
                  self.writeFunctionEnd()
                
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

    def writeDummyAllocation(self, node):
        self.st.out("")
        port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(node)
        if port_direction == rur.Direction.IN:
             self.st.out( param_type + " dummy" + port_name + ";")  

    def writeDummyInitiation(self, node):
        port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(node)
        if port_direction == rur.Direction.IN:
             self.st.out( "dummy" + port_name + " = " + param_type + "(0);")  
  
# Initialize this parser
def run(tree, args):
	st = output.Stream(sys.stdout, 2)
	dv = StandardVisitor(st)
	tree.accept(dv)
	# And then write everything to the to-be-generated header file
        dv.writeAll()

