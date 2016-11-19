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

using namespace std;
using namespace stdext;
#define _CRT_SECURE_NO_WARNINGS

void fillUserGenresRelations(User * user, vector<string> * genres, map<int, Movie*> * movieMap, int * genresUsersCount) {
	//Можно genreModel убрать genreId и сделать по аналогии с genres
	for(int i = 0; i < genres->size(); i++) {
		GenreModel * model = new GenreModel();
		model -> genreId = i;
		model -> isFavorite = 0;
		user -> genresRelations.push_back(model);
	}

	int * genresRatings = new int[genres->size()];
	int totalUserRating = 0;
	
	for (int i = 0; i < genres->size(); i++) {
		genresRatings[i] = 0;
	}

	for(int i = 0; i < user -> moviesRatings.size(); i++) {
		int * genreIds = movieMap -> find(user->moviesRatings[i]->movieId)->second->genreIds;
		int movieGenresCount = movieMap -> find(user->moviesRatings[i]->movieId)->second->genreCount;
		for(int j = 0; j < movieGenresCount; j++) {
			int genreId = genreIds[j];
			genresRatings[genreId] += (user -> moviesRatings[i]->rating);
		}
		totalUserRating += (user -> moviesRatings[i]->rating);
	}

	//TODO Добавить проверку булевую на проценты
	for (int i = 0; i < genres->size(); i++) {
		float likeProbability = (float)genresRatings[i] / totalUserRating;
		if(likeProbability >= 0.2f) {
			user -> genresRelations[i] -> isFavorite = 1;
			genresUsersCount[i] ++;
		}
	}
}

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

int _tmain(int argc, _TCHAR* argv[])
{
	DataParser dataParser;
	map<int, User*> * userMap = new map<int,User*>();
	map<int, Movie*> * movieMap = new map<int,Movie*>();
	//genreId - position in vector. genres[5] = horror; - means genre "horror" has id 5;
	vector<string> * genres = new vector<string>();

	time_t startParseTime;
	time(&startParseTime);
	cout << "Parsing dataset" << endl;
	
	dataParser.parseRatings(userMap);
	dataParser.parseUsers(userMap);
	dataParser.parseMovies(movieMap, genres);
	
	const int genresCount = genres->size();

	float **genderTable = new float*[genresCount];
	float **occupationTable = new float*[genresCount];
	float ** ageTable = new float*[genresCount];
	int * genresUsersCount = new int[genresCount];//Count of users likes genre i
	float maleGenderPrior = 0;
	float * genrePrior = new float[genresCount];
	float * genderPrior = new float[2]; //TODO поставить перемнную //0 - male, 1 - female

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
	
	time_t endParseTime;
	time(&endParseTime);
	cout << "Dataset was parsed in " + (endParseTime - startParseTime)<< endl;
	
	cout << "Training dataset" << endl;
	
	for(map<int, User*>::iterator MapItor = userMap->begin(); MapItor != userMap->end(); ++MapItor)
	{
		User* user = (*MapItor).second;
		fillUserGenresRelations(user, genres, movieMap, genresUsersCount);
	}

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


	//TODO В будущем можно объединить 2 цикла в один, пока что отдельно для понимания
	for(map<int, User*>::iterator MapItor = userMap->begin(); MapItor != userMap->end(); ++MapItor)
	{
		User* user = (*MapItor).second;
		char gender = user->gender;
		int age = user->age;
		int occupation = user->occupation;
		//cout << gender << endl;
		for(int i = 0; i < user->genresRelations.size(); i++) {
			GenreModel * model = user->genresRelations[i];
			if(model -> isFavorite) {
				if(gender == 'M')
					genderTable[i][0]++;
				else
					genderTable[i][1]++;

				occupationTable[i][occupation]++;
			}
		}
	}

	for (int i = 0; i < genres->size(); i++) {
		//cout << genresUsersCount[i] << endl;
		for(int j = 0; j < 21; j++) {
			occupationTable[i][j] /= (float)genresUsersCount[i];
			//cout << "occupation i = " << i << "  j = " << j << " value " << occupationTable[i][j] << endl; 
		}
		for(int j = 0; j < 2; j++) {
			genderTable[i][j] /= (float)genresUsersCount[i];
			cout << "genre i = " << i << "  j = " << j << " value " << genderTable[i][j] << endl; 
		}	 
	}

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
