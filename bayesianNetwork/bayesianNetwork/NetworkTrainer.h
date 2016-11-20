#pragma once
#include <vector>
#include <map>
#include "User.h"
#include <string>
#include "Movie.h"

using namespace std;

class NetworkTrainer
{
public:
	void fillUserGenresRelations(User * user, vector<string> * genres, map<int, Movie*> * movieMap, int * genresUsersCount);
	void fillGenderTables(map<int, User*> * userMap, float **genderTable, vector<string> * genres, int * genresUsersCount);
	NetworkTrainer(void);
	~NetworkTrainer(void);
};

