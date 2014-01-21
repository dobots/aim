---
layout: default
title: Module Specification
---

# Module Specification

Each Artificial Intelligence Module is specified with a so-called Interface Description Language (IDL) file. A standard compiler (omniORB) is used to check if this file is written according to the IDL specs. The IDL specifications restrict for example the support for data types, so that it becomes easier to do cross-language development. 

The modules in the AIM framework have the following properties:

* The modules are **binary units**. On hand hand, this requires crossing process boundaries (using shared memory, sockets, or other means). On the other hand, this allows very loose coupling between the modules.
* The modules obey a **specificiation** written down as an IDL file. This file will be used a.) during development to generate cross-platform wrapper code, b.) during deployment to know how to connect to the module.

## Specification details

An IDL file describes the module with only that much detail that is necessary for building and deployment within the AIM framework.

    #pragma author               Developer
    #pragma copyright            Organisation
    #pragma date                 Jan 21, 2014
    #pragma license              GNU General Lesser Public Licence
    
    // Recommended namespace "rur"
    module rur {
    
    // A nice name to our class
    interface MovingAverageModule {
    
      // Input, a series of values
      void Input(in long input);
      
      // Output, the moving average
      // backend zeromq
      void Average(out long output);
    
    };
    }
    
### Pragmas

There are several pragmas supported that will be used at several places later on in the build or deployment process.

* pragma author               Developer
* pragma copyright            Organisation
* pragma date                 Date
* pragma license              GNU General Lesser Public Licence

### Module

The keyword **module** indicates the binary unit. It will be cast in the C++ world to a namespace.

### Interface

The interface has a single requirement, it has to end with **Module**.

### Methods

The methods have normal signatures as in for example C++ or Java, but with a twist. There is an additional keyword: **in** or **out**. The methods in the AIM framework are also called **ports** that make this directional property clear. A method is transformed into a function that can be called from C++ code:

    inline int *readInput(bool blocking=true) {
      ...
    }

And:

    bool writeAverage(int *output) {
      ...
    }

Several remarks:

* A read method can be called in a **blocking** or **nonblocking** fashion. If called in a blocking fashion the developer of the module is responsible for being able to read other ports using a threading library. 
* A write method is **nonblocking**, returns true if successful, and dereferences the entity in its argument to be sent. It does not deallocate the entity, so that the user has the ability to reuse parts of it.
* In a rare situation it is possible to write a module to communicate within one middleware and have only a few functions communicating using another middleware. This is indicated in a comment above the method, here **backend zeromq**.

## See also

The IDL module specification is also used for the [deployment specification](deployment_spec.md). This means you can write your proprietary code and deploy it on the Dodedodo architecture if provided with the proper deployment specification.

