# MultiscaleTuring

This is an implementation of multiscale turing patterns using openFrameworks and OpenCv. Originally based on the implementation [here](http://www.wblut.com/2011/07/13/mccabeism-turning-noise-into-a-thing-of-beauty/).

The basic sketch will run in your browser on [Open Processing](http://www.openprocessing.org/sketch/31195).

Each version of the code (`MultiscaleTuring0` through `MultiscaleTuring3`) represents a new way of thinking about how to optimize the multiscale turing computation. `MultiscaleTuring0` is a direct port from a Processing implementation, while `MultiscaleTuring3` represents an attempt at a hyper-optimized implementation.

`3DMultiscaleTuring` evaluates the multiscale turing algorithm across multiple 2d slices of a 3d space.