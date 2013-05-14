{
  "targets": [
    {
      "target_name": "ExpectationMaximizationModule",
      #"type": "executable",
      #"type": "<(library)",
      
      "include_dirs": [
        "/usr/include",
        "aim/inc",
        "/usr/include/opencv",
      ],
      
      "dependencies":[
        # Other binding.gyp
      ],
      
      "cflags": [
        "-std=c++11",
        "-fPIC",
      ],
      
      "libraries": [
        #"-lopencv_core",
        #"-lopencv_ml",
        #"/usr/lib/libopencv_core.so",
        #"/usr/lib/libopencv_ml.so",
        "/opt/ros/groovy/lib/libopencv_core.so",
        "/opt/ros/groovy/lib/libopencv_ml.so",
      ],
      
      "ldflags": [
        "-pthread",
      ],
      
      "sources": [
        "aim/inc/ExpectationMaximizationModule.cpp",
        "src/ExpectationMaximizationModuleExt.cpp",
        "aim/inc/ExpectationMaximizationModuleNode.cc",
      ],
    }
  ]
}
