#include <iostream>
#include <string>

#include "ajson.hpp"

using namespace std;
using namespace boost::ajson;

struct demo
{
  string hello;
	string world;
};

AJSON(demo,(hello)(world))

int main(int argc, char * argv[])
{
	const char * buff = "{\"hello\" : \"Hello\", \"world\" : \"world.\"}";
	demo the_demo;
	string error_msg;
	load_from_buff(the_demo,buff,error_msg);
	cout << the_demo.hello << " " << the_demo.world << std::endl;
	cin.get();
	return 0;
}
