#include "stdafx.h"
#include "DataParser.h"



DataParser::DataParser(void)
{
}


DataParser::~DataParser(void)
{
}


std::vector<std::string> split(std::string &s, string delim) {
	vector<string> results;
	bool hasDelimeter;
	int lastDelimeterIndex = 0;
	if(s.size() == 0 || s.size() < delim.size())
		return results;
	for(int i = 0; i < s.size() - delim.size(); i++) {
		hasDelimeter = true;
		for(int j = 0; j < delim.size(); j++)
			if(s[i + j] != delim[j] ) {
				hasDelimeter = false;
				break;
			}

			if(hasDelimeter) {
				results.push_back(s.substr(lastDelimeterIndex, i - lastDelimeterIndex));
				i += delim.size();
				lastDelimeterIndex = i;
			}
	}

	results.push_back(s.substr(lastDelimeterIndex, s.size() - lastDelimeterIndex));
	
	return results;
}

void DataParser::parseMovies(map<int, Movie*> * movieMap, vector<string> * genres) {
	ifstream moviesFile("movies.dat");
	string line;
  
	if (moviesFile.is_open())
	{
	while (moviesFile.good() )
	{
		getline (moviesFile, line);
		vector<string> splitResult = split(line,"::");
		if(splitResult.size() < 3)
			break;

		int movieId = stoi(splitResult[0]);
		string title = splitResult[1];
		vector<string> movieGenres = split(splitResult[2], "|");
		bool genreIsExists = false;
		
		int * genreIds = new int[movieGenres.size()];
		int genreCount = 0;
		
		for(int j = 0, i = 0; j < movieGenres.size(); j++)  {
			genreIsExists = false;
			for(i = 0; i < genres -> size(); i++) {
				if(genres[0][i] == movieGenres[j]) {
					genreIds[genreCount++] = i;
					genreIsExists = true;
					break;
				}
			}

			if(!genreIsExists) {
					genres -> push_back(movieGenres[j]);
					genreIds[genreCount++] = genres -> size() - 1;
			}
		}

		if(genres->size() == 0) {
			for(int j = 0; j < movieGenres.size(); j++)  {
				genres -> push_back(movieGenres[j]);
				genreIds[genreCount++] = genres -> size() - 1;
			}
		}

		Movie * movie = new Movie();
		movie -> genreIds = genreIds; 
		movie -> title = title;
		movie -> movieId = movieId;
		movie -> genreCount = genreCount;
		movieMap->insert(std::pair<int,Movie*>(movieId,movie));
		
		}
	}
	
	moviesFile.close();
}

void DataParser::parseRatings(map<int, User*> *userMap) {
	ifstream ratingsFile("ratings.dat");
	string line;
	int debug = 0;
	if (ratingsFile.is_open())
	{
		while (ratingsFile.good() )
		{
			getline (ratingsFile, line);
			vector<string> splitResult = split(line,"::");

			if(splitResult.size() < 4)
				break;
	  
			int userId = stoi(splitResult[0]);
			int movieId = stoi(splitResult[1]);
			int rating = stoi(splitResult[2]);
	 
			if(userMap->count(userId)) {
				User * foundedUser = userMap->find(userId)->second;
				MovieRating * newRating = new MovieRating();
				newRating -> movieId = movieId;
				newRating -> rating = rating;
				foundedUser ->moviesRatings.push_back(newRating);
			} else {
				User * user = new User();
				user -> userId = userId;
				MovieRating * newRating = new MovieRating();
				newRating -> movieId = movieId;
				newRating -> rating = rating;
				user -> moviesRatings.push_back(newRating);	
				userMap->insert(std::pair<int,User*>(userId,user));
			}
		}
	ratingsFile.close();
	}
}

void DataParser::parseUsers(map<int, User*> * userMap) {
	ifstream usersFile("users.dat");
	string line;
	int debugCount = 0;
	if (usersFile.is_open())
	{
		while (usersFile.good() )
		{
			getline (usersFile, line);
			vector<string> splitResult = split(line,"::");

			if(splitResult.size() < 4)
				break;
	  
			int userId = stoi(splitResult[0]);
			char gender = splitResult[1][0];
			int age = stoi(splitResult[2]);
			int occupation = stoi(splitResult[3]);
		
			if(userMap->count(userId)) {
				User * foundedUser = userMap->find(userId)->second;
				foundedUser -> age = age;
				foundedUser -> gender = gender;
				foundedUser -> occupation = occupation;
			}
		}
	usersFile.close();
	}
}