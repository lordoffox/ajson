// (C) Copyright Ning Ding 2013.8
// lordoffox@gmail.com
// Distributed under the Boost Software License, Version 1.0. (See accompany-
// ing file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef AJSON_HPP_DFGDFG39328429
#define AJSON_HPP_DFGDFG39328429

#include "rapidjson/rapidjson.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

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

#include <stdio.h>

#ifndef _MSC_VER
errno_t static inline fopen_s(FILE **f, const char *name, const char *mode) {
	errno_t ret = 0;
	assert(f);
	*f = fopen(name, mode);
	/* Can't be sure about 1-to-1 mapping of errno and MS' errno_t */
	if (!*f)
		ret = errno;
	return ret;
}
#endif

namespace boost
{
	namespace ajson
	{
		template <typename alloc_ty>
		struct ajson_store_buffer
		{
		private:
			alloc_ty alloc;
		public:

			enum { good , read_overflow };

			char * m_header_ptr;
			char * m_read_ptr;
			char * m_write_ptr;
			char * m_tail_ptr;
			int							m_status;
			std::size_t			m_length;

			enum{INIT_AMSG_BUFF_SIZE=1024};
			ajson_store_buffer():m_length(INIT_AMSG_BUFF_SIZE),m_status(good)
			{
				this->m_header_ptr = this->alloc.allocate(INIT_AMSG_BUFF_SIZE);
				this->m_read_ptr = this->m_header_ptr;
				this->m_write_ptr = this->m_header_ptr;
				this->m_tail_ptr = this->m_header_ptr + m_length;
			}

			~ajson_store_buffer()
			{
				this->alloc.deallocate(m_header_ptr,this->m_length);
			}

			std::size_t read(char * buffer,std::size_t len)
			{
				if(this->m_read_ptr + len > this->m_tail_ptr)
				{
					m_status = read_overflow;
					return 0;
				}
				memcpy(buffer,this->m_read_ptr,len);
				this->m_read_ptr += len;
				return len;
			}

			std::size_t growpup(std::size_t want_size)
			{
				std::size_t new_size = ((want_size+INIT_AMSG_BUFF_SIZE - 1) / INIT_AMSG_BUFF_SIZE)*INIT_AMSG_BUFF_SIZE;
				std::size_t write_pos = this->m_write_ptr - this->m_header_ptr;
				std::size_t read_pos = this->m_read_ptr - this->m_header_ptr;
				char * temp = this->m_header_ptr;
				this->m_header_ptr = this->alloc.allocate(new_size);
				memcpy(this->m_header_ptr,temp,this->m_length);
				this->alloc.deallocate(temp,this->m_length);
				this->m_length = new_size;
				this->m_write_ptr = this->m_header_ptr + write_pos;
				this->m_read_ptr = this->m_header_ptr + read_pos;
				this->m_tail_ptr = this->m_header_ptr + m_length;
				return new_size;
			}

			std::size_t write(const char * buffer,std::size_t len)
			{
				std::size_t writed_len = this->m_write_ptr + len - this->m_header_ptr;
				if(writed_len > this->m_length)
				{
					this->growpup(writed_len);
				}
				memcpy((void*)this->m_write_ptr,buffer,len);
				this->m_write_ptr += len;
				return len;
			}

			bool bad(){ return m_status != good; }

			ajson_store_buffer& seekp(int offset , int seek_dir)
			{
				switch(seek_dir)
				{
				case std::ios::beg:
					{
						if(offset<0)
						{
							offset = 0;
						}
						this->m_write_ptr = this->m_header_ptr + offset;
						break;
					}
				case std::ios::cur:
					{
						if(offset<0)
						{
							offset = offset + int(this->m_write_ptr - this->m_header_ptr);
							if (offset < 0)
							{
								offset = 0;
							}
							this->m_write_ptr = this->m_header_ptr + offset;
						}
						else
						{
							if(this->m_write_ptr+offset > this->m_tail_ptr)
							{
								this->m_write_ptr = this->m_tail_ptr;
							}
						}
						
						break;
					}
				case std::ios::end:
					{
						if(offset<0)
						{
							offset = offset + int(this->m_write_ptr - this->m_header_ptr);
							if (offset < 0)
							{
								offset = 0;
							}
							this->m_write_ptr = this->m_header_ptr + offset;
						}
						break;
					}
				}
				return *this;
			}

