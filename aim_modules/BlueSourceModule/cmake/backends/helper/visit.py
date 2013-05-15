from omniidl import idlast, idltype, idlutil, idlvisitor, output

class Visit (idlvisitor.AstVisitor, idlvisitor.TypeVisitor):

    # The user is able to define its own data structures to be used on the channels 
    structList = []
    
    # Every method corresponds to a port/channel
    portList = []
    
    # Typedef list
    typedefList = []

    # Pragma list
    pragmaList = [] 

    # The module's name
    classname = ''
    
    # The namespace
    namespace = ''

##########################################################################################
# Visitor functions
##########################################################################################

    # Use the already build abstract syntax tree and visit all nodes
    def visitAST(self, node):
        # First visit all of the nodes
        for n in node.declarations():
           n.accept(self)
	# Visit all of the pragmas
        for n in node.pragmas():
           self.pragmaList.append(n)

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
        self.__result_type = type.decl().scopedName()[-1]
#        self.__result_type = self.__prefix + type.decl().scopedName()[-1]

##########################################################################################
# Helper functions
##########################################################################################

    def seqToVec(self, type):
        type.seqType().accept(self)
        result_type = "std::vector<" + self.__result_type + ">"
        return result_type

    def getParamType(self, param):
        self.__prefix = ""
        param.paramType().accept(self)
        param_type = self.__result_type
        return param_type

    def getParamKind(self, param):
        #return param.paramType().kind()
        return param.paramType().unalias().kind()

    def getMemberType(self, member):
        member.memberType().accept(self)
        member_type = self.__result_type
        return member_type

    def getType(self, type):
        type.accept(self)
        result = self.__result_type
        return result

