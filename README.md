ajson
=====

a utility for orm C++ and json dom。

什么是AJSON？

struct Person
{

  std::string  Name;
  
  int          Age;
  
};


AJSON(Person , (Name)(Age))

Person obj;


char * data = ""
"{\n"
"	\"Name\" : \"Boo\","
"	\"Age\" : 28"
"}";

load_from_buff(obj,data);

以上代码解释了什么是AJSON,AJSON还支持stl的顺序容器

为什么要选择json而不是xml？
因为json的数据描述更符合C++的数据描述，
举个例子，xml有Node和Attribute两种方式，
给出一个C++的Struct的Member field，
应该对应那种类型呢，如果是写入，
那么写成Node呢还是Attribute呢，
这里面对一个二义性问题。

AJSON还有什么比较酷的地方吗

因为有时候有这样一种需求，一个C++的Type会有多种ORM方式，
比如Person可以有(Name)(Age),(Name),(Age)这3种方式存到不同的json文件中。
AJSON的本身是带有tag机制的

AJSONX(Person , (Name)(Age),0)

load_from_buffx<0>(obj,data);

AJSONX(Person , (Name),1)

load_from_buffx<1>(obj,data);

AJSONX(Person , (Age),2)

load_from_buffx<2>(obj,data);

每个tag编号就是一组编制，这样你可以给你的系统分配多种ORM方案，
更加灵活，默认的AJSON和load_from_buff其实就是用的0号tag，
这样不需要分组的用户也不会受到干扰。

AJSON就一个ajson.hpp头文件，能工作吗，不忽悠人吗
因为虽然就ajson.hpp这么一个头文件，但是确实能工作哦，
不过，他还依赖rapidjson和boost.proprocessor，
lexical_cast都只是头文件依赖，你无需配置链接库。

用AJSON要掏钱吗，要共享源代码吗
AJSON完全免费，不用共享源代码，不用传播出处，使用boost的授权方式。

