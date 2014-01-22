---
layout: default
title: Roadmap
---

# Roadmap

The roadmap for [Dodedodo](http://www.dodedodo.com) and the underlying infrastructure is threefold:

* Enabling [monetization](#Monetization) for third parties
* [Modularization](#Modularization) of the gateway
* Incorporation of a [high-bandwidth real-time data transfer](#Bandwidth) technology, eventually using [Eve](http://eve.almende.com/).

## Monetization {#Monetization}

The internet of things is different from the internet as we know it. Thanks to the fact that humans exists within our internet infrastructure, it is possible to **offload** intelligence to the users. However, this will be different if devices start to talk to each other. To be useful these devices need to be **smart**, they cannot afford to remain ignorant. Commerically however, smart algorithms are difficult to sell. Basic functionality of an algorithm does convince a party, not theoretical guarantees or other incredible useful properties for the long-term. Moreover, universities and other institutes provide smart algorithms for free. It is hard to compete as companies with the brains that did specialize in this skill. There is however a very important enabler for smart algorithms, they tend to be **data hungry**. A lot of data is required to make machines learn. The storage of data becomes cheaper and cheaper. However, there is one thing that costs a lot of money. That is the **processing** of data. 

Hence, our business model is to allow third parties to **provide processing** time on their server parks. Of course, a consumer can decide to run algorithms locally or on her own infrastructure for free. Providing the horsepower for intelligent algorithms will however be more convenient for many people. The twist to this cloud computing concept is that we will channel part of the revenues to the developers of the algorithms. This means that their software can be totally open source and still money can be earned through it. Each time an algorithm is used on one of the servers of a cloud provider, a percentage is going to the developer. 


## Gateway Modularization {#Modularization}

Currently the Dodedodo infrastructure uses gateways on devices that enable establishing connections through a firewall (using XMPP). There are several components that are required in such a gateway:

* **Protocol conversion**: the data needs to be converted from the inter-device protocol (e.g. XMPP) to the intra-device protocol (e.g. Intents, or YARP over TCP)
* **Remote control**: the start, stop, connect, deploy, etc. commands as defined in the [protocol spec](protocol_spec.html) need to executed
* **Data forwarding**: data meant for two modules is transferred through a third

Separation of these different components allow for a neat modularization and each component might be treated as just another Dodedodo module. This would lead to more flexibility with respect to inter-device protocol support and a better maintainable ecosystem.

## High-bandwidth Real-Time Data Transport {#Bandwidth}

Currently, in-band XMPP is used for cross-device data transport. This can be extended to higher bandwidth if direct connections between peers can be set up. Several extensions to XMPP support such out-of-band data transport. [Eve](http://eve.almende.com/), an agent platform build in-house, might be used to do this in the end.