			inline void clear()
			{
				this->m_read_ptr = this->m_header_ptr;
				this->m_write_ptr = this->m_header_ptr;
			}

			inline const char * data() const
			{
				return this->m_header_ptr;
			}

			inline ::std::size_t read_length() const
			{
				return this->m_read_ptr - this->m_header_ptr;
			}

			inline ::std::size_t write_length() const
			{
				return this->m_write_ptr - this->m_header_ptr;
			}
		};

		typedef ajson_store_buffer<std::allocator<char> > store_buffer;

		struct ajson_file_stream
		{
		public:

			enum { good , read_overflow , file_error };

			FILE * m_f;
			int		 m_status;
			enum{INIT_AMSG_BUFF_SIZE=1024};
			ajson_file_stream(const char * filename):m_f(NULL),m_status(good)
			{
				fopen_s(&this->m_f,filename,"w");
				if(NULL ==this->m_f)
				{
					this->m_status = file_error;
				}
			}

			~ajson_file_stream()
			{
				if(this->m_f)
				{
					fclose(m_f);
				}
			}

			inline std::size_t read(char * buffer,std::size_t len)
			{
				std::size_t rlen = fread(buffer,len,1,this->m_f);
				return rlen;
			}

			inline std::size_t write(const char * buffer,std::size_t len)
			{
				std::size_t wlen = fwrite(buffer,len,1,this->m_f);
				return wlen;
			}

			inline bool bad(){ return m_status != good; }

			inline int seekp(int offset , int seek_dir)
			{
				switch(seek_dir)
				{
				case std::ios::beg:
					{
						if(offset<0)
						{
							offset = 0;
						}
						return fseek(this->m_f,offset,SEEK_SET);
					}
				case std::ios::cur:
					{
						return fseek(this->m_f,offset,SEEK_CUR);
					}
				case std::ios::end:
					{
						return fseek(this->m_f,offset,SEEK_END);
					}
				}
				return 0;
			}

			inline void clear()
			{
				fseek(this->m_f,0,SEEK_SET);
			}
		};
		template <typename ty, typename jsonvalue_type, int tag>
		struct value_support_read_impl
		{
			typedef ty value_type;
			typedef value_support_read_impl<ty, jsonvalue_type, tag> impl;
			static void read( jsonvalue_type& json_value, value_type& value);
		};

    template <typename ty, int tag>
		struct value_support_write_impl
		{
			typedef ty value_type;
			typedef value_support_write_impl<ty, tag> impl;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value);
		};
		
		template <typename ty, typename jsonvalue_type , int tag>
		struct value_support_read;

		template <typename ty, int tag>
		struct value_support_write;

