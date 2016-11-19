#pragma once
#include "MovieRating.h"
#include "GenreModel.h"
#include <vector>

class User
{
public:
	int userId;
	std::vector<MovieRating*>  moviesRatings;
	std::vector<GenreModel*>  genresRelations;
	char gender;
	int occupation;
	int age;
	void print();
	User(void);
	~User(void);
};

