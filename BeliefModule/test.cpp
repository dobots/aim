#define DECL_STRUCT_FIELD(name,field) \
    _##name##_##field

#define DECL_STRUCT(name) \
    unsigned _##name##_a:2; \
	unsigned _##name##_b:4;

struct Msg{
	DECL_STRUCT(0);
	DECL_STRUCT(1);
};

#define DECL_STRUCT0(field) DECL_STRUCT_FIELD(0,field)
#define DECL_STRUCT1(field) DECL_STRUCT_FIELD(1,field)
#define DECL_STRUCT2(field) DECL_STRUCT_FIELD(2,field)

void decl_str_list(Msg & msg, int i) {

}

//void DECL_STRUCT_LIST(int i) {
//#define DECL_STRUCT_F(field) \
//	switch(i) { \
//	case 0: DECL_STRUCT0(field); break; \
//	} \
//}

#define ONE 0

//template <int t = 3>
void temp() {
	Msg msg;
	const int t = 0;
	for (int i = 0; i < 3; ++i) {
		msg._0_a = 1;
		msg.DECL_STRUCT_FIELD(0,a) = 1;
		msg.DECL_STRUCT_FIELD(ONE,a) = 2;
		msg.DECL_STRUCT_FIELD(t,a) = 3;
		msg.DECL_STRUCT0(a) = 2;
//		msg.DECL_STRUCT_F(a) = 3;

//		DECL_STRUCT0(msg, 0, a) = 3;
	}
}

