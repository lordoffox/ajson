// (C) Copyright Ning Ding 2013.8
// lordoffox@gmail.com
// Distributed under the Boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AJSON_HPP_DFGDFG39328429
#define AJSON_HPP_DFGDFG39328429

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/stringbuffer.h>

#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/mpl/if.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/stringize.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/preprocessor/facilities/empty.hpp>

#include <string>
#include <list>
#include <deque>
#include <vector>

namespace boost
{
	namespace ajson
	{
		template <typename ty ,  int tag>
		struct value_support_read_impl
		{
			typedef ty value_type;
			static void read( ::rapidjson::Value& json_value, value_type& value);
		};
		template <typename ty ,  int tag , typename alloc_ty>
		struct value_support_write_impl
		{
			typedef ty value_type;
			static void write( ::rapidjson::Value& json_value,const value_type& value , alloc_ty& alloc);
		};
		
		template<typename ty>
		struct integer_arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				if(json_value.IsNull())
				{
					value = 0;
					return;
				}
				if(json_value.IsBool())
				{
					bool jvalue = json_value.GetBool();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsInt())
				{
					int32_t jvalue = json_value.GetInt();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsUint())
				{
					uint32_t jvalue = json_value.GetUint();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsInt64())
				{
					int64_t jvalue = json_value.GetInt64();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsUint64())
				{
					uint64_t jvalue = json_value.GetUint64();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsDouble())
				{
					double jvalue = json_value.GetDouble();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsString())
				{
					int64_t jvalue = atol(json_value.GetString());
					value = static_cast<ty>(jvalue);
					return;
				}
			}
		};

		template<typename ty>
		struct integer_arithmetic_support_write_impl
		{
			typedef typename ty value_type;
			static void write( ::rapidjson::Value& json_value,const value_type& value)
			{
					json_value = value;
			}
		};

		template<typename ty>
		struct float_arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				if(json_value.IsNull())
				{
					value = 0.0;
					return;
				}
				if(json_value.IsBool())
				{
					bool jvalue = json_value.GetBool();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsInt())
				{
					int32_t jvalue = json_value.GetInt();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsUint())
				{
					uint32_t jvalue = json_value.GetUint();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsInt64())
				{
					int64_t jvalue = json_value.GetInt64();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsUint64())
				{
					uint64_t jvalue = json_value.GetUint64();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsDouble())
				{
					double jvalue = json_value.GetDouble();
					value = static_cast<ty>(jvalue);
					return;
				}
				if(json_value.IsString())
				{
					double jvalue = atof(json_value.GetString());
					value = static_cast<ty>(jvalue);
					return;
				}
			}
		};

		template<typename ty>
		struct float_arithmetic_support_write_impl
		{
			typedef typename ty value_type;
			static void write( ::rapidjson::Value& json_value,const value_type& value)
			{
				json_value = value;
			}
		};

		template <typename ty>
		struct arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_integral<value_type>,
				integer_arithmetic_support_read_impl<value_type>,
				float_arithmetic_support_read_impl<value_type>
			>::type impl;
			
			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				impl::read(json_value,value);
			}
		};

		template <typename ty>
		struct arithmetic_support_write_impl
		{
			typedef typename ty value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_integral<value_type>,
				integer_arithmetic_support_write_impl<value_type>,
				float_arithmetic_support_write_impl<value_type>
			>::type impl;

			template<typename alloc_ty>
			static void write( ::rapidjson::Value& json_value,const value_type& value , alloc_ty& alloc)
			{
				impl::write(json_value,value);
			}
		};

		template<typename char_traits_ty , typename char_alloc_type ,  int tag>
		struct value_support_read_impl<::std::basic_string<char, char_traits_ty, char_alloc_type>, tag>
		{
			typedef typename ::std::basic_string<char, char_traits_ty, char_alloc_type> value_type;

			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				if(json_value.IsNull())
				{
					value = "";
					return;
				}
				if(json_value.IsBool())
				{
					bool jvalue = json_value.GetBool();
					if(jvalue)
					{
						value = "yes";
					}
					else
					{
						value = "no";
					}
					return;
				}
				if(json_value.IsInt())
				{
					int32_t jvalue = json_value.GetInt();
					value = ::boost::lexical_cast<value_type>(jvalue);
					return;
				}
				if(json_value.IsUint())
				{
					uint32_t jvalue = json_value.GetUint();
					value = ::boost::lexical_cast<value_type>(jvalue);
					return;
				}
				if(json_value.IsInt64())
				{
					int64_t jvalue = json_value.GetInt64();
					value = ::boost::lexical_cast<value_type>(jvalue);
					return;
				}
				if(json_value.IsUint64())
				{
					uint64_t jvalue = json_value.GetUint64();
					value = ::boost::lexical_cast<value_type>(jvalue);
					return;
				}
				if(json_value.IsDouble())
				{
					double jvalue = json_value.GetDouble();
					value = ::boost::lexical_cast<value_type>(jvalue);
					return;
				}
				if(json_value.IsString())
				{
					value = json_value.GetString();
					return;
				}
			}
		};

		template<typename char_traits_ty , typename char_alloc_type ,  int tag , typename alloc_ty>
		struct value_support_write_impl<::std::basic_string<char, char_traits_ty, char_alloc_type>, tag , alloc_ty>
		{
			typedef typename ::std::basic_string<char, char_traits_ty, char_alloc_type> value_type;

			static void write( ::rapidjson::Value& json_value,const value_type& value , alloc_ty& alloc)
			{
				json_value.SetString(value.c_str());
			}
		};

		template<typename ty,int tag>
		inline void read_stl_seq_imp( ::rapidjson::Value& json_value, ty& value)
		{
			if(json_value.IsArray())
			{
				::rapidjson::SizeType len = json_value.Size();
				value.resize(len);
				::rapidjson::SizeType i = 0;
				for (ty::iterator iter = value.begin(); iter != value.end();++iter,++i)
				{
					value_support_read<typename ty::value_type,tag>::read(json_value[i],*iter);
				}
			}
			return ;
		}

		template<typename ty , int tag , typename alloc_ty>
		inline void write_stl_seq_imp( ::rapidjson::Value& json_value, const ty& value , alloc_ty& alloc)
		{
			if(!json_value.IsArray())
			{
				json_value.SetArray();
			}
			::std::size_t len = value.size();
			json_value.Clear();
			for (typename ty::const_iterator i = value.begin(); i != value.end();++i)
			{
				::rapidjson::Value evalue;
				value_support_write<typename ty::value_type,tag,alloc_ty>::write(evalue,*i,alloc);
				json_value.PushBack(evalue,alloc);
			}
			return;
		}

		template<typename ty,typename alloc_ty , int tag>
		struct value_support_read_impl<::std::list<ty,alloc_ty> , tag>
		{
			typedef typename ::std::list<ty,alloc_ty> value_type;

			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				value.clear();
				read_stl_seq_imp<value_type,tag>(json_value,value);
			}
		};

		template<typename ty,typename alloc_ty , int tag , typename json_alloc_ty>
		struct value_support_write_impl<::std::list<ty,alloc_ty> , tag , json_alloc_ty>
		{
			typedef typename ::std::list<ty,alloc_ty> value_type;

			static void write( ::rapidjson::Value& json_value,const value_type& value , json_alloc_ty& alloc)
			{
				write_stl_seq_imp<value_type,tag,json_alloc_ty>(json_value,value,alloc);
			}
		};

		template<typename ty,typename alloc_ty , int tag>
		struct value_support_read_impl<::std::deque<ty,alloc_ty> , tag>
		{
			typedef typename ::std::deque<ty,alloc_ty> value_type;

			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				value.clear();
				read_stl_seq_imp<value_type,tag>(json_value,value);
			}
		};

		template<typename ty,typename alloc_ty , int tag , typename json_alloc_ty>
		struct value_support_write_impl<::std::deque<ty,alloc_ty> , tag , json_alloc_ty>
		{
			typedef typename ::std::deque<ty,alloc_ty> value_type;

			static void write( ::rapidjson::Value& json_value,const value_type& value , json_alloc_ty& alloc)
			{
				write_stl_seq_imp<value_type,tag,json_alloc_ty>(json_value,value,alloc);
			}
		};

		template<typename ty,typename alloc_ty , int tag>
		struct value_support_read_impl<::std::vector<ty,alloc_ty> , tag>
		{
			typedef typename ::std::vector<ty,alloc_ty> value_type;

			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				value.clear();
				read_stl_seq_imp<value_type,tag>(json_value,value);
			}
		};

		template<typename ty,typename alloc_ty , int tag , typename json_alloc_ty>
		struct value_support_write_impl<::std::vector<ty,alloc_ty> , tag , json_alloc_ty>
		{
			typedef typename ::std::vector<ty,alloc_ty> value_type;

			static void write( ::rapidjson::Value& json_value,const value_type& value , json_alloc_ty& alloc)
			{
				write_stl_seq_imp<value_type,tag,json_alloc_ty>(json_value,value,alloc);
			}
		};


		template <typename ty ,  int tag>
		struct value_support_read
		{
			typedef typename ::boost::remove_const<ty>::type value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_arithmetic<value_type>,
				arithmetic_support_read_impl< value_type>,
				value_support_read_impl<value_type , tag>
			>::type impl;

			static void read( ::rapidjson::Value& json_value, value_type& value)
			{
				impl::read(json_value,value);
			}
		};

		template <typename ty ,  int tag , typename alloc_ty>
		struct value_support_write
		{
			typedef typename ::boost::remove_const<ty>::type value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_arithmetic<value_type>,
				arithmetic_support_write_impl< value_type>,
				value_support_write_impl<value_type , tag , alloc_ty>
			>::type impl;

			static void write( ::rapidjson::Value& json_value,const value_type& value , alloc_ty& alloc)
			{
				impl::write(json_value,value , alloc);
			}
		};

		template<int tag , typename ty>
		void ajson_readx( ::rapidjson::Value& json_value , ty& value)
		{
			value_support_read<ty,tag>::read(json_value,value);
		}

		template<int tag , typename ty , typename alloc_ty>
		void ajson_writex( ::rapidjson::Value& json_value , ty& value , alloc_ty& alloc)
		{
			value_support_write<ty,tag,alloc_ty>::write(json_value,value,alloc);
		}

		template<typename ty>
		void ajson_read( ::rapidjson::Value& json_value , ty& value)
		{
			ajson_readx<0>(json_value,value);
		}

		template<typename ty , typename alloc_ty>
		void ajson_write( ::rapidjson::Value& json_value , ty& value , alloc_ty& alloc)
		{
			ajson_writex<ty,0,alloc_ty>::write(json_value,value,alloc);
		}

		template <typename string_ty>
		struct string_buff_writer{
			typedef char Ch;

			string_buff_writer(string_ty& str) : str_(str){}

			void Put(char c) {str_.push_back(c) ; }

			// Not implemented
			char* PutBegin() { return 0; }
			size_t PutEnd(char*) { return 0; }
			void Flush(){}

			string_ty& str_;
		};

		template <typename string_ty>
		string_buff_writer<string_ty> make_string_buff_writer(string_ty& str)
		{
			return string_buff_writer<string_ty>(str);
		}

		template<class Writer>
		void	print_json(Writer& jw_,const ::rapidjson::Value& o)
		{
			if(o.IsObject())
			{
				jw_.StartObject();
				::rapidjson::SizeType count = 0;
				for(::rapidjson::Value::ConstMemberIterator p = o.MemberBegin() ; p != o.MemberEnd() ; ++p,++count)
				{
					jw_.String(p->name.GetString());
					print_json(jw_,p->value);
				}
				jw_.EndObject(count);
				return;
			}
			if(o.IsArray())
			{
				jw_.StartArray();
				::rapidjson::SizeType count = o.Size();
				for(::rapidjson::SizeType i = 0 ; i < count ; ++i)
				{
					print_json(jw_,o[i]);
				}
				jw_.EndArray(count);    
				return;
			}
			if(o.IsString())
			{
				jw_.String(o.GetString());
				return;
			}
			if(o.IsBool())
			{
				jw_.Bool(o.GetBool());
				return;
			}
			if(o.IsInt())
			{
				jw_.Int(o.GetInt());
				return;
			}
			if(o.IsUint())
			{
				jw_.Uint(o.GetUint());
				return;
			}
			if(o.IsInt64())
			{
				jw_.Int64(o.GetInt64());
				return;
			}
			if(o.IsUint64())
			{
				jw_.Uint64(o.GetUint64());
				return;
			}
			if(o.IsDouble())
			{
				jw_.Double(o.GetDouble());
				return;
			}
			if(o.IsNull())
			{
				jw_.Null();
				return;
			}
		}

		template<int tag,typename ty , typename alloc_ty>
		inline bool save_to_nodex(ty& value , ::rapidjson::Value& node , alloc_ty& alloc)
		{
			ajson_writex<tag>(node,value,alloc);
			return true;
		}

		template<typename ty , typename alloc_ty>
		inline bool save_to_node(ty& value , ::rapidjson::Value& node , alloc_ty& alloc)
		{
			return save_to_buffx<0>(value , node , alloc);
		}

		template<int tag,typename ty>
		inline bool load_from_nodex(ty& value , const ::rapidjson::Value& node)
		{
			ajson_readx<tag>(node,value);
		}

		template<typename ty>
		inline bool load_from_node(ty& value , const ::rapidjson::Value& node)
		{
			return load_from_nodex<0>(value,node);
		}

		template<int tag,typename ty , typename string_ty>
		inline bool load_from_buffx(ty& value , const char * data , string_ty& error_message)
		{
			::rapidjson::Document document;
			document.Parse<0>(data);
			if(document.HasParseError())
			{
				char * error_offset = (char *)data + document.GetErrorOffset();
				::std::size_t len = strlen(error_offset);
				if (len > 50)
				{
					len = 50;
				}
				char error_str[256];
				int offset = ::sprintf_s(error_str,"error occurred %s near ",document.GetParseError());
				memcpy(error_str+offset,error_offset,len);
				error_str[offset+len] = 0;
				error_message = error_str;
				return false;
			}
			ajson_readx<tag>(document,value);
		}

		template<typename ty , typename string_ty>
		inline bool load_from_buff(ty& value , const char * data , string_ty& error_message)
		{
			return load_from_buffx<0>(value,data,error_message);
		}

		template<int tag,typename ty , typename string_ty>
		inline bool save_to_buffx(ty& value , string_ty& buff)
		{
			::rapidjson::PrettyWriter<string_buff_writer<string_ty> > w(make_string_buff_writer(buff));
			::rapidjson::Document doc;
			doc.SetObject();
			ajson_writex<tag>(doc,value,doc.GetAllocator());
			print_json(w,doc);
			return true;
		}

		template<typename ty , typename string_ty>
		inline bool save_to_buff(ty& value , string_ty& buff)
		{
			return save_to_buffx<0>(value , buff);
		}

		template<int tag,typename ty , typename string_ty>
		inline bool load_from_filex(ty& value , char * filename , string_ty& error_message)
		{
			::rapidjson::Document document;
			char readBuffer[65536];
			FILE * fd;
			if( 0 != ::fopen_s(&fd,filename,"r"))
			{
				error_message = "open file error.";
				return false;
			}
			::rapidjson::FileReadStream is(fd, readBuffer, sizeof(readBuffer));
			document.ParseStream<0,::rapidjson::UTF8<>>(is);
			if(document.HasParseError())
			{
				char error_str[256];
				::std::size_t offset = ::sprintf_s(error_str,256,"error occurred %s near ",document.GetParseError());
				fseek(fd,(int)document.GetErrorOffset(),SEEK_SET);
				offset += fread(error_str+offset,1,50,fd);
				error_str[offset] = 0;
				error_message = error_str;
				::fclose(fd);
				return false;
			}
			::fclose(fd);
			ajson_readx<tag>(document,value);
			return true;
		}

		template<typename ty , typename string_ty>
		inline bool load_from_file(ty& value , char * filename ,string_ty& error_message )
		{
			return load_from_filex<0>(value,filename,error_message);
		}

		template<int tag,typename ty  , typename string_ty>
		inline bool save_to_filex(ty& value , const char * filename ,string_ty& error_message)
		{
			FILE * fd;
			if( 0 != ::fopen_s(&fd,filename,"w"))
			{
				error_message = "open file error.";
				return false;
			}
			char writeBuffer[65536];
			::rapidjson::FileWriteStream os(fd, writeBuffer, sizeof(writeBuffer));
			::rapidjson::PrettyWriter<::rapidjson::FileWriteStream > w(os);
			::rapidjson::Document o;
			ajson_writex<tag>(o,value,o.GetAllocator());
			print_json(w,o);
			::fclose(fd);
			return true;
		}

		template<typename ty  , typename string_ty>
		inline bool save_to_file(ty& value , const char * filename , string_ty& error_message)
		{
			return save_to_filex<0>(value,filename,error_message);
		}

	}
}

