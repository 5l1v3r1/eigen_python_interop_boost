#define WRAP_PYTHON 1
#if WRAP_PYTHON
#include <Python.h>
#include <boost/python.hpp>
#include <numpy/arrayobject.h>
#endif 

#include <iostream>
using namespace std;

#include <Eigen/Core>
#include <Eigen/Array>
using namespace Eigen;

class FooClass
{
public:
	FooClass( int new_m );
	~FooClass();
	
	template<typename Derived>
	int foo(const MatrixBase<Derived>& barIn, MatrixBase<Derived>& barOut);
#if WRAP_PYTHON
	int foo_python(PyObject* barIn, PyObject* barOut);
#endif
private:
	int m;
};

FooClass::FooClass( int new_m ){
	m = new_m;
}
FooClass::~FooClass(){
}

template<typename Derived>
int FooClass::foo(const MatrixBase<Derived>& barIn, MatrixBase<Derived>& barOut){
	barOut = barIn*3.0;  // Some trivial placeholder computation.
}

#if WRAP_PYTHON
int FooClass::foo_python(PyObject* barIn, PyObject* barOut){
	Map<VectorXd> _barIn((double *) PyArray_DATA(barIn),m);
	Map<VectorXd> _barOut((double *) PyArray_DATA(barOut),m);
	return foo(_barIn, _barOut);
}
using namespace boost::python;
BOOST_PYTHON_MODULE(_FooClass)
{
    class_<FooClass>("FooClass", init<int>(args("m")))
        .def("foo", &FooClass::foo_python)
    ;
}
#endif