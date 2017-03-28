#include <vector>
#include <iostream>
#include <utility>


#ifndef minheap_H
#define minheap_H

class minheap{
	private:
		//member variables
		std::vector<std::pair<unsigned int, unsigned int> > _heap;
		
	public:
		//constructor and destructor
		minheap();
		~minheap();
	
		//public functions
		void Add(unsigned int number, unsigned int weight);
		void RemoveTop();
		bool IsEmpty();
		std::pair<unsigned int, unsigned int> Top();
		
		//friend members
		friend std::ostream& operator<<(std::ostream& output, minheap heap);
};
#endif