var nodeModule = require('./build/Release/MovingAverageModule');
//var nodeModule = require('./build/Debug/MovingAverageModule');

var obj = new nodeModule.MovingAverageModule();
var obj2 = new nodeModule.MovingAverageModule();

var readCallBack = function(msg){ console.log(msg); };
obj.RegReadAverage(readCallBack);
obj2.RegReadAverage(readCallBack);

for (var i=0; i<10; i++) {
	obj.WriteInput(2*i);
	obj2.WriteInput(3*i);
}

//console.log(obj.Destroy());
console.log(obj2.Destroy());
setTimeout(function(){obj.Destroy()},3000);
//setTimeout(function(){obj2.Destroy()},5000);

console.log("----- End of JavaScript -----");

