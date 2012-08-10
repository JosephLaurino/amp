// amp.cpp : 
//
// + based on C++ AMP sample from http://msdn.microsoft.com/en-us/library/hh265136(v=vs.110).aspx
// + added CPU based parallel_for_each

#include "stdafx.h"

#include <amp.h>
#include <iostream>
#include <array>
#include <ppl.h>

using namespace std;
using namespace concurrency;

void CppAmpMethodWithPPL() {

    const int size = 5;

    std::array<int, size> aCPU = {1, 2, 3, 4, 5}; 
    std::array<int, size> bCPU = {6, 7, 8, 9, 10};
    std::array<int, size> sumCPU; // destination of calculation results 
    
    // Create C++ AMP objects via array_view so we don't have
    // to manually copy the data to and from the GPU
    array_view<const int, 1>    aGPU(size, aCPU); 
    array_view<const int, 1>    bGPU(size, bCPU);
    array_view<int, 1>          sumGPU(size, sumCPU);

    sumGPU.discard_data();

    // run the calculation in parallel on the GPU 
    // keep this very simple since the GPU does not like "branching"
    parallel_for_each( 
        sumGPU.extent, 
        [=](index<1> idx) restrict(amp)          
        {               
            sumGPU[idx] = aGPU[idx] + bGPU[idx]; 
        }                                        
    );
    
    // accessing sumGPU forces a copy of the data back to CPU land
    sumGPU.data(); 
    
    // run the calculation in parallel on multi-core CPU
    // Parallel Patterns Library handles all the threading underneath
    parallel_for_each ( 
        sumCPU.begin(), 
        sumCPU.end(),   
        [&](int& n) 
        {   // a bunch of branching can go here
            n = (n % 3 == 0) ? n * 2 : n * n;
            std::cout << "n = " << n << " { ";
            for( int i = 0; i < n; i++ )
            {
                if( i % 5 == 0 )
                {
                    std::cout << i << ", ";
                }
            }
            std::cout << " }\n";
        }
    );
}

int main(int argc, char* argv[])
{
    CppAmpMethodWithPPL();
	return 0;
}

