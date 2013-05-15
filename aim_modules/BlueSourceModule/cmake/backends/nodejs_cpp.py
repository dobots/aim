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
        print "#include <" + self.classname + ".h>"
        print "#include <" + self.classname + "Ext.h>"
        print ""
        
        self.st.out("using namespace " + self.namespace + ";")
        self.st.out("using namespace v8;")
        self.st.out("")
        
        self.st.out("typedef " + self.classname + "Ext className;")
        self.st.out("")
        
        self.writeNodeRun()
        self.writeNodeConstructor()
        self.writeNodeDestructor()
        self.writeNodePorts();
        
        self.writeConstructor()
        self.writeDestructor()
        
        self.writeInit()
        self.writeRun();
        self.writeNodeRegister();
#        for c in node.callables():
#            self.methodList.append(c)
        self.writePorts();

    # Allocate structs
    # we define an exception for the struct Param that can be used to store command-line parameters    
    def writeStructAllocation(self, node):
        name = node.scopedName()[-1]
        if name == 'Param':
            self.st.out("cli" + name + " = new " + name + "();")
        else:
            self.st.out("dummy" + name + " = new " + name + "();")

    def writePrivateVarsInitiation(self):
        self.st.out("DestroyFlag = false;")

    def writeConstructor(self):
        self.st.out(self.classname + "::" + self.classname + "() {")
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
        self.writePrivateVarsInitiation()
        for p in self.portList:
            self.writeBufInitiation(p)
        for s in self.structList:
            self.writeStructAllocation(s)
        self.writeFunctionEnd()
        self.st.out("")

    def writeDestructor(self):
        self.st.out(self.classname + "::~" + self.classname + "() { }")
        self.st.out("")

    def writeRun(self):
        self.st.out("void " + self.classname + "::Run() {")
        self.st.inc_indent()
        self.st.out("while (true) {")
        self.st.inc_indent()
        self.st.out("Tick();")
        self.writeFunctionEnd()
        self.writeFunctionEnd()
        self.st.out("")

    def writeInit(self):
        self.st.out("void " + self.classname + "::Init(std::string & name) {")
        self.st.inc_indent()
        self.st.out("cliParam->module_id = name;")
        self.st.out("// init more stuff here??")
        self.writeFunctionEnd()
        self.st.out("")
        
    def writeNodeRegister(self):
        self.st.out("void " + self.classname + "::NodeRegister(v8::Handle<v8::Object> exports) {")
        self.st.inc_indent()
        self.st.out("v8::Local<v8::FunctionTemplate> tpl = v8::FunctionTemplate::New(NodeNew);")
        self.st.out("tpl->SetClassName(v8::String::NewSymbol(\"" + self.classname + "\"));")
        self.st.out("tpl->InstanceTemplate()->SetInternalFieldCount(1);")
        self.st.out("")
        self.st.out("// Prototypes")
        self.st.out("tpl->PrototypeTemplate()->Set(v8::String::NewSymbol(\"Destroy\"), v8::FunctionTemplate::New(NodeDestroy)->GetFunction());")
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                self.st.out("tpl->PrototypeTemplate()->Set(v8::String::NewSymbol(\"Write" + port_name + "\"), v8::FunctionTemplate::New(NodeWrite" + port_name + ")->GetFunction());")
            if port_direction == rur.Direction.OUT:
                self.st.out("tpl->PrototypeTemplate()->Set(v8::String::NewSymbol(\"RegRead" + port_name + "\"), v8::FunctionTemplate::New(NodeRegRead" + port_name + ")->GetFunction());")
        self.st.out("")
        self.st.out("v8::Persistent<v8::Function> constructor = v8::Persistent<v8::Function>::New(tpl->GetFunction());")
        self.st.out("exports->Set(v8::String::NewSymbol(\"" + self.classname + "\"), constructor);")
        self.writeFunctionEnd()
        self.st.out("")
    
    def writeNodeConstructor(self):
        self.st.out("v8::Handle<v8::Value> " + self.classname + "::NodeNew(const v8::Arguments& args) {")
        self.st.inc_indent()
        self.st.out("v8::HandleScope scope;")
        self.st.out("className* obj = new className();")
        self.st.out("obj->Wrap(args.This());")
        self.st.out("")
        self.st.out("// Should obj->Init(name) here!") # should call Init(name) here
        self.st.out("")
        self.st.out("pthread_mutex_init(&(obj->destroyMutex), NULL);")
        self.st.out("")
        self.st.out("// Init ports")
        self.writePortInit()
        self.st.out("// Start the module loop")
        self.st.out("pthread_create(&(obj->moduleThread), 0, RunModule, obj);")
        self.st.out("")
        self.st.out("// Make this object persistent")
        self.st.out("obj->Ref();")
        self.st.out("")
        self.st.out("return args.This();")
        self.writeFunctionEnd()
        self.st.out("")

    def writeNodeDestructor(self):
        self.st.out("v8::Handle<v8::Value> " + self.classname + "::NodeDestroy(const v8::Arguments& args) {")
        self.st.inc_indent()
        self.st.out("v8::HandleScope scope;")
        self.st.out("className* obj = ObjectWrap::Unwrap<className>(args.This());")
        #self.st.out("bool destroy = obj->Destroy();")
        self.st.out("return scope.Close(v8::Boolean::New(obj->Destroy()));")
        self.writeFunctionEnd()
        self.st.out("")
        self.st.out("bool " + self.classname + "::Destroy() {")
        self.st.inc_indent()
        self.st.out("bool canDestroy = true;")
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.OUT:
                self.st.out("if (canDestroy) {")
                self.st.inc_indent()
                self.st.out("pthread_mutex_lock(&writeMutex" + port_name + ");")
                self.st.out("if (!writeBuf" + port_name + ".empty())")
                self.st.inc_indent()
                self.st.out("canDestroy = false;")
                self.st.dec_indent()
                self.st.out("pthread_mutex_unlock(&writeMutex" + port_name + ");")
                self.writeFunctionEnd()
        self.st.out("if (canDestroy) {")
        self.st.inc_indent()
        self.st.out("pthread_cancel(moduleThread);")
        self.st.out("Unref();")
        self.st.out("return true;")
        self.writeFunctionEnd()
        self.st.out("else {")
        self.st.inc_indent()
        self.st.out("pthread_mutex_lock(&destroyMutex);")
        self.st.out("DestroyFlag = true;")
        self.st.out("pthread_mutex_unlock(&destroyMutex);")
        self.st.out("return true; // return true anyway?")
        self.writeFunctionEnd()
        self.writeFunctionEnd()
        self.st.out("")

    def writeNodeRun(self):
        self.st.out("static void* RunModule(void* object) {")
        self.st.inc_indent()
        self.st.out("static_cast<className*>(object)->Run();")
        self.writeFunctionEnd()
        self.st.out("")

    def writePortInit(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                self.st.out("pthread_mutex_init(&(obj->readMutex" + port_name + "), NULL);")
                self.st.out("")
            if port_direction == rur.Direction.OUT:
                self.st.out("pthread_mutex_init(&(obj->writeMutex" + port_name + "), NULL);")
                self.st.out("uv_async_init(uv_default_loop() , &(obj->async" + port_name + "), &(obj->CallBack" + port_name + "));")
                self.st.out("")

    def writePorts(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                #self.writePortFunctionSignature(p) Can't use this
                self.st.out(param_type + "* " + self.classname + "::read" + port_name + "(bool blocking_dummy) {")
                self.st.inc_indent()
                self.st.out("pthread_mutex_lock(&destroyMutex);")
                self.st.out("bool destroy = DestroyFlag;")
                self.st.out("pthread_mutex_unlock(&destroyMutex);")
                self.st.out("if (destroy)")
                self.st.inc_indent()
                if (param_kind == idltype.tk_sequence):
                    self.st.out("return &readVal" + port_name + ";")
                    self.st.dec_indent()
                    self.st.out("pthread_mutex_lock(&readMutex" + port_name + ");")
                    self.st.out("if (!readBuf" + port_name + ".empty()) {")
                    self.st.inc_indent()
                    self.st.out("readVal" + port_name + ".swap(readBuf" + port_name + ".front());")
                    self.st.out("readBuf" + port_name + ".pop_front();")
                    self.writeFunctionEnd()
                    self.st.out("pthread_mutex_unlock(&readMutex" + port_name + ");")
                    self.st.out("return &readVal" + port_name + ";")
                else:
                    self.st.out("return NULL;")
                    self.st.dec_indent()
                    self.st.out("pthread_mutex_lock(&readMutex" + port_name + ");")
                    self.st.out("if (readBuf" + port_name + ".empty()) {")
                    self.st.inc_indent()
                    self.st.out("pthread_mutex_unlock(&readMutex" + port_name + "); // Don't forget to unlock!")
                    self.st.out("return NULL;")
                    self.writeFunctionEnd()
                    self.st.out("readVal" + port_name + " = readBuf" + port_name + ".front();")
                    self.st.out("readBuf" + port_name + ".pop_front();")
                    self.st.out("pthread_mutex_unlock(&readMutex" + port_name + ");")
                    self.st.out("return &readVal" + port_name + ";")
                self.writeFunctionEnd()
                self.st.out("")
                
            if port_direction == rur.Direction.OUT:
                #self.writePortFunctionSignature(p) Can't use this
                self.st.out("bool " + self.classname + "::write" + port_name + "(const " + param_type + " " + param_name + ") {")
                self.st.inc_indent()
                self.st.out("pthread_mutex_lock(&destroyMutex);")
                self.st.out("bool destroy = DestroyFlag;")
                self.st.out("pthread_mutex_unlock(&destroyMutex);")
                self.st.out("if (destroy)")
                self.st.inc_indent()
                self.st.out("return false;")
                self.st.dec_indent()
                self.st.out("pthread_mutex_lock(&writeMutex" + port_name + ");")
                self.st.out("writeBuf" + port_name + ".push_back(output);")
                self.st.out("pthread_mutex_unlock(&writeMutex" + port_name + ");")
                self.st.out("async" + port_name + ".data = (void*) this;")
                self.st.out("uv_async_send(&async" + port_name + ");")
                self.st.out("return true;")
                self.writeFunctionEnd()
                self.st.out("")

    def writeNodePorts(self):
        for p in self.portList:
            port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(p)
            if port_direction == rur.Direction.IN:
                # Port IN, so in javascript you write to it
                self.st.out("v8::Handle<v8::Value> " + self.classname + "::NodeWrite" + port_name + "(const v8::Arguments& args) {")
                self.st.inc_indent()
                self.st.out("v8::HandleScope scope;")
                self.st.out("className* obj = ObjectWrap::Unwrap<className>(args.This());")
                #self.st.out("//type=" + param_type + " kind=" + str(param_kind))
                if param_kind == idltype.tk_sequence:
                    self.st.out("if (args.Length() < 1 || !args[0]->IsArray())")
                    self.st.inc_indent()
                    self.st.out("return scope.Close(v8::Boolean::New(false)); // Could also throw an exception")
                    self.st.dec_indent()
                    self.st.out("v8::Handle<v8::Array> arr = v8::Handle<v8::Array>::Cast(args[0]);")
                    self.st.out("unsigned int len = arr->Length();")
                    #self.st.out(param_type + " readVec(0);")
                    self.st.out("pthread_mutex_lock(&(obj->readMutex" + port_name + "));")
                    #self.st.out("obj->readBuf" + port_name + ".push_back(readVec);")
                    self.st.out("obj->readBuf" + port_name + ".resize(obj->readBuf" + port_name + ".size()+1);")
                    self.st.out(param_type + "& readVec = obj->readBuf" + port_name + ".back();")
                    self.st.out("for (unsigned int i=0; i<len; ++i)")
                    self.st.inc_indent()
                    #self.st.out("obj->readBuf" + port_name + ".back().push_back(arr->Get(v8::Integer::New(i))->NumberValue());")
                    self.st.out("readVec.push_back(arr->Get(v8::Integer::New(i))->NumberValue());")
                    self.st.dec_indent()
                else:
                    self.st.out("if (args.Length() < 1)")
                    self.st.inc_indent()
                    self.st.out("return scope.Close(v8::Boolean::New(false)); // Could also throw an exception")
                    self.st.dec_indent()
                    self.st.out("pthread_mutex_lock(&(obj->readMutex" + port_name + "));")
                    self.st.out("obj->readBuf" + port_name + ".push_back(args[0]->NumberValue());")
                self.st.out("pthread_mutex_unlock(&(obj->readMutex" + port_name + "));")
                self.st.out("return scope.Close(v8::Boolean::New(true));")
                self.writeFunctionEnd()
                self.st.out("")
                
            if port_direction == rur.Direction.OUT: 
                # Port OUT, so in javascript you read from it
                self.st.out("v8::Handle<v8::Value> " + self.classname + "::NodeRegRead" + port_name + "(const v8::Arguments& args) {")
                self.st.inc_indent()
                self.st.out("v8::HandleScope scope;")
                self.st.out("className* obj = ObjectWrap::Unwrap<className>(args.This());")
                self.st.out("if (args.Length() < 1 || !args[0]->IsFunction())")
                self.st.inc_indent()
                self.st.out("return scope.Close(v8::Boolean::New(false)); // Could also throw an exception")
                self.st.dec_indent()
                self.st.out("v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(args[0]);")
                self.st.out("obj->nodeCallBack" + port_name + " = v8::Persistent<v8::Function>::New(callback);")
                self.st.out("return scope.Close(v8::Boolean::New(true));")
                self.writeFunctionEnd()
                self.st.out("")
                
                # The callback function that calls the javascript callback
                self.st.out("void " + self.classname + "::CallBack" + port_name + "(uv_async_t *handle, int status) {")
                self.st.inc_indent()
                self.st.out("v8::HandleScope scope;")
                self.st.out("className* obj = (className*)(handle->data);")
                self.st.out("const unsigned argc = 1;")
                self.st.out("while (true) {") # begin while loop
                self.st.inc_indent()
                self.st.out("pthread_mutex_lock(&(obj->writeMutex" + port_name + "));")
                self.st.out("if (obj->writeBuf" + port_name + ".empty()) {")
                self.st.inc_indent()
                self.st.out("pthread_mutex_unlock(&(obj->writeMutex" + port_name + ")); // Don't forget to unlock!")
                self.st.out("break;")
                self.writeFunctionEnd()
                if param_kind == idltype.tk_sequence:
                    self.st.out("unsigned int len;")
                    self.st.out("v8::Handle<v8::Array> arr;")
                    self.st.out("if (!obj->nodeCallBack" + port_name + ".IsEmpty()) {")
                    self.st.inc_indent()
                    self.st.out("len = obj->writeBuf" + port_name + ".front().size();")
                    self.st.out("arr = v8::Array::New(len);")
                    self.st.out(param_type + "& vec = obj->writeBuf" + port_name + ".front();")
                    self.st.out("for (unsigned int i=0; i<len; ++i)")
                    self.st.inc_indent()
                    self.st.out("arr->Set(v8::Integer::New(i), v8::Number::New(vec[i]));")
                    self.st.dec_indent()
                    self.writeFunctionEnd()
                    self.st.out("obj->writeBuf" + port_name + ".pop_front();")
                    self.st.out("pthread_mutex_unlock(&(obj->writeMutex" + port_name + "));")
                    self.st.out("if (!obj->nodeCallBack" + port_name + ".IsEmpty()) {")
                    self.st.inc_indent()
                    self.st.out("v8::Local<v8::Value> argv[argc] = { v8::Local<v8::Value>::New(arr) };")
                    self.st.out("obj->nodeCallBack" + port_name + "->Call(v8::Context::GetCurrent()->Global(), argc, argv);")
                    self.writeFunctionEnd()
                else:
                    self.st.out("v8::Local<v8::Value> argv[argc] = { v8::Local<v8::Value>::New(v8::Number::New(obj->writeBuf" + port_name + ".front())) };")
                    self.st.out("obj->writeBuf" + port_name + ".pop_front();")
                    self.st.out("pthread_mutex_unlock(&(obj->writeMutex" + port_name + "));")
                    self.st.out("if (!obj->nodeCallBack" + port_name + ".IsEmpty())")
                    self.st.inc_indent()
                    self.st.out("obj->nodeCallBack" + port_name + "->Call(v8::Context::GetCurrent()->Global(), argc, argv);")
                    self.st.dec_indent()
                self.writeFunctionEnd() # end of while loop
                self.st.out("pthread_mutex_lock(&(obj->destroyMutex));")
                self.st.out("bool destroy = obj->DestroyFlag;")
                self.st.out("pthread_mutex_unlock(&(obj->destroyMutex));")
                self.st.out("if (destroy)")
                self.st.inc_indent()
                self.st.out("obj->Destroy();")
                self.st.dec_indent()
                self.writeFunctionEnd()
                self.st.out("")

    def writeBufInitiation(self, node):
        port, port_name, port_direction, param_name, param_type, param_kind = self.getPortConfiguration(node)
        if port_direction == rur.Direction.IN:
            #self.st.out( "readBuf" + port_name + " = " + param_type + "_node(0);")
            self.st.out("readBuf" + port_name + " = " + "std::deque<" + param_type + ">(0);")
            self.st.out("readVal" + port_name + " = " + param_type + "(0);")
        if port_direction == rur.Direction.OUT:
            #self.st.out( "writeBuf" + port_name + " = " + param_type + "_node(0);")
            self.st.out("writeBuf" + port_name + " = " + "std::deque<" + param_type + ">(0);")

# Initialize this parser
def run(tree, args):
    st = output.Stream(sys.stdout, 2)
    dv = StandardVisitor(st)
    tree.accept(dv)
    # And then write everything to the to-be-generated header file
    dv.writeAll()

