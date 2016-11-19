#pragma once
#include "Movie.h"
#include <map>
#include <vector>
#include <string>
#include "User.h"
#include <fstream>
#include <sstream>

using namespace std;

class DataParser
{
public:
	void parseUsers(map<int, User*> * userMap);
	void parseMovies(map<int, Movie*> * movieMap, vector<string> * genres);
	void parseRatings(map<int, User*> *userMap);
	DataParser(void);
	~DataParser(void);
};

