var exec = require('child_process').exec,
    spawn = require('child_process').spawn;

/*
 * GET users listing.
 */
exports.list = function(req, res){
  child = exec("aimlist 0", 
    function (error, stdout, stderr) {
      console.log('stdout: ' + stdout);
      if (error !== null) {
        console.log('exec error: ' + error);
      } else {
        res.writeHead(200, {"Content-Type": "text/plain", "Access-Control-Allow-Origin": "*"});
        res.write(stdout);
	res.end();
      }
  });
};

exports.run = function(req, res){
  var name_s = req.params.name.replace(/[^a-z 0-9]+/gi,'');
  var id_s = req.params.id.replace(/[^a-z 0-9]+/gi,'');
  var parameters = [ name_s, id_s ];
  var aimcmd = "aimrun";
  console.log('execute command: ' + aimcmd + ' with parameters: ' + parameters);

  var child = spawn(aimcmd, parameters);
  child.stdout.on('data', function (data) {
    var data_str = '' + data;
    //data_str = data_str.replace(/(\r\n|\n|\r)/gm,'');
    console.log(data_str);
  });
  child.on('exit', function (code) {
    console.log('child process exited with code ' + code);
  });
  res.writeHead(200, {"Content-Type": "text/plain", "Access-Control-Allow-Origin": "*"});
  res.write("Started");
  res.end();
};

exports.stop = function(req, res){
  var name_s = req.params.name.replace(/[^a-z 0-9]+/gi,'');
  var id_s = req.params.id.replace(/[^a-z 0-9]+/gi,'');
  var parameters = [ "middleware", name_s, id_s ];
  var aimcmd = "aimstop";
  console.log('execute command: ' + aimcmd + ' with parameters: ' + parameters);

  var child = spawn(aimcmd, parameters);
  child.stdout.on('data', function (data) {
    var data_str = '' + data;
    //data_str = data_str.replace(/(\r\n|\n|\r)/gm,'');
    console.log(data_str);
  });
  child.on('exit', function (code) {
    console.log('child process exited with code ' + code);
  });
  res.writeHead(200, {"Content-Type": "text/plain", "Access-Control-Allow-Origin": "*"});
  res.write("Started");
  res.end();
};