#define AJSON_READ_MEMBER( r , v , elem ) \
	member_ptr = json_value.FindMember( BOOST_DO_STRINGIZE(elem) );\
	if ( member_ptr )\
	{\
		::boost::ajson::ajson_readx< v >( member_ptr->value , value.elem);\
	}
	

#define AJSON_WRITE_MEMBER( r ,v , elem ) \
	member_ptr = json_value.FindMember( BOOST_DO_STRINGIZE(elem) );\
	if ( member_ptr )\
	{\
		::boost::ajson::ajson_writex< v >( member_ptr->value , value.elem , alloc);\
	}\
	else\
	{\
		::rapidjson::Value mvalue;\
		::boost::ajson::ajson_writex< v >( mvalue , value.elem , alloc);\
		json_value.AddMember( BOOST_DO_STRINGIZE(elem) , mvalue , alloc);\
	}

#define AJSONX(TYPE, MEMBERS,X)\
namespace boost\
{\
	namespace ajson\
	{\
		template<>\
		struct value_support_read_impl<TYPE,X>\
		{\
			typedef TYPE value_type;	\
			static inline void read(::rapidjson::Value& json_value , value_type& value)\
			{\
				::rapidjson::Value::Member * member_ptr;\
				BOOST_PP_SEQ_FOR_EACH( AJSON_READ_MEMBER , X , MEMBERS ) \
				return;\
			}\
		};\
		template <typename alloc_ty>\
		struct value_support_write_impl<TYPE,X,alloc_ty>\
		{\
			typedef TYPE value_type;	\
			static inline void write(::rapidjson::Value& json_value , const value_type& value , alloc_ty& alloc)\
			{\
				json_value.SetObject();\
				::rapidjson::Value::Member * member_ptr;\
				BOOST_PP_SEQ_FOR_EACH( AJSON_WRITE_MEMBER , X , MEMBERS ) \
				return;\
			}\
		};\
	}\
}

#define AJSON(TYPE, MEMBERS) AJSONX(TYPE, MEMBERS , 0)
#endif