    template<typename ty, typename jsonvalue_type>
		struct integer_arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			static void read( jsonvalue_type& json_value, value_type& value)
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
			typedef ty value_type;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value)
			{
				long long temp = (long long)value;
				char buffer[64];
				buffer[63] = 0;
				size_t len = 64;
				size_t pos = 62;
				bool Sig = false;
				if (temp < 0)
				{
					Sig = true;
					temp *= -1;
				}
				if (temp == 0)
				{
					buffer[pos--] = '0';
				}
				while (temp)
				{
					buffer[pos--] = (char)((temp % 10) + '0');
					temp = temp / 10;
				}
				if (Sig)
				{
					buffer[pos--] = '-';
				}
				++pos;
				memmove(buffer, buffer + pos, (len - pos));
				store_data.write(buffer, len - pos - 1);
			}
		};

    template<typename ty, typename jsonvalue_type>
		struct float_arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			static void read( jsonvalue_type& json_value, value_type& value)
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
			typedef ty value_type;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value)
			{
				char buffer[64] = { 0 };
#ifdef _MSC_VER
				_gcvt_s(buffer, 63, value, 8);
#else
				gcvt(value, 63, buffer);
#endif
				store_data.write(buffer, strlen(buffer));
			}
		};

		template<typename jsonvalue_type>
		struct integer_arithmetic_support_read_impl <jsonvalue_type, bool >
		{
			typedef bool value_type;
			static void read(jsonvalue_type& json_value, value_type& value)
			{
				if (json_value.IsNull())
				{
					value = 0;
					return;
				}
				if (json_value.IsBool())
				{
					bool jvalue = json_value.GetBool();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsInt())
				{
					int32_t jvalue = json_value.GetInt();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsUint())
				{
					uint32_t jvalue = json_value.GetUint();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsInt64())
				{
					int64_t jvalue = json_value.GetInt64();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsUint64())
				{
					uint64_t jvalue = json_value.GetUint64();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsDouble())
				{
					double jvalue = json_value.GetDouble();
					value = (jvalue != 0);
					return;
				}
				if (json_value.IsString())
				{
					int64_t jvalue = atol(json_value.GetString());
					value = (jvalue != 0);
					return;
				}
			}
		};

		template<>
		struct integer_arithmetic_support_write_impl < bool >
		{
			typedef bool value_type;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value)
			{
				if (value)
				{
					store_data.write("1", 1);
				}
				else
				{
					store_data.write("0", 1);
				}
			}
		};

		template <typename ty, typename jsonvalue_type>
		struct arithmetic_support_read_impl
		{
			typedef typename ty value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_integral<value_type>,
        integer_arithmetic_support_read_impl<value_type, jsonvalue_type>,
        float_arithmetic_support_read_impl<value_type, jsonvalue_type>
			>::type impl;
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
		};

		template<typename ty,typename jsonvalue_type>
		struct string_support_read_impl
		{
			typedef typename ty value_type;

			static void read( jsonvalue_type& json_value, value_type& value)
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
						value = "1";
					}
					else
					{
						value = "0";
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

    template<typename ty>
		struct string_support_write_impl
		{
			typedef ty value_type;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value)
			{
				static const char hexDigits[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
				static const char escape[256] = {
#define Z16 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
					//0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
					'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'b', 't', 'n', 'u', 'f', 'r', 'u', 'u', // 00
					'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', 'u', // 10
					0, 0, '"', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20
					Z16, Z16,																		// 30~4F
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\\', 0, 0, 0, // 50
					Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16, Z16								// 60~FF
#undef Z16
				};

				store_data.write("\"", 1);
				std::size_t length = value.length();
				const char * ptr = value.c_str();
				const char * end = ptr + length;
				while (ptr < end)
				{
					const char c = *ptr;
					++ptr;
					if (escape[(unsigned char)c])
					{
						store_data.write("\\", 1);
						store_data.write(&escape[(unsigned char)c], 1);
						if (escape[(unsigned char)c] == 'u')
						{
							char buff[4] = { '0', 'x' };
							buff[2] = (hexDigits[(unsigned char)c >> 4]);
							buff[3] = (hexDigits[(unsigned char)c & 0xF]);
							store_data.write(buff, 4);
						}
					}
					else
					{
						store_data.write(&c, 1);
					}
				}
				store_data.write("\"", 1);
			}
		};

    template<typename char_traits_ty, typename char_alloc_type, typename jsonvalue_type , int tag>
    struct value_support_read_impl< ::std::basic_string<char, char_traits_ty, char_alloc_type>, jsonvalue_type , tag>
		{
			typedef ::std::basic_string<char, char_traits_ty, char_alloc_type> value_type;
      typedef typename string_support_read_impl<value_type, jsonvalue_type> impl;
		};

    template<typename char_traits_ty, typename char_alloc_type, int tag>
    struct value_support_write_impl< ::std::basic_string<char, char_traits_ty, char_alloc_type>, tag>
		{
			typedef ::std::basic_string<char, char_traits_ty, char_alloc_type> value_type;
      typedef typename string_support_write_impl<value_type> impl;
		};

    template<typename ty, typename jsonvalue_type, int tag>
		struct container_seq_support_read_impl
		{
			typedef ty value_type;

			static void read( jsonvalue_type& json_value, value_type& value)
			{
				value.clear();
				if(json_value.IsArray())
				{
					::rapidjson::SizeType len = json_value.Size();
					value.resize(len);
					::rapidjson::SizeType i = 0;
					for (ty::iterator iter = value.begin(); iter != value.end();++iter,++i)
					{
						value_support_read<typename ty::value_type,tag>::impl::read(json_value[i],*iter);
					}
				}
				return ;
			}
		};

    template<typename ty, int tag>
		struct container_seq_support_write_impl
		{
			typedef ty value_type;
			template<typename store_ty>
			static void write(store_ty& store_data, const value_type& value)
			{
				store_data.write("[", 1);
				std::size_t len = 0;
				for (typename ty::const_iterator i = value.begin(); i != value.end(); ++i, ++len)
				{
					value_support_write<typename value_type::value_type, tag>::impl::write(store_data, *i);
					if (len < value.size() - 1)
					{
						store_data.write(",", 1);
					}
				}
				store_data.write("]", 1);
			}
		};

    template<typename ty, typename jsonvalue_type, typename alloc_ty, int tag>
    struct value_support_read_impl< ::std::list<ty, alloc_ty>, jsonvalue_type, tag>
		{
			typedef typename ::std::list<ty,alloc_ty> value_type;
      typedef container_seq_support_read_impl<value_type, jsonvalue_type,tag> impl;
		};

		template<typename ty, typename alloc_ty, int tag>
    struct value_support_write_impl< ::std::list<ty, alloc_ty>, tag>
		{
			typedef typename ::std::list<ty,alloc_ty> value_type;
      typedef container_seq_support_write_impl<value_type,tag> impl;
		};

    template<typename ty, typename jsonvalue_type, typename alloc_ty, int tag>
    struct value_support_read_impl< ::std::deque<ty, alloc_ty>, jsonvalue_type,tag>
		{
			typedef typename ::std::deque<ty,alloc_ty> value_type;
      typedef container_seq_support_read_impl<value_type, jsonvalue_type,tag> impl;
		};

		template<typename ty, typename alloc_ty, int tag>
    struct value_support_write_impl< ::std::deque<ty, alloc_ty> , tag>
		{
			typedef typename ::std::deque<ty,alloc_ty> value_type;
      typedef container_seq_support_write_impl<value_type,tag> impl;
		};

    template<typename ty, typename jsonvalue_type, typename alloc_ty, int tag>
    struct value_support_read_impl< ::std::vector<ty, alloc_ty>, jsonvalue_type, tag>
		{
			typedef typename ::std::vector<ty,alloc_ty> value_type;
      typedef container_seq_support_read_impl<value_type, jsonvalue_type,tag> impl;
		};

    template<typename ty, typename alloc_ty, int tag>
    struct value_support_write_impl< ::std::vector<ty, alloc_ty>, tag>
		{
			typedef typename ::std::vector<ty,alloc_ty> value_type;
      typedef container_seq_support_write_impl<value_type,tag> impl;
		};


    template <typename ty, typename jsonvalue_type, int tag>
		struct value_support_read
		{
			typedef typename ::boost::remove_const<ty>::type value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_arithmetic<value_type>,
        typename arithmetic_support_read_impl< value_type, jsonvalue_type>::impl,
        typename value_support_read_impl<value_type, jsonvalue_type, tag>::impl
			>::type impl;
		};

    template <typename ty , int tag>
		struct value_support_write
		{
			typedef typename ::boost::remove_const<ty>::type value_type;
			typedef typename ::boost::mpl::if_<
				::boost::is_arithmetic<value_type>,
        typename arithmetic_support_write_impl< value_type>::impl,
        typename value_support_write_impl<value_type, tag>::impl
			>::type impl;
		};

		template<int tag , typename ty , typename jsonvalue_type>
    void ajson_readx(jsonvalue_type& json_value, ty& value)
		{
      value_support_read<ty, jsonvalue_type, tag>::impl::read(json_value, value);
		}

		template<int tag, typename ty, typename store_ty>
		void ajson_writex(store_ty& store, ty& value)
		{
			value_support_write<ty, tag>::impl::write(store, value);
		}

    template<typename ty, typename jsonvalue_type>
		void ajson_read( jsonvalue_type& json_value , ty& value)
		{
			ajson_readx<0>(json_value,value);
		}

		template<typename ty, typename store_ty>
		void ajson_write(store_ty& store, ty& value)
		{
			ajson_writex<0, ty, store_ty>::impl::write(store, value);
		}

    template<int tag, typename ty, typename jsonvalue_type>
		inline bool load_from_nodex(ty& value , const jsonvalue_type& node)
		{
			ajson_readx<tag>(node,value);
		}

    template<typename ty, typename jsonvalue_type>
		inline bool load_from_node(ty& value , const jsonvalue_type& node)
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
#ifdef _MSC_VER
        ::std::size_t offset = ::sprintf_s(error_str, "error occurred %s near ", ::rapidjson::GetParseError_En(document.GetParseError()));
#else
        ::std::size_t offset = ::sprintf(error_str, "error occurred %s near ", ::rapidjson::GetParseError_En(document.GetParseError));
#endif
				memcpy(error_str+offset,error_offset,len);
				error_str[offset+len] = 0;
				error_message = error_str;
				return false;
			}
      ::rapidjson::Document::ValueType& json_value = document;
			ajson_readx<tag>(json_value,value);
      return true;
		}

		template<typename ty , typename string_ty>
		inline bool load_from_buff(ty& value , const char * data , string_ty& error_message)
		{
			return load_from_buffx<0>(value,data,error_message);
		}

		template<int tag, typename ty, typename stroe_ty>
		inline bool save_to_buffx(stroe_ty& buff, ty& value)
		{
			ajson_writex<tag>(buff, value);
			buff.write("\0", 1);
			return true;
		}

		template<typename ty, typename stroe_ty>
		inline bool save_to_buff(ty& value, stroe_ty& buff)
		{
			return save_to_buffx<0>(buff, value);
		}

		template<int tag,typename ty , typename string_ty>
		inline bool load_from_filex(ty& value , char * filename , string_ty& error_message)
		{
			::rapidjson::Document document;
			char readBuffer[4096];
			FILE * fd;
			if( 0 != ::fopen_s(&fd,filename,"r"))
			{
				error_message = "open file error.";
				return false;
			}
			::rapidjson::FileReadStream is(fd, readBuffer, sizeof(readBuffer));
      document.ParseStream<0, ::rapidjson::UTF8<>>(is);
			if(document.HasParseError())
			{
				char error_str[256];
#ifdef _MSC_VER
        ::std::size_t offset = ::sprintf_s(error_str, "error occurred %s near ", ::rapidjson::GetParseError_En(document.GetParseError), res.Offset());
#else
        ::std::size_t offset = ::sprintf(error_str, "error occurred %s near  ", ::rapidjson::GetParseError_En(document.GetParseError), res.Offset());
#endif
				fseek(fd,(int)document.GetErrorOffset(),SEEK_SET);
				offset += fread(error_str+offset,1,50,fd);
				error_str[offset] = 0;
				error_message = error_str;
				::fclose(fd);
				return false;
			}
			::fclose(fd);
      ::rapidjson::Document::ValueType& json_value = document;
      ajson_readx<tag>(json_value, value);
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
			ajson_file_stream outf(filename);
			if (!outf.bad())
			{
				ajson_writex<tag, ty, ajson_file_stream>(outf, value);
			}
			else
			{
				error_message = "error open file";
				return false;
			}
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
	if ( member_ptr != json_value.MemberEnd() )\
	{\
		::boost::ajson::ajson_readx< v >( member_ptr->value , value.elem);\
	}
	

#define AJSON_WRITE_MEMBER( r ,v , elem ) \
	member_name = BOOST_DO_STRINGIZE(elem);\
	store_data.write("\"",1); store_data.write(member_name,strlen(member_name));store_data.write("\"",1);store_data.write(":",1);\
	::boost::ajson::ajson_writex< v >(store_data , value.elem);\
	store_data.write(",",1);\
	++member_count;

#define AJSONX(TYPE, MEMBERS,X)\
namespace boost\
{\
	namespace ajson\
	{\
		template<typename jsonvalue_type>\
		struct value_support_read_impl<TYPE,jsonvalue_type,X>\
		{\
			typedef TYPE value_type;	\
			typedef value_support_read_impl<TYPE,jsonvalue_type,X> impl;\
			static inline void read(const jsonvalue_type& json_value , value_type& value)\
			{\
				jsonvalue_type::ConstMemberIterator member_ptr;\
				BOOST_PP_SEQ_FOR_EACH( AJSON_READ_MEMBER , X , MEMBERS ) \
				return;\
			}\
		};\
		template <>\
		struct value_support_write_impl<TYPE,X>\
		{\
			typedef TYPE value_type;	\
			typedef value_support_write_impl<TYPE,X> impl;\
			template<typename store_ty>\
			static inline void write(store_ty& store_data , const value_type& value)\
			{\
				char * member_name = NULL;\
				int member_count = 0;\
				store_data.write("{",1);\
				BOOST_PP_SEQ_FOR_EACH( AJSON_WRITE_MEMBER , X , MEMBERS ) \
				if(member_count){ store_data.seekp(-1,std::ios::cur); }\
				store_data.write("}",1);\
				return;\
			}\
		};\
	}\
}

#define AJSON(TYPE, MEMBERS) AJSONX(TYPE, MEMBERS , 0)
#endif
