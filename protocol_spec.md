---
layout: default
title: Protocol Specification
---

# Protocol Specification

The [module specification](module_spec.html) and the [deployment specification](deployment_spec.html) can be used to have modules in the Dodedodo ecosystem. However, there is another spec required for communicating about modules and between modules. This is called the  **protocol** spec.

## Specification details

The current **protocol specification** is written in such way that it can be interpreted and written by the user. It is plain text, not JSON, which would require the user to add all kind of curly brackets to the commands. Note, that the user does not need to use these commands. A very easy interface hiding these complexities is found on [Dodedodo](http://www.dodedodo.com)..

The protocol is defined over XMPP, a normal XMPP client such as Empathy or Pidgin can be used to send the commands. The syntax is like this:

<table>
<tr>
    <th>Protocol</th>
    <th>Opcode</th>
    <th>Parameters</th>
    <th>Reference</th>
</tr>
<tr>
    <td>AIM</td>
    <td>deploy</td>
    <td>module_description</td>
    <td><a href="#Deploy">Deploy</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>start</td>
    <td>[git/repo/]module id</td>
    <td><a href="#Start">Start</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>connect</td>
    <td>source_JID [git/repo/]module id port_out target_JID [git/repo/]module id port_in</td>
    <td><a href="#Connect">Connect</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>disconnect</td>
    <td>[git/repo/]module id port</td>
    <td><a href="#Disconnect">Disconnect</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>stop</td>
    <td>[git/repo/]module id</td>
    <td><a href="#Stop">Stop</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>uninstall</td>
    <td>[git/repo/]module id</td>
    <td><a href="#Uninstall">Uninstall</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>status</td>
    <td>[[git/repo/]module [id [port]]]</td>
    <td><a href="#Status">Status</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>list</td>
    <td></td>
    <td><a href="#List">List</a></td>
</tr>
<tr>
    <td>AIM</td>
    <td>data</td>
    <td></td>
    <td><a href="#Data">Data</a></td>
</tr>
</table>

### Deploy {#Deploy}

A module is deployed according to the deployment description (in json), see the [deployment specification](deployment_spec.html).

### Start {#Start}

A modoule does not run by default. It is started by this command.

### Connect {#Connect}

A module is (different from an agent) unaware of the modules it is connected to. This scheme is called \"exogenous coordination\". It means that an external entity connects the ports on modules to each other. This comes with the benefit that there no programming logic ends up within each agent about how to find other agents, how to connect to them, etcetera.

### Stop {#Stop}

A module can be stopped. It will be deallocated and requires starting before it can do anything again.

### Uninstall {#Uninstall}

The counterpart of deployment.

### Status {#Status}

Status messages can be send on a general level, on the level of a module name, the level of a module instance, or the level of a module port.

### List {#List}

Knowing the list of modules that are available to you.

### Data {#Data}

The format of the data send over XMPP. Note that this is not the only method to communicate between modules. 

    AIM data int module id port data
    AIM data intarray module id port type nArrays nDim sizeD1 sizeD2 .. data
    AIM data float module id port data
    AIM data floatarray module id port type nArrays nDim sizeD1 sizeD2 .. data
    AIM data string module id port some text here

The arrays can be given an N-dimensional structure (as a tensor) with nDim the number of dimensions, and subsequently the \"size\" of each finite array in that dimensions. The data is expected to be with D1 as the outer loop, going inwards with D2, etc.


