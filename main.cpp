#include <iostream>
#include <iomanip>
#include <conio.h>
#include <ctime>
#include <locale>

int main(int argc, char** argv) {
	srand(time(NULL));
	setlocale(LC_CTYPE, "ukr");

	//size of matrix from program arguments
	const int N = atoi(argv[1]);
	const int M = atoi(argv[2]);

	//std::cout << "child process " << "argc 0: " << argv[0] << "\n" 
			//<< "argc 1: " << argv[1] << " argc 2: " << argv[2] << "\n\n\n";
	std::cout << "Matrix size: " << N << " x " << M << "\n\n";

	//allocating memory
	int** matrix = new int*[N];
	for (int i = 0; i < N; ++i) {
		matrix[i] = new int[M];
	}

	//fill with random nums
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < M; ++j) {
			matrix[i][j] = rand() % 100;
		}
	}

	
	int counter = 0;
	//print matrix
	for (int i = 0; i < N; ++i) {
		counter = 0;
		for (int j = 0; j < M; ++j) {
			if (matrix[i][j] == matrix[i][0] && j != 0) {
				counter++;
			}
			std::cout << std::setw(2) << matrix[i][j] << " ";
		}
		std::cout << " | Num of elements equal to first: " << std::setw(2) << counter << "\n\n";
	}

	//clear memory
	for (int i = 0; i < N; ++i) {
		if(!matrix[i])
			delete[] matrix[i];
	}
	if(!matrix) 
		delete[] matrix;

	_getch();
	return 0;
}

