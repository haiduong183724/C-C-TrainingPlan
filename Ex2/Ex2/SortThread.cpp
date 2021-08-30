#include "SortThread.h"

void SortThread::operator()(int sortArr[], int start, int end)
{
	//int* arrs = new int[end - start + 1];
	//for (int i = 0; i < start - end + 1; i++) {
	//	arrs[i] = sortArr[start + i];
	//}
	
	Sort* s;
	QuickSort qs;
	MergeSort ms;
	s = &qs;
	//clock_t t_start = clock();
	s->sort(sortArr, start, end);
	//printf("\nTime taken: %.9fs\n", (float)(clock() - t_start) / CLOCKS_PER_SEC);
}
