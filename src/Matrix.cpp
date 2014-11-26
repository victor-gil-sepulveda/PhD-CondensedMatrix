/*
 * 1 - Crear un objecto c++ templated para la matriz
 * 2 - Hacer que esto use uno de los objetos dependiendo del tipo (como un factory)
 * 3 - Hacer lo mismo para las estadísticas y los neighbours!
 */

#include <Python.h>
#include <structmember.h>
#include <numpy/arrayobject.h>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <vector>

#include "Statistics.h"
#include "CondensedMatrix/CondensedMatrix.h"
#include "CondensedMatrix/DataTypes.h"
using namespace std;

typedef struct {
    PyObject_HEAD
    
    // The condensed matrix itself
    C_CondensedMatrix_DummyBase* matrix;
    
    // Data
    PyObject* numpy_array;
    unsigned long int data_size;
    unsigned long int row_length;
    
    
    // Statistics
    StatisticsCalculator* statisticsCalculator;
   
} CondensedMatrix;

/*
 * Object destructor. Only has to free memory used for data storage and the statistics calculator.
 */
static void condensedMatrix_dealloc(CondensedMatrix* self){

	delete self->matrix;
	
	delete self->statisticsCalculator;
	
	Py_DECREF(self->numpy_array);
	
    self->ob_type->tp_free((PyObject*)self);
}

/*
 *
 */
static PyObject* condensedMatrix_new(PyTypeObject *type, PyObject *args, PyObject *kwds){
	CondensedMatrix* self;
	self = (CondensedMatrix*) type->tp_alloc(type, 0);
    if (self != NULL) {
    	self->matrix = NULL;
    	self->numpy_array = NULL;
    	self->statisticsCalculator = NULL;
    }
    return (PyObject *) self;
}

static int condensedMatrix_init(CondensedMatrix *self, PyObject *args, PyObject *kwds){
	PyObject* input = NULL;

	if (!PyArg_ParseTuple(args, "O",&input)){
		PyErr_SetString(PyExc_ValueError, "[CondensedMatrix] Undefined problem parsing parameters.");
		return -1;
	}

	if (PyArray_Check(input)){
		PyArrayObject* numpy_array = (PyArrayObject*) input;
		self->numpy_array = input;
		
		if ( numpy_array->nd != 1) { // or PyArray_NDIM(PyArrayObject *arr)
			PyErr_SetString(PyExc_ValueError, "[CondensedMatrix] Input numpy array must be a 1D vector.");
			return -1;
		}
		
		// Create the matrix object
		switch (numpy_array->descr->type_num){
			case NPY_INT:
			{
				self->matrix = new C_CondensedMatrix<npy_int>(PyArray_GETPTR1(self->numpy_array,0),
						TYPE_INT,
						PyArray_DIMS(numpy_array)[0]);
			}
			case NPY_LONG:
			{
				self->matrix = new C_CondensedMatrix<npy_long>(PyArray_GETPTR1(self->numpy_array,0),
						TYPE_LONG,
						PyArray_DIMS(numpy_array)[0]);
			}
			break;
			case NPY_FLOAT:
			{
				self->matrix = new C_CondensedMatrix<npy_float>(PyArray_GETPTR1(self->numpy_array,0),
						TYPE_FLOAT,
						PyArray_DIMS(numpy_array)[0]);
			}
			break;
			case NPY_DOUBLE:
			{
				self->matrix = new C_CondensedMatrix<npy_double>(PyArray_GETPTR1(self->numpy_array,0),
						TYPE_DOUBLE,
						PyArray_DIMS(numpy_array)[0]);
			}
			break;
			default:
				PyErr_SetString(PyExc_ValueError, "[CondensedMatrix] Data type not in supported data types ( INT, LONG, FLOAT, DOUBLE). Please recast your array.");
				return -1;
		}
		
		self->row_length = self->matrix->row_length;
		self->data_size = self->matrix->data_size;
		
		// Let's alloc the statistics object
		//self->statisticsCalculator =  new StatisticsCalculator(self->data, self->data_size);
		
		// Increase reference (we are using the array)
		Py_INCREF(self->numpy_array);
	}
	else{
		PyErr_SetString(PyExc_RuntimeError, "[CondensedMatrix] You must use a numpy array to feed the matrix.");
	}

	return 0;
}

