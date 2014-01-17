---
layout: default
title: Introduction
---

# Dodedodo - Artificial Intelligence for you!

[Dodedodo](http://www.dodedodo.com/) is an "internet of things" infrastructure. More than other software packages it provides machine learning and artificial intelligence algorithms for everyone to use in their applications. Why the internet of things? We would like answer that question with a few questions of our own:

* Why would you want to come home to a cold house?
* Why would you want to turn on the lights yourself?
* Why would you want to forget to turn off the heating?
* Why would you want to clean your house yourself?
* Why would you not want to know if your grandma fell?
* Why would you not want to know if there is a burglar in your home?
* Why would you want to have dead plants?
* Why would you not want to find out that your dog hurt itself by accident?
* Why do you want to forget groceries in the supermarket?
* Why do you want to forget your suitcase in the train?

The need of new technology might not always be visible beforehand, however, actively withholding yourself from technology might have undesired side-effects. We want to give you the power of this type of pervasive technology. You want to be able to use those services, and you want to decide where your data goes, and how to process it. We would like to take you on a journey with us, to enable you to do so.

## Infrastructure

The Dodedodo infrastructure exists out of a rich toolset. It is not created as a stand-alone library or a single API and it must much more be seen as an ecosystem. It takes its inspiration from robotic platforms such as [YARP](http://eris.liralab.it/yarp) (Yet Another Robot Platform) and [ROS](http://www.ros.org) (Robot Operating System), from online application repositories such as [Google play](https://play.google.com) and [Launchpad](https://launchpad.net/~dobots/+archive/ppa).

The Dodedodo infrastructure consists of:

* A visual, web-based **drag-and-drop interface** at [www.dodedodo.com](http://www.dodedodo.com/)
* An infrastructure for **real-time streaming** between devices (using at this moment XMPP).
* An application (coined rur-builder) that allows you to run the same code on **Android, Raspberry PI, Docker, or Ubuntu** and which also allows you a wide set of **communication schemes** (middlewares) on those platforms.
* A tool set for **module management and scheduling**. 

The code can be found at [AIM on GitHub](https://github.com/dobots/aim).

## Drag-and-drop Interface

The visual interface at [www.dodedodo.com](http://www.dodedodo.com/) is not required to be used. However, it is a very attractive way to quickly setup an entire infrastructure of modules across a lot of devices. As a tongue-in-cheek example, you can see this Christmas tree that sends out a message in Morse code, which is picked up by an Android smartphone.

Imagine how much work that would be without an infrastructure like Dodedodo. Things like NAT traversal to cross a firewall, starting the modules in the right sequence, cross-compiling code for the Raspberry PI and the Android phone. Using the [Dodedodo](http://www.dodedodo.com/) infrastructure you can concentrate just on building the algorithms themselves.

## Real-time streaming

The infrastructure used by Dodedodo is not reinvented. Dodedodo uses XMPP, a well established protocol that has plenty of extensions for specific purposes. This means that you can manage your applications by sending chat messages! The other neat property of using well established solutions such as XMPP is that security is baked in from the start.

## Cross-platform support

The cross-platform support is delivered by the rur-builder application (see [AIM on GitHub](https://github.com/dobots/aim)). Currently rur-builder provides functionality for different middlewares. Supported are:

* **YARP**, Yet Another Robot Platform, a middleware on top of TCP, UDP, and a few other options
* **ROS**, Robotic Operating System, the most widely used robotic software ecosystem
* **ZeroMQ**, a middleware that adds neat abstractions for distributed communication structures
* **Android**, the well-known virtual machine where application communicate with "intents"
* **NodeJS**, the software that together with npm shook up the javascript world

The rur-builder allows you to write exactly the same code, in C++, and it runs smoothly in all the scenarios of above. For example, if you want to use a module that does "unsupervised learning", say "k-means clustering", you do not have to change anything to run it on Android and communicate to it using "intents", or to run it as a module in your Node server installing it via npm and communicating with it using javascript function calls.

### Module Management and Scheduling

Managing of modules can be done with the so-called **aim tools**. This is a suite of tools in the same spirit as for example roscreate-pkg in ROS. For more information type in **aim help**.

* **aim browse**: search for repositories with AIM modules
* **aim create-pkg**: create a new AIM module
* **aim connect**: connect two ports on running modules
* **aim copy**: copy a module
* **aim get**: add a github repository to your AIM workspace
* **aim init**: create your own github repository in your AIM workspace
* **aim make**: compile the module
* **aim register**: register the module locally
* **aim run**: run a previously registered module
* **aim select**: select a specific middleware to compile for
* **aim stop**: stop the module
* **aim update**: update a module to a newer version of AIM

## Support {#support}

You would like to know more? Check out the company providing all this open-source software [Distributed Organisms B.V.](http://www.dobots.nl), a spinoff of the Dutch research institute [Almende](http://www.almende.com). The github repository of DO is called [dobots](https://github.com/dobots). You can also checkout the repositories of individual authors [Bart van Vliet](https://github.com/vliedel), [Dominik Egger](https://github.com/eggerdo) and [Anne van Rossum](https://github.com/mrquincle).

## Open source {#opensource}

Our software is offered as open-source software under LGPLv3 or Apache license. Distributed Organisms B.V. (or Dobots) is an Almende spin-off. This group uses this software in all kind of commercial products, which allows us to provide it for free to the community at large.

