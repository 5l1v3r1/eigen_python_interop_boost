This is an example of one way to wrap an Eigen-based C++ class using Boost::Python for use with numpy.

Here is an overview of how it works:
1. You write a wrapper function for any member functions that take Eigen Matrices,Vectors,etc... 
	This function will take PyObjects (your numpy arrays) as inputs,
	use a macro to get at the underlying data,
	use Eigen's Map functionality to "turn them into" Eigen arrays,
	and then call the wrapped function using these arrays.
2. You write a bit more code to tell Boost about your class and the functions you are exposing to python.
3. You build the module as a shared library
4. You can either import this directly in your code (you will crash hard if the inputs are incorrect) ~or~
	You write some Python to do typechecking, bounds checking, etc... in __init__.py,
	exporting a "safe" version to the rest of your code.  (the example does this)

All in all, this is definitely overkill for the included example, and hopefully some day there will be a much cleaner way 
of doing this, but this is a place to start, and may even be satisfactory if you are already familiar with Boost::Python.

The code is tested on Debian with Eigen 2.0.12 on Apr. 29, 2010.

Code Dependencies:
python
numpy
boost::python
libeigen2
cmake

Good luck!
Drew Wagner
drewm1980@gmail.com
