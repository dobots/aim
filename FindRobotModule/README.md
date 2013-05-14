# FindRobotModule

This module uses a camera to find a robot as quick as possible. It is refactored to use a v4l2 driver instead of a v4l(1) one. The code in the folder camera is henceforth one of the ugliest in the universe. On the other hand, it actually compiles and works fine for 64-bit Intel as well as Blackfin architectures.

## Functionality

This module is not functional yet. It decomposes an image in multiple patches. This simple detector will subsequently just use a histogram based method to match against a dictionary with patches. The search method that will be implemented is so-called ["Efficient Subwindow Search"](http://eprints.pascal-network.org/archive/00005516/01/lampert09a.pdf). This is actually quite a smart search method that allows for multi-scale efficient search. The parameter space is split in a hierarchy manner and entire branches of the hierarchy can be disregarded. This through using bounds that define the "maximum" quality that can be obtained from a certain region.

## Copyrights
The copyrights (2013) belong to:

- Author: Anne van Rossum
- Almende B.V., http://www.almende.com and DO bots B.V., http://www.dobots.nl
- Rotterdam, The Netherlands