char row_length_text[] = "row_length";
char data_size_text[] = "data_size";
char bogus_description_text[] = "TODO";

static PyMemberDef condensedMatrix_members[] = {
    {row_length_text, T_INT, offsetof(CondensedMatrix, row_length), READONLY, PyDoc_STR(bogus_description_text)},
    {data_size_text, T_INT, offsetof(CondensedMatrix, data_size), READONLY, PyDoc_STR(bogus_description_text)},
    {NULL}  // Sentinel 
};

static PyObject* condensedMatrix_get_number_of_rows(CondensedMatrix* self, PyObject *args){
	return Py_BuildValue("i", self->matrix->row_length);
}

static PyObject* condensedMatrix_get_data(CondensedMatrix* self, PyObject *args){
	Py_INCREF(self->numpy_array);
	return  PyArray_Return((PyArrayObject*) self->numpy_array);
}

static PyObject* condensedMatrix_get_data_type(CondensedMatrix* self, PyObject *args){
	return  Py_BuildValue("s", DataType_to_string(self->matrix->d_type).c_str());
}


PyObject* condensedMatrix_subscript(CondensedMatrix *self, PyObject *key){
	unsigned long i = PyInt_AS_LONG(PyTuple_GET_ITEM(key,0));
	unsigned long j = PyInt_AS_LONG(PyTuple_GET_ITEM(key,1));
	
	switch (self->matrix->d_type){
		case TYPE_INT:
		{
			npy_int result;
			dynamic_cast<C_CondensedMatrix<npy_int>* >(self->matrix)->get_element_at(i, j, &result);
			return PyInt_FromLong(result);
		}
		break;
		case TYPE_LONG:
		{
			npy_long result;
			dynamic_cast<C_CondensedMatrix<npy_long>* >(self->matrix)->get_element_at(i, j, &result);
			return PyLong_FromLong(result);
		}
		break;
		case TYPE_FLOAT:
		{
			npy_float result;
			dynamic_cast<C_CondensedMatrix<npy_float>* >(self->matrix)->get_element_at(i, j, &result);
			return PyFloat_FromDouble((double) result);
		}
		break;
		case TYPE_DOUBLE:
		{
			npy_double result;
			dynamic_cast<C_CondensedMatrix<npy_double>* >(self->matrix)->get_element_at(i, j, &result);
			return PyFloat_FromDouble((double) result);
		}
		break;
	}
	return NULL;
}

int condensedMatrix_ass_subscript(CondensedMatrix *self, PyObject *key, PyObject *v){
	unsigned long i = PyInt_AS_LONG(PyTuple_GET_ITEM(key,0));
	unsigned long j = PyInt_AS_LONG(PyTuple_GET_ITEM(key,1));
	
	switch (self->matrix->d_type){
		case TYPE_INT:
		{
			npy_int value = (npy_int) PyInt_AS_LONG(v);
			dynamic_cast<C_CondensedMatrix<npy_int>* >( self->matrix)->set_element_at(i, j, value);
		}
		break;
		case TYPE_LONG:
		{
			npy_long value = (npy_long) PyInt_AS_LONG(v);
			dynamic_cast<C_CondensedMatrix<npy_long>* >( self->matrix)->set_element_at(i, j, value);
		}
		break;
		case TYPE_FLOAT:
		{
			npy_float value = (npy_float) PyFloat_AS_DOUBLE(v);
			dynamic_cast<C_CondensedMatrix<npy_float>* >( self->matrix)->set_element_at(i, j, value);
		}
		break;
		case TYPE_DOUBLE:
		{
			npy_double value = (npy_double) PyFloat_AS_DOUBLE(v);
			dynamic_cast<C_CondensedMatrix<npy_double>* >( self->matrix)->set_element_at(i, j, value);
		}
		break;
	}
	return 0;
}

Py_ssize_t condensedMatrix_length(CondensedMatrix *self){
	return self->matrix->row_length;
}

