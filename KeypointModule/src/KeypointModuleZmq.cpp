#include <KeypointModuleZmq.h>

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

using namespace rur;


KeypointModuleZmq::KeypointModuleZmq() {
}

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that...
void KeypointModuleZmq::Prepare() {
	context = zmq_ctx_new ();
	responder = zmq_socket (context, ZMQ_REP);
	zmq_bind (responder, "tcp://*:5555");
}

void KeypointModuleZmq::Tick() {
while(1) {
	 // Wait for next request from client
	zmq_msg_t request;
	zmq_msg_init (&request);
	zmq_msg_recv (&request, responder, 0);
	printf ("Received Hello\n");
	zmq_msg_close (&request);

	// Do some 'work'
	sleep (1);

	// Send reply back to client
	zmq_msg_t reply;
	zmq_msg_init_size (&reply, 5);
	memcpy (zmq_msg_data (&reply), "World", 5);
	zmq_msg_send (&reply, responder, 0);
	zmq_msg_close (&reply);
}
}

bool KeypointModuleZmq::Stop() {
	zmq_close (responder);
	zmq_ctx_destroy (context);
	return true;
}


