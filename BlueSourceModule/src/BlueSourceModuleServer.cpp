/**
 * @file BlueSourceModuleServer.cpp
 * @brief 
 *
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
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Jul 26, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string>
#include <BlueSourceModuleServer.h>

BlueSourceModuleServer::BlueSourceModuleServer() {

}

BlueSourceModuleServer::~BlueSourceModuleServer() {

}

// bluetooth.h fix
// not allowed in C++ (taking address of temporary)
// loc_addr.rc_bdaddr = (*BDADDR_ANY);
const bdaddr_t BDADDR_ANY_INITIALIZER [] = {0, 0, 0, 0, 0, 0};

void BlueSourceModuleServer::Init() {
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };

    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY_INITIALIZER;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(s, 1);

    // accept one connection
    client = accept(s, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));
}

int BlueSourceModuleServer::Read(std::string &buffer) {
    // read data from the client
    int bytes_read = read(client, buf, sizeof(buf));
    if( bytes_read > 0 ) {
        printf("received [%s]\n", buf);
    }
    buffer = std::string(buf);
    return bytes_read;
}

void BlueSourceModuleServer::Close() {
    // close connection
    close(client);
    close(s);
}
