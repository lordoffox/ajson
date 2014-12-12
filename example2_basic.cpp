#include <iostream>
#include <string>
#include <vector>

#include "ajson.hpp"

using namespace std;
using namespace boost::ajson;

struct Education
{
	string	School;
	double	GPA;

	Education():GPA(0.0){}

	Education(const string& school , double gpa)
		:School(school),GPA(gpa)
	{
	}
};

struct Person
{
	string	Name;
	int			Age;
	vector<Education> Educations;
};

AJSON(Education,(School)(GPA))
AJSON(Person,(Name)(Age)(Educations))

int main(int argc, char * argv[])
{
	Person person;
	person.Name = "Bob";
	person.Age = 28;
	person.Educations.push_back(Education("MIT",600));
	store_buffer json_buff;
	save_to_buff(person,json_buff);
	std::cout << json_buff.data() << std::endl;
	cin.get();
	return 0;
}
