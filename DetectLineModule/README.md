# Detection of lines

A lot computer vision functionality relies on forward and backwards operations. Lines can be formed out of points, and on their turn provide bias for where to search for points. One of the used transforms for detection of lines is the Hough transform.

This module contains an original implementation of the Hough transform. It uses:

* a basic n-dimensional array, only one file, not the python monster :-)
* a file with random functions, which are actually uniform, it has the Robert Floyd algorithm that picks M items out of N
* an extremely simple image data store and manipulation file, using uncompressed BMP, B&W and color, and with basic plotting functions

## Implementation details

Currently only the Randomized Hough Transform is implemented.

The n-dimensional array structure is used for creating a grid, so that only points that are approximately close will be considered as part of one line. There will be too many false matches if this constraint is not applied. In other words, you will probably on a noisy image a point that extends a line segment somewhere on the image. However, you do not want to consider that random point as belong to that line.

The n-dimensional array is also used for the Accumulator structure. The standard data structure used in the Hough transform. 

There are two possibilities of picking a random subset of points that describe a line (two):

* pick a random cell, and then pick a random pair
* pick a random point, and pick another random point

When the density of points is not uniform over the cells, this has vastly different behavior. Cells with only a few points will vote for these points all the time. This is not the desired behaviour. Hence, the second method is implemented in a slightly different flavor. A random point of all points is picked, then a reference to the cell belonging to that point is obtained, and only now a pair from this cell is picked. This will pick every point with the same chance not depending on the discretization at hand.

## Example

You see here an input image of squares. Then the corresponding Hough space, which is extremely sparse. Only a few cells have been "incremented" by the corresponding point pairs. And last, you see an image in which only the lines that are stored in these Hough cells that exceed a given threshold (set here to be 1% of the total number of iterations that increment the accumulator).

<img src="https://github.com/mrquincle/aim_modules/raw/master/DetectLineModule/data/square.bmp" height="320" width="240" title="Input image of squares" margin="10px"/>

<img src="https://github.com/mrquincle/aim_modules/raw/master/DetectLineModule/data/square_accumulator.bmp" height="320" width="240" title="Hough space"/margin="10px">
<img src="https://github.com/mrquincle/aim_modules/raw/master/DetectLineModule/data/square_backprojection.bmp" height="320" width="240" title="Restored image" margin="10px"/>

## To be expected next

There is currently:

* no reasoning on where a line segments ends, each cell in the accumulator contains a vector of potential line segments. These points must be considered as a probability density distribution over segments. The points [0.0,0.0] [0.1,0.1] [0.2,0.2] [0.3,0.3] and [10.0,10.0] [10.1,10.1] [10.2,10.2] [10.3,10.3] are probably to be considered as depicted. Namely, as two line segments. More important, if two lines meet at a given point, it is reasonable to inspect if this correspond to a "corner" rather than an "intersection". Spurious points that extend the lines of the square too much as in the example can be prevented.
* no interaction between line segments and the selection process for the next points. This is more fundamental and requires some thought.


