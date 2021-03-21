#include "mpi.h"
#include "time.h"
#include "stdlib.h"
#include "string"
#include "stdio.h"
#include <iostream>;

using namespace std;

// Программа симулирует подсчет выполненных работ до первой поломки.
// За симуляцию работы отвечает функция work(int crit), где crit - критическое значение от 0 до 100
// В случае поломки функция возвращает -1
// Процесс 0 анализирует выполнение работы другими процессами и завершает работу, если хотя бы один сломался.




int work(int crit)
{
	int value = rand() % 100000 - crit;

	if (value < 0)
		return -1;
	return value;
}



int main(int argc, char* argv[])
{
	const int broke_radius = 10;
	const int min_broke_chance = 1;
	
	int buffer = 0, numtasks, rank;
	int done_work = 0, work_value = 0;

	int crit_value = 0;

	MPI_Status Status;
	// <программный код без использования MPI функций>


	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// <программный код с использованием MPI функций>

	if (rank == 0)
	{
		//printf("Hello from Arbitr (proccess 0)\n");
		cout << "\n\n\nHello from Arbitr(proccess 0)\n" 
			 << "-----------------------------\n";
		fflush(stdout);
		MPI_Barrier(MPI_COMM_WORLD);
		work_value = 1;
		done_work = 0;
		while (work_value == 1)
		{
			MPI_Bcast(&work_value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			MPI_Reduce(&work_value, &buffer, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
			if (buffer == -1)
			{
				work_value = -1;
				MPI_Bcast(&work_value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
			}
			else done_work++;			
		}
		MPI_Barrier(MPI_COMM_WORLD);
		//printf("Loops without any broke: %d\n", done_work);
		cout << "-----------------------------\n"
			 << "Loops without any broke: " << done_work << "\n\n\n";		
	}
	else
	{
		srand(time(NULL) + 100 * rank);
		crit_value = rand() % broke_radius + min_broke_chance;
		MPI_Barrier(MPI_COMM_WORLD);
		//printf("Hello from proccess %d with broke chance %d\n", rank, crit_value);
		cout << "Hello from proccess " << rank << " with broke chance " << crit_value << endl;
		while (true)
		{				
			if (work_value == -1) break;		
			
			work_value = work(crit_value);
			MPI_Reduce(&work_value, &buffer, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
			MPI_Bcast(&work_value, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
		}
		//printf("Hello from proccess %d", rank);
		cout << "Bye from proccess " << rank << endl;
		MPI_Barrier(MPI_COMM_WORLD);
	}

	MPI_Finalize();
	// <программный код без использования MPI функций>
	return 0;
}