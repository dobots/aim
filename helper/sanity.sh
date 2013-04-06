#!/bin/bash

####################################################################################################
# Configuration
####################################################################################################

# Installation paths, the bin path is to store the application itself, the share path to store 
# additional details such as helper python files, icons, translations. The config path to store 
# configuration details, such as starting at booting.
AIM_CONFIG_PATH=/etc/aim

# Update paths by prepending it with DESTDIR so installation via the Ubuntu PPA works properly. This
# causes a duplicate "//" when $DESTDIR is empty, which is no problem at all, just does not look so
# nice.
AIM_CONFIG_PATH="$DESTDIR/$AIM_CONFIG_PATH"

# Default configuration files
AIM_CONFIG_COLOR=$AIM_CONFIG_PATH/color.sh

####################################################################################################
# Tweaks and general configuration details
####################################################################################################

source $AIM_CONFIG_COLOR

####################################################################################################
# Implementation
####################################################################################################

# Function aim_sanity sanitizes the name that can be used for a module. This is convenient
# for all kind of purposes. a.) It makes the job easier for registration of the module, for
# running the module, and for you for searching all modules on your system by a combination
# of "locate" and "grep Module". b.) It allows for sanitizing commands that can be issued in
# starting binaries on a server. If you enforce (even within a sandbox like Linux Containers)
# that the applications that can be run should end with "Module", this rules out the 
# execution of system binaries such as "sshd", "telnet", etc. and although of course real
# security is required, this is one additional step that would need to be mitigated by a
# potential hacker. The disallowance of spaces in the module name has the same purpose.

aim_sanity() {
	local __resulterr=$2
	local myresulterr=0
	local modulename="$1"

	# Check if there are no whitespaces
	mn_sanity_check=${modulename//[[:space:]]}

	if [[ "$mn_sanity_check" == "$modulename" ]]; then
		if [[ "$modulename" == *Module ]]; then
			myresulterr=0
		else
			msg_warning "End the module with \"Module\""
			myresulterr=1
		fi
	else
		msg_warning "Please, do not use whitespaces"
		myresulterr=2
	fi

	if [[ "$__resulterr" ]]; then
		eval $__resulterr="'$myresulterr'"
	else
		echo "$myresulterr"
	fi
}

