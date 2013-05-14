{
  "targets": [
    {
      "target_name": "MovingAverageModule",
      #"type": "executable",
      #"type": "<(library)",
      
      "include_dirs": [
        "/usr/include",
        "aim/inc",
      ],
      
      "cflags": [
        "-std=c++11",
      ],
      
      "ldflags": [
        "-pthread",
      ],
      
      "sources": [
        "aim/inc/MovingAverageModule.cpp",
        "src/MovingAverageModuleExt.cpp",
        "aim/inc/MovingAverageModuleNode.cc",
      ],
    }
  ]
}
