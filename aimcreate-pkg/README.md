This is a script like roscreate-pkg used in the ROS build system. It creates the proper files which allow you to quickly instantiate a new module. To these belong:

 * template files, such as a Makefile, a CMakeLists.txt file, a cmake directory, and a manifest file to build the module, which will be default reside in /usr/share/rur/template
 * configuration files, which will be by default copied to /etc/rur

It expects previously installed:

 * omniidl builder (sudo apt-get install omniidl)
 * rur-builder, by default in /usr/share/rur/backends

Take a look at the other aimtools, to see if you can use them.
