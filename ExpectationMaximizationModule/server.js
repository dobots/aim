var nodeModule = require('./build/Release/ExpectationMaximizationModule');
//var nodeModule = require('./build/Debug/ExpectationMaximizationModule');

var obj = new nodeModule.ExpectationMaximizationModule();

var modelCallBack = function(msg){
	console.log("model:");
	console.log(msg);
	classify();
};
obj.RegReadModel(modelCallBack);

var classifyCallBack = function(msg){
	console.log("classification:");
	console.log(msg);
};
obj.RegReadClassified(classifyCallBack);


// Train the model
var arr = new Array(2);
var avg = new Array(2);

// Cluster 1
avg[0] = 0;
avg[1] = 5;

for (var i=0; i<30; i++) {
	arr[0] = avg[0] + Math.floor(Math.random() * 10) - 5
	arr[1] = avg[1] + Math.floor(Math.random() * 10) - 5
	obj.WriteSample(arr);
}

// Cluster 2
avg[0] = avg[0]+15.0;
avg[1] = avg[1]+15.0;

for (var i=0; i<30; i++) {
	arr[0] = avg[0] + Math.floor(Math.random() * 10) - 5
	arr[1] = avg[1] + Math.floor(Math.random() * 10) - 5
	obj.WriteSample(arr);
}

var train = function() {
  // Train command, will call modelCallBack()
  obj.WriteTrain(1);
}

var classify = function() {
  // Classify
  arr[0] = avg[0] + Math.floor(Math.random() * 10) - 5
  arr[1] = avg[1] + Math.floor(Math.random() * 10) - 5
  obj.WriteClassify(arr);

  avg[0] = avg[0] - 10.0;
  avg[1] = avg[1] - 10.0;

  arr[0] = avg[0] + Math.floor(Math.random() * 10) - 5
  arr[1] = avg[1] + Math.floor(Math.random() * 10) - 5
  obj.WriteClassify(arr);

  // Stop
  //console.log(obj.Destroy());
  setTimeout(function(){obj.Destroy()},3000);
}

// Give the addon some time to process all training sets
setTimeout(train, 1000);
