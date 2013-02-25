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

function record(identifier, server, port) {
	var self = this;
	self.identifier = identifier;
	self.server = server;
	self.port = port;
}

function getrecord(list, identifier) {
	for (var i = 0; i < list.length; i++) {
		if (list[i].identifier == identifier) {
			return list[i];
		}
	}
	return null;
}

function resolve(identifier) {
	var rec;
	if ((rec = getrecord(records, identifier)) == null) {
		port++;
		console.log(' * register ' + identifier);
		console.log('   + set ' + identifier + ' ' + host + ':' + String(port));
		rec = new record(identifier, host, String(port));
		records.push(rec);	
	}
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
	var rec = resolve(identifier);
	res.send(rec);
});

//app.listen(nameserver_port);

var socket = zeromq.socket('rep');

socket.identity = 'server [' + process.pid + ']';

/************************************************************************************************************************
 * Actual server using zeromq channels
 ***********************************************************************************************************************/

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
		console.log(socket.identity + ': received ' + str);
		var response = resolve(str);
		console.log(socket.identity + ': sent ' + response);		
		socket.send(response);
	});
});

console.log('Name server for zeromq started at http://' + host + ':' + nameserver_port);

