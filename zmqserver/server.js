var express	= require('express')
  , app		= module.exports = express.createServer()
  , zeromq	= require('zmq')
  ;

var records = new Array();

var port = 60000;

var host = '127.0.0.1';

var nameserver_port = 10101;

/************************************************************************************************************************
 * General functions
 ***********************************************************************************************************************/

function record(identifier, server, port, pid) {
	var self = this;
	self.identifier = identifier;
	self.server = server;
	self.port = port;
        self.pid = pid;
}

function getrecord(list, identifier) {
	for (var i = 0; i < list.length; i++) {
		if (list[i].identifier == identifier) {
			return list[i];
		}
	}
	return null;
}

function resolve(identifier, pid) {
	var rec;
	if ((rec = getrecord(records, identifier)) == null) {
		port++;
		console.log(' * register ' + identifier);
		console.log('   + set ' + identifier + ' ' + host + ':' + String(port));
		rec = new record(identifier, host, String(port), pid);
		records.push(rec);	
	}
	if (pid != "")
		rec.pid = pid;
	return JSON.stringify(rec);
}

/************************************************************************************************************************
 * Server if would nicely work via routes in RESTful manner
 ***********************************************************************************************************************/

app.get('/', function(req, res) {
	res.send('Nameserver for zeromq');
});

app.get('/resolve/:identifier', function(req, res) {
	var identifier = request.params.identifier;
	var pid = request.params.pid;
	var rec = resolve(identifier, pid);
	res.send(rec);
});


/************************************************************************************************************************
 * Actual server using zeromq channels
 ***********************************************************************************************************************/

// the name server is a "rep" socket, it accepts connections and responds to requests directly.

var socket = zeromq.socket('rep');
socket.identity = 'server [' + process.pid + ']';

socket.bind('tcp://' + host + ':' + nameserver_port, function(err) {
	if (err) throw err;
	console.log('bound!');

	socket.on('message', function(data) { // data is a binary Buffer object in nodejs
//		console.log(socket.identity + ': received ' + data.toString());
		var str = new String();
		var start = 1;
		for (var i = 1; i < data.length - 1 ; i++) {
  			if (data[i] == 47) { // '/'
				start = i;
				break;
			}
		}
		str = data.toString('utf8', start, data.length-1);
		var identifier = str.split(':')[0];
		var pid = str.split(':')[1];
		console.log(socket.identity + ': received ' + str);
		var response = resolve(identifier, pid);
		console.log(socket.identity + ': sent ' + response);		
		socket.send(response);
	});
});

console.log('Name server for zeromq started at http://' + host + ':' + nameserver_port);

