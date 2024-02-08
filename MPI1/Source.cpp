#include <iostream>
#include "mpi.h"
#include "time.h"
#include "stdlib.h"
#include "string"

using namespace std;

// Ïðîãðàììà ñèìóëèðóåò ïîäñ÷åò âûïîëíåííûõ ðàáîò äî ïåðâîé ïîëîìêè.
// Çà ñèìóëÿöèþ ðàáîòû îòâå÷àåò ôóíêöèÿ work(int crit), ãäå crit - êðèòè÷åñêîå çíà÷åíèå îò 0 äî 100
// Â ñëó÷àå ïîëîìêè ôóíêöèÿ âîçâðàùàåò -1, ïîñëå ÷åãî ñëîìàâøèéñÿ ïðîöåññ îòïðàâëÿåò îò÷åò è çàâåðøàåò ðàáîòó
// Ïðîöåññ 0 àíàëèçèðóåò âûïîëíåíèå ðàáîòû äðóãèìè ïðîöåññàìè è çàâåðøàåò ðàáîòó, åñëè îí îñòàëñÿ îäèí.




int work(int crit)
{
	srand(time(NULL));
	int value = rand() % 100;

	if (value < crit)
		return value;

	return -1;
}



int main(int argc, char* argv[])
{
	int buffer, numtasks, rank;
	int done_work, work_value;

	int crit_value = 0;

	MPI_Status Status;
	// <ïðîãðàììíûé êîä áåç èñïîëüçîâàíèÿ MPI ôóíêöèé>


	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// <ïðîãðàììíûé êîä ñ èñïîëüçîâàíèåì MPI ôóíêöèé>

	if (rank == 0)
	{
		cout << "Hello from Arbitr (proccess 0)" << endl;
		
		for (int i = 1; i < numtasks; i++)
		{
			do {
				cout << "\n\nWaiting for entering crit value for proccess #"<< i << "   (0-100) " << endl;
				cin >> crit_value;
			} while (crit_value < 0 || crit_value>100);
			MPI_Send(&crit_value, 1, MPI_INT, i, 10, MPI_COMM_WORLD);
		}	


		while (numtasks > 1)
		{
			buffer = 0;
			MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE,
				MPI_ANY_TAG, MPI_COMM_WORLD, &Status);			
			cout << "\n\nProccess #" << buffer << " has mulfunction." << endl;
			work_value = 0;
			MPI_Send(&work_value, 1, MPI_INT, buffer, 1, MPI_COMM_WORLD);
			MPI_Recv(&work_value, 1, MPI_INT, buffer,
				1, MPI_COMM_WORLD, &Status);
			cout << "It's counter-value is " << work_value << endl;
			numtasks--;
			cout << "Now we have " << numtasks - 1 << "\n\n";
		}
		cout << "The program was termineted because there is no proccess to control" << endl;
		system("pause");
	}
	else
	{
		MPI_Recv(&crit_value, 1, MPI_INT, 0,
			10, MPI_COMM_WORLD, &Status);
		cout << "Proccess # " << rank << " with crit value "<< crit_value << " start working" << endl;
		work_value = 0;
		done_work = -1;
		do {
			done_work++;
			work_value = work(crit_value);			
		} while (work_value != -1);
		cout << "Error in proccess #" << rank << " value: "<< done_work <<endl;
		
		buffer = rank;
		MPI_Send(&buffer, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE,
			1, MPI_COMM_WORLD, &Status);
		MPI_Send(&done_work, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	// <ïðîãðàììíûé êîä áåç èñïîëüçîâàíèÿ MPI ôóíêöèé>
	return 0;
}
