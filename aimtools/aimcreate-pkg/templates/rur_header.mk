########################################################################################
#
#  THIS IS A GLOBAL USER-WIDE RUR CONFIGURATION FILE
#
#  THIS FILE WILL BE REGENERATED ON RUNNING sudo make install FOR aimcreate-pkg
#
########################################################################################

# This file makes it possible to set user-wide specific environmental variables for the
# rur-builder. If you check out a project, adjust this setting if module-specific things
# are needed by copying in the path where the module resides:
#   cp default.mk local.mk
# Then change that file and NOT commit "local.mk" to the repository. It should be only
# used locally. At any times the "default.mk" file will be overwriting by this global 
# configuration file "~/.rur/rur.mk" and which is overwitten again by "local.mk".
# 

# Default backend to be used, it can also be YARP or ROS
BACKEND=YARP
#BACKEND=ROS
#BACKEND=STANDARD

#RUR_BACKENDS depends on your system configuration and should end with something like
# rur-builder/backends.
