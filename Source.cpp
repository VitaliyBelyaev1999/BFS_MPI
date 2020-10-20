#include "mpi.h"
#include<stdio.h>
#include<stdlib.h>
#include<iostream>
using namespace std;

#define TOP_COUNT 4

int areAllchecked_top(int checked_top[], int size)
{
	for (int i = 0; i < size; i++)
	{
		if (checked_top[i] == 0)
			return 0;
	}
	return 1;
}

int main(int argc, char* argv[])
{
	//Variables and Initializations
	int size;
	int rank;
	int source_top;
	int M[TOP_COUNT][TOP_COUNT];
	int adj_matrix[TOP_COUNT * TOP_COUNT];
	int adj_row[TOP_COUNT];
	int nodes_of_row[TOP_COUNT - 1];
	int nodes_of_matrix[(TOP_COUNT - 1) * TOP_COUNT];
	int checked_top[TOP_COUNT];
	//MPI Code
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		//Entering the Adjacency Matrix
		cout << "Enter the Adjacency Matrix\n";
		for (int i = 0; i < TOP_COUNT * TOP_COUNT; i++)
		{
			cin >> adj_matrix[i];
		}
		cout << endl;

		//Entering the Source Vertex
		cout << "Enter the Source Vertex\n";
		cin >> source_top;
		cout << endl;
	}

	//Broadcasting the source vertex;
	MPI_Bcast(&source_top, 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Scattering each row of the adjacency matrix to each of the processes
	MPI_Scatter(adj_matrix, TOP_COUNT, MPI_INT, adj_row, TOP_COUNT, MPI_INT, 0, MPI_COMM_WORLD);

	//Initializing The Adjacency Queue of each process
	for (int i = 0; i < TOP_COUNT - 1; i++)
	{
		nodes_of_row[i] = -1;
	}

	//BFS code
	int index = 0;
	if (rank >= source_top)
	{
		for (int i = 0; i < TOP_COUNT; i++)
		{
			if (adj_row[i] == 1)
			{
				nodes_of_row[index++] = i;
			}
		}
	}

	//Each Process printing the adjacent nodes found
	cout << "Process " << rank << ": ";
	for (int i = 0; i < index; i++)
	{
		cout << nodes_of_row[i] << " ";
	}
	cout << endl;

	//For synchronization
	MPI_Barrier(MPI_COMM_WORLD);

	//Gathering all the nodes in BFS found by each process
	MPI_Gather(nodes_of_row, TOP_COUNT - 1, MPI_INT, nodes_of_matrix, TOP_COUNT - 1, MPI_INT, 0, MPI_COMM_WORLD);

	//Printing the Order of traversed nodes in root
	for (int i = 0; i < TOP_COUNT; i++)
	{
		checked_top[i] = 0;
	}

	if (rank == 0)
	{
		cout << "\nBFS Traversal: " << endl;
		cout << source_top;
		for (int i = 0; i < (TOP_COUNT - 1) * TOP_COUNT; i++)
		{
			//Exit Condition
			if (areAllchecked_top(checked_top, TOP_COUNT))
			{
				break;
			}

			if (nodes_of_matrix[i] != -1)
			{
				if (checked_top[nodes_of_matrix[i]] == 0)
				{
					cout << " -> " << nodes_of_matrix[i];
					checked_top[nodes_of_matrix[i]] = 1;
				}
			}
			else
			{
				continue;
			}
		}
	}

	//End of BFS code
	MPI_Finalize();
	return 0;
}