// bayesianNetwork.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <string>
#include <ctime>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>
#include <time.h>
#include "User.h"
#include "Movie.h"
#include "DataParser.h"
#include "NetworkTrainer.h"

using namespace std;
using namespace stdext;
#define _CRT_SECURE_NO_WARNINGS



float predictGender(User * user, float ** genderTable, float koef) {
	float probability = 1;
	for(int i = 0; i < 17; i++) {
		float pgu = genderTable[i][0];
		if(user->genresRelations[i]->isFavorite)
			probability *= pgu;
	}
	
	//probability /= koef;
	return probability;
}

template < typename T > void cleanMap(map<int, T*> * deletedMap){
	for(map<int, T*>::iterator MapItor = deletedMap->begin(); MapItor != deletedMap->end(); ++MapItor) {
		T* Value = (*MapItor).second;
		delete Value;
	}
}

// initialization of Conditional probabilities tables and prior probabilities vectors
void tablesInit(int genresCount, float **genderTable, float ** occupationTable, float ** ageTable, int * genresUsersCount,
				float * genrePrior, float *genderPrior) {
	for (int i = 0; i < genresCount; i++) {
		genrePrior[i] = 0;
	
		for(int i = 0; i < 2; i++) {
			genderPrior[i] = 0;
		}

		genderTable[i] = new float[2]; // 0 - male, 1 - female
		
		for(int j = 0; j < 2; j++){
			genderTable[i][j] = 0;
		}

		occupationTable[i] = new float[21]; // see readme
		
		for(int j = 0; j < 21; j++){
			occupationTable[i][j] = 0;
		}

		ageTable[i] = new float[7];
		
		for(int j = 0; j < 7; j++) {
			ageTable[i][j] = 0;
		}

		genresUsersCount[i] = 0;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	DataParser dataParser;
	NetworkTrainer networkTrainer;
	map<int, User*> * userMap = new map<int,User*>();
	map<int, Movie*> * movieMap = new map<int,Movie*>();
	//genreId - position in vector. genres[5] = horror; - means genre "horror" has id 5;
	vector<string> * genres = new vector<string>();

	time_t startParseTime;
	time(&startParseTime);
	cout << "Parsing dataset" << endl;
	
	//Parse files content into maps
	dataParser.parseRatings(userMap);
	dataParser.parseUsers(userMap);
	dataParser.parseMovies(movieMap, genres);
	//number of parsed genres
	const int genresCount = genres->size();

	float **genderTable = new float*[genresCount];//Conditional probability  sex | genres
	float **occupationTable = new float*[genresCount]; // Conditional probability  occupation | genres
	float ** ageTable = new float*[genresCount]; // Conditional probability  age | genre
	int * genresUsersCount = new int[genresCount];//Count of users likes genre i
	float maleGenderPrior = 0;
	float * genrePrior = new float[genresCount]; // genre prior probability
	float * genderPrior = new float[2]; //TODO поставить перемнную //0 - male, 1 - female
	tablesInit(genresCount, genderTable, occupationTable, ageTable, genresUsersCount, genrePrior, genderPrior);
	
	time_t endParseTime;
	time(&endParseTime);
	cout << "Dataset was parsed in " + (endParseTime - startParseTime) << endl;
	
	cout << "Training dataset" << endl;
	
	for(map<int, User*>::iterator MapItor = userMap->begin(); MapItor != userMap->end(); ++MapItor)
	{
		User* user = (*MapItor).second;
		networkTrainer.fillUserGenresRelations(user, genres, movieMap, genresUsersCount);
	}

	//Calculate the prior probability
	for(map<int, Movie*>::iterator MapItor = movieMap->begin(); MapItor != movieMap->end(); ++MapItor)
	{
		Movie* movie = (*MapItor).second;
		for(int i = 0; i < movie->genreCount; i++) {
			genrePrior[movie->genreIds[i]]++;
		}
	}

	float bayesScaleCoef = 1;
	float debug = 0;
	for(int i = 0; i < genres->size(); i++) {
		genrePrior[i] /= movieMap->size();
		debug += genrePrior[i];
		bayesScaleCoef *= genrePrior[i];
	}

	cout << debug << endl;
	
	networkTrainer.fillGenderTables(userMap, genderTable, genres, genresUsersCount);

	cout << "Dataset was trained in" << endl;
	float count  = 0;
	float predicted = 0;
	for(map<int, User*>::iterator MapItor = userMap->begin(); MapItor != userMap->end(); ++MapItor)
	{
		
		User* user = (*MapItor).second;
		//cout << "Source user is: " << endl;
		//user->print();
		//cout << "Favorite genres is " ;
		/*for(int i = 0; i < genresCount; i++){
			if(user->genresRelations[i]->isFavorite) {
				cout << i << "  "; 
			}
		}
		cout << endl;*/
		float prob = predictGender(user, genderTable, bayesScaleCoef);
		if(prob >= 0.5 && user->gender == 'M') {
			predicted++;
		} else 
			if(prob <= 0.5 && user->gender == 'F')
			{
				predicted++;
			}
		//cout << "--------------------------" << endl;
		count++;
		//if(count == 3)
			//break;
	}
	
	float rate = predicted / count; 
	cout << endl << " RATE " << rate << endl;
	cleanMap(userMap);
	cleanMap(movieMap);
	delete genres;
	delete userMap;
	getchar();
	return 0;
}

