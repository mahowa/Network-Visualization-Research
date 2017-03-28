#include <iostream>
#include <utility>
#include "minheap.h"

minheap::minheap(){
	
}

minheap::~minheap(){
	
}

void minheap::Add(unsigned int number, unsigned int weight) {
	std::pair<unsigned int, unsigned int> node(number,weight);
	unsigned int location;

	_heap.push_back(node);
	location = _heap.size();

	while (location > 1) {
		unsigned int parent = location >> 1;
		if (_heap[location - 1].second < _heap[parent - 1].second) {
			std::swap(_heap[location - 1], _heap[parent - 1]);
			location = parent;
		}
		else {
			break;
		}
	}
}

void minheap::RemoveTop() {
	_heap[0] = _heap[_heap.size() - 1];
	_heap.pop_back();
	unsigned int location = 1;

	while (true) {
		unsigned int child1 = location << 1;
		unsigned int child2 = child1 + 1;
		if (child2 > _heap.size()) {
			if (child1 <= _heap.size() && _heap[location - 1].second > _heap[child1 - 1].second)
				std::swap(_heap[location - 1], _heap[child1 - 1]);
			break;
		}
			
		unsigned int smaller = location;

		if (_heap[smaller - 1].second > _heap[child1 - 1].second)
			smaller = child1;
		if (_heap[smaller - 1].second > _heap[child2 - 1].second)
			smaller = child2;
		if (smaller != location)
			std::swap(_heap[location - 1], _heap[smaller - 1]);
		else
			break;

		location = smaller;
	}
}

bool minheap::IsEmpty() {
	return (_heap.size() == 0);
}

std::pair<unsigned int, unsigned int> minheap::Top() {
	return _heap[0];
}


std::ostream& operator<<(std::ostream& output, minheap heap)
{
	unsigned int size = heap._heap.size();
	for (unsigned int index = 0; index < size; index++) {
		output << "index:" << heap._heap[index].first << "weight:" << heap._heap[index].second;
		if (index < size - 1)
			output << ", ";
	}
	return output;
}