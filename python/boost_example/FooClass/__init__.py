# Provide type-checking in Python where it is nice and easy.
import _FooClass
import numpy

# First step, import the raw classes.
from _FooClass import FooClass

def _typecheck(a):
    assert (type(a) == numpy.ndarray) or (type(a) == numpy.core.memmap), 'Input should be a numpy array or memmap object!'
    assert a.dtype == numpy.dtype('float64'), 'Array needs to be 64 bit floating point!'
    assert a.flags['F_CONTIGUOUS'], 'Array needs to have column major storage!'
    assert a.flags['ALIGNED'], 'Array needs to be aligned!'

def _typecheck_output(a):
    _typecheck(a)
    assert a.flags['WRITEABLE'] # The inputs might not be writeable.

def _typecheckify(unsafeFunction):
    # Take an unsafe verion of a function, and return a typesafe version.
    
    def safeFunction(self, booIn, booOut):
        map(_typecheck, [booIn])  # Add input variables to this list
        map(_typecheck_output, [booOut]) # Add output variables to this list
        
        assert booIn.shape == booOut.shape
        
        # Make the unsafe call
        return unsafeFunction(self, booIn, booOut)
        
    return safeFunction
FooClass.foo = _typecheckify(FooClass.foo)

# Note: This machinery is absolutely overkill for this simple example, but can be handy if
# you are wrapping multiple functions that share an interface containing lots of variables.