#ifndef __NEIGH_FUNCTIONS__
#define __NEIGH_FUNCTIONS__

namespace NeighbourFunctions{
	
	template<class T>
	void get_neighbors_for_node(const C_CondensedMatrix<T>* matrix, double cutoff, 
			unsigned int len_nodes_remaining, int* nodes_remaining, vector<int>& neighbours){

		int pos,j;
		for(int i = 0; i < len_nodes_remaining; ++i){
			j = nodes_left[i];
			if(node<j){
				pos = calc_vector_pos(node,j,self); // *************
				if(self->data[pos]<=cutoff){
					neighbours.push_back(j);
				}
			}
			if(node>j){
				pos = calc_vector_pos(j,node,self); //*****************
				if(self->data[pos]<=cutoff){
					neighbours.push_back(j);
				}
			}
		}

	}
	
	template<class T>
	int* get_neighbors_of_node_for_radius(const C_CondensedMatrix<T>* matrix, ){
		

	}
	
	void choose_node_with_higher_cardinality(const C_CondensedMatrix<T>* matrix, double cutoff){
		//Do the job
		vector<int> neighbors(len_nodes,0);
		int inode, jnode, pos;
		double value;
		for (int i = 0; i< len_nodes-1; ++i){
			inode = nodes[i];
			for (int j = i+1; j < len_nodes; ++j){
				jnode = nodes[j];
				pos = calc_vector_pos(inode, jnode, self);
				value = self->data[pos];
				if(value <= cutoff){
					neighbors[i] += 1;
					neighbors[j] += 1;
				}
			}
		}
		
		//Get maximum value
		int max =  *(max_element(neighbors.begin(),neighbors.end()));
		// Get the index that corresponds to that value
		// cout<<"DBG: Looking for "<<max<<endl;
		int index = 0;
		for (int i = 0; i < len_nodes; ++i){
			if (neighbors[i]==max){
				index = i;
				break;
			}
		}
	}
	
	
};

#endif