#include <ZmqModuleExt.h>

#include <zmq.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

using namespace rur;
using namespace std;

static int lifetime = 10;

ZmqModuleExt::ZmqModuleExt() {
}

// zmq_ctx_new used to be zmq_init which is by default installed in Ubuntu with apt-get install libzmq-dev
// so you'll need to deinstall that...
void ZmqModuleExt::Prepare() {
	printf("Open ZMQ socket\n");
	context = zmq_ctx_new ();
	responder = zmq_socket (context, ZMQ_REP);
	zmq_bind (responder, "tcp://*:5555");
}

/**
 * This example is from the web, but doesn't seem to work.
 */
void ZmqModuleExt::Tick() {
	printf("One request-respond cycle\n");

	// Wait for next request from client
	zmq_msg_t request;
	zmq_msg_init (&request);
	zmq_msg_recv (&request, responder, 0);
	printf ("Received Hello\n");
	zmq_msg_close (&request);

	// Do some 'work'
	sleep (1);
	printf("Sleep\n");

	// Send reply back to client
	zmq_msg_t reply;
	zmq_msg_init_size (&reply, 5);
	memcpy (zmq_msg_data (&reply), "World", 5);
	zmq_msg_send (&reply, responder, 0);
	zmq_msg_close (&reply);
}

bool ZmqModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	zmq_close (responder);
	zmq_ctx_destroy (context);
	return true;
}


