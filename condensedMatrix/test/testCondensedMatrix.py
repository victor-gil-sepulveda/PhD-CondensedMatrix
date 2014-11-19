"""
Created on 30/01/2012

@author: victor
"""
import unittest
import random
from pyRMSD.condensedMatrix import CondensedMatrix
import numpy

class TestCondensedMatrix(unittest.TestCase):

    def test_list_creation(self):
        MAX_ELEMENTS = 30
        DATA_LEN = (MAX_ELEMENTS * (MAX_ELEMENTS-1))/2
        numpy_matrix_data = numpy.abs(numpy.random.rand(DATA_LEN))
        np_matrix = CondensedMatrix(numpy_matrix_data)
        ls_matrix = CondensedMatrix(list(numpy_matrix_data))
        self.assertEqual(ls_matrix.row_length, MAX_ELEMENTS)
        self.assertEqual(np_matrix.row_length, MAX_ELEMENTS)
        numpy.testing.assert_almost_equal(numpy_matrix_data, ls_matrix.get_data(), 7)
        numpy.testing.assert_almost_equal(numpy_matrix_data, np_matrix.get_data(), 7)
        numpy.testing.assert_almost_equal(np_matrix.get_data(), ls_matrix.get_data(), 7)
    
    def test_get_number_of_rows(self):
        random.seed()
        for i in range(20): #@UnusedVariable
            rows = random.randint(1,1000)
            number_of_elements =  (rows *(rows-1)) / 2
            matrix = CondensedMatrix(numpy.array(range(number_of_elements))*1.)
            self.assertEqual(rows,matrix.get_number_of_rows())
            self.assertEqual(rows,matrix.row_length)
            self.assertEqual(rows,len(matrix))
     
    
    def test_item_access(self):
        condensed_matrix_1 = CondensedMatrix([1.0, 4.5,7.2, 
                                                  8.5, 4.5, 
                                                       7.8])
         
        condensed_matrix_2 = CondensedMatrix([.0]*6)
         
        complete_matrix = [[0.0, 1.0, 4.5, 7.2],
                           [1.0, 0.0, 8.5, 4.5],
                           [4.5, 8.5, 0.0, 7.8],
                           [7.2, 4.5, 7.8, 0.0]]
         
        row_len = condensed_matrix_1.row_length
         
         
        for i in range(row_len):
            for j in range(row_len):
                condensed_matrix_2[i,j] = complete_matrix[i][j]
         
        ## The access for a complete and a condensed matrix is exactly the same
        for i in range(row_len):
            for j in range(row_len):
                self.assertAlmostEqual(condensed_matrix_1[i,j],complete_matrix[i][j],6) #-> there are some errors, as it stores floats instead
                                                                                        # of doubles     
         
        ## And we can build a condensed matrix as a complete matrix
        self.assertItemsEqual(condensed_matrix_1.get_data(), condensed_matrix_2.get_data())
    
        
if __name__ == "__main__":
    #import sys;sys.argv = ['', 'Test.testName']
    unittest.main()