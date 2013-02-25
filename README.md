# zmqserver

The zmqserver is similar to the YARP name server. There might be something better suited to this in the zeromq code itself, like a "router" option I did not find. The only thing that the zmqserver enables is to have really exogeneous coordination. It is implemented as a node.js server.

Another (C/C++) binary will be implemented that allows you to perform:

* zmqconnect /name0 /name1

This will connect the two processes by a ZMQ construct (REQ/REP) over these two ports/channels if they are known by the zmqserver.