static PyMethodDef condensedMatrix_methods[] = {
	// Basic field access
	{"get_number_of_rows", (PyCFunction) condensedMatrix_get_number_of_rows, METH_NOARGS,PyDoc_STR("description")},
	{"get_data", (PyCFunction) condensedMatrix_get_data, METH_NOARGS,PyDoc_STR("description")},
	{"get_data_type", (PyCFunction) condensedMatrix_get_data_type, METH_NOARGS,PyDoc_STR("description")},
	
	/*// Statistics
	{"recalculateStatistics", (PyCFunction) condensedMatrix_calculate_statistics, METH_NOARGS,PyDoc_STR("description")},
	{"calculateMean", 		(PyCFunction) condensedMatrix_get_mean, METH_NOARGS,PyDoc_STR("description")},
	{"calculateVariance", 	(PyCFunction) condensedMatrix_get_variance, METH_NOARGS,PyDoc_STR("description")},
	{"calculateSkewness", 	(PyCFunction) condensedMatrix_get_skewness, METH_NOARGS,PyDoc_STR("description")},
	{"calculateKurtosis", 	(PyCFunction) condensedMatrix_get_kurtosis, METH_NOARGS,PyDoc_STR("description")},
	{"calculateMax", 		(PyCFunction) condensedMatrix_get_max, METH_NOARGS,PyDoc_STR("description")},
	{"calculateMin", 		(PyCFunction) condensedMatrix_get_min, METH_NOARGS,PyDoc_STR("description")},

	// Matrix as graph
	{"get_neighbors_for_node", (PyCFunction)condensedMatrix_get_neighbors_for_node, METH_VARARGS,PyDoc_STR("description")},
	{"choose_node_with_higher_cardinality", (PyCFunction)condensedMatrix_choose_node_with_higher_cardinality, METH_VARARGS,PyDoc_STR("description")},
	{"element_neighbors_within_radius",(PyCFunction)condensedMatrix_get_neighbors_of_node_for_radius, METH_VARARGS,PyDoc_STR("description")},
	 *///{"calculate_rw_laplacian", (PyCFunction)condensedMatrix_calculate_rw_laplacian, METH_NOARGS,PyDoc_STR("description")},
	//{"calculate_affinity_matrix", (PyCFunction)condensedMatrix_calculate_affinity_matrix, METH_VARARGS,PyDoc_STR("description")},
	{NULL}  /* Sentinel */
};

static PyMappingMethods pdb_as_mapping = {
    (lenfunc)     	condensedMatrix_length,				/*mp_length*/
    (binaryfunc)	condensedMatrix_subscript,			/*mp_subscript*/
    (objobjargproc)	condensedMatrix_ass_subscript,		/*mp_ass_subscript*/
};

static PyTypeObject CondensedMatrixType = {
    PyObject_HEAD_INIT(NULL)
    0,                         						/*ob_size*/
    "condensedMatrix.CondensedMatrixType",      	/*tp_name*/
    sizeof(CondensedMatrix), 	/*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)condensedMatrix_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    &pdb_as_mapping,           /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT| Py_TPFLAGS_BASETYPE , /*tp_flags*/
    "Condensed matrix as in pdist",           /* tp_doc */
	0,		               					  /* tp_traverse */
	0,		               					  /* tp_clear */
	0,		               					  /* tp_richcompare */
	0,		              					  /* tp_weaklistoffset */
	0,		               	   /* tp_iter */
	0,		               	   /* tp_iternext */
	condensedMatrix_methods,   /* tp_methods */
	condensedMatrix_members,   /* tp_members */
	0,                         /* tp_getset */
	0,                         /* tp_base */
	0,                         /* tp_dict */
	0,                         /* tp_descr_get */
	0,                         /* tp_descr_set */
	0,                         /* tp_dictoffset */
	(initproc)condensedMatrix_init, /* tp_init */
	0,                         		/* tp_alloc */
	condensedMatrix_new,        		/* tp_new */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif


PyMODINIT_FUNC initmatrix(void){
    PyObject* module;

    if (PyType_Ready(&CondensedMatrixType) < 0)
        return;

    module = Py_InitModule3("matrix", NULL,"Fast Access Condensed Matrix");
    if (module == NULL)
          return;

    Py_INCREF(&CondensedMatrixType);
    PyModule_AddObject(module, "CondensedMatrix", (PyObject*) &CondensedMatrixType);

    import_array();
}
