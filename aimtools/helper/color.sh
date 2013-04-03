#!/bin/bash

################################################################################
# Enables colors
################################################################################

# Example usage:
# echo -e ${RedF}This text will be red!${Reset}
# echo -e ${BlueF}${BoldOn}This will be blue and bold!${BoldOff} - and this is just blue!${Reset}
# echo -e ${RedB}${BlackF}This has a red background and black font!${Reset}and everything after the reset is normal text!
colors() {
	Escape="\033";
	BlackF="${Escape}[30m"; RedF="${Escape}[31m";   GreenF="${Escape}[32m";
	YellowF="${Escape}[33m";  BlueF="${Escape}[34m";    PurpleF="${Escape}[35m";
	CyanF="${Escape}[36m";    WhiteF="${Escape}[37m";
	BlackB="${Escape}[40m";     RedB="${Escape}[41m";     GreenB="${Escape}[42m";
	YellowB="${Escape}[43m";    BlueB="${Escape}[44m";    PurpleB="${Escape}[45m";
	CyanB="${Escape}[46m";      WhiteB="${Escape}[47m";
	BoldOn="${Escape}[1m";      BoldOff="${Escape}[22m";
	ItalicsOn="${Escape}[3m";   ItalicsOff="${Escape}[23m";
	UnderlineOn="${Escape}[4m";     UnderlineOff="${Escape}[24m";
	BlinkOn="${Escape}[5m";   BlinkOff="${Escape}[25m";
	InvertOn="${Escape}[7m";  InvertOff="${Escape}[27m";
	Reset="${Escape}[0m";
}

colors

msg_error() {
	echo -e ${RedF}"[#] $(date +"%x %R") - Error: $1"${Reset}
}

msg_warning() {
	echo -e ${YellowF}"[#] $(date +"%x %R") - Warning: $1"${Reset}
}

msg_info() {
	echo -e ${GreenF}"[#] $(date +"%x %R") - Info: $1"${Reset}
}

msg_debug() {
	echo -e ${BlueF}"[#] $(date +"%x %R") - Debug: $1"${Reset}
}

