#ifndef __CONDENSED_TEMPLATE__
#define __CONDENSED_TEMPLATE__

#include <cmath>
#include "DataTypes.h"

#define calc_pos(i,j) (i*row_length_minus_one - i - ((( i-1)*i) >> 1 ) + j - 1)


class C_CondensedMatrix_DummyBase {
	public:
		C_CondensedMatrix_DummyBase(){}
		virtual ~C_CondensedMatrix_DummyBase() {}
		
		void* data;
		DataType d_type;
		unsigned long int data_size;
		unsigned long int row_length;
		unsigned long int row_length_minus_one;
};


template <class T>
class C_CondensedMatrix : public C_CondensedMatrix_DummyBase{

	public:
		C_CondensedMatrix(void* data, DataType d_type, unsigned int data_size){
			this->data_size = data_size;
			this->row_length = (unsigned long int) (1 + sqrt(1+8*this->data_size))/2;
			this->row_length_minus_one = row_length - 1;
			this->d_type = d_type;
		}
		
		void get_element_at(long i, long j, T* element){
			if (i==j){
				*element = 0;
			}
			else{
				if (i < j){
					*element = ((T*) data)[calc_pos(i,j)];
				}
				else{
					*element = ((T*) data)[calc_pos(j,i)];
				}
			}
		}
		
		void set_element_at(long i, long j, T element){
			if (i < j){
				((T*) data)[calc_pos(i,j)] = element;
			}
			else{
				((T*) data)[calc_pos(j,i)] = element;
			}
		}
		
};

#endif

