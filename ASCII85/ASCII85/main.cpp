#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include <algorithm>
#include <string>


struct block{
	const char *data;
	std::size_t n;	
};

std::string toAscii85(block b){
	std::string str = "<~";

	unsigned long n = 0;
	char shift = 32;

	const unsigned char * pc = (const unsigned char *)(b.data);
	unsigned cnt = b.n;

	while(cnt--){
		shift -= 8;
		n += (unsigned long)(*pc++) << shift;

		if(!shift){
			if(n){
				str.resize(str.size() + 5);
				auto b = std::prev(str.end()), e = std::prev(str.end(), 5);
				do{
					*b = char(33 + n % 85);
					n /= 85;
				} while(e <= --b);
			} else
				str += 'z';

			shift = 32;
		}
	}

	if(shift != 32){
		if(n){
			str.resize(str.size() + 5);
			auto b = std::prev(str.end()), e = std::prev(str.end(), 5);
			do{
				*b = char(33 + n % 85);
				n /= 85;
			} while(e <= --b);
			str.resize(str.size() - shift / 8);
		} else
			for(int i = 0, l = 5 - shift / 8; i < l; ++i){
				str.resize(str.size() + 1);
				str[str.size() - 1] = '!';
			};
	}


	str += "~>";

	return str;
}

block fromAscii85(const const std::string & str_in){
	std::string str;

	const unsigned char *e, *pc = (const unsigned char*)str_in.c_str();
	{	//поиск начала (встаем на конце "<~")
		const unsigned char *p = pc;
		while(++pc, *pc && (*p != '<' || *pc != '~'))
			if(33 <= *pc && (*pc <= 117 || *pc == '~')) p = pc;
		if(!*pc){
			pc = (const unsigned char*)str_in.c_str() - 1;
			e = p = pc + 1;
		} else
			e = p = pc;
		//поиск конца (встаем на начале "~>")
		while(++p, *p && (*e != '~' || *p != '>'))
			if(33 <= *p && *p <= 117 || *p == '~')	e = p;
		if(!*p) e = p;

	}
	unsigned long long n = 0,
		m = 85ULL * 85 * 85 * 85 * 85;
	while(++pc<e){
		if(*pc == 'z')
			str.resize(str.size() + 4);
		else if(*pc<33 || 117<*pc)
			;//пропускаем все пробельные и прочие символы 
		else{
			m /= 85;
			n += (*pc - 33) * m;

			if(m == 1){
				str.resize(str.size() + 4);
				auto it = str.end();
				n = n & 0xffffffff;
				while(n){
					*--it = char(n & 255);
					n >>= 8;
				}

				m = 85ULL * 85 * 85 * 85 * 85;
			}
		}
	}

	if(m != 85ULL * 85 * 85 * 85 * 85){
		switch(m){
			case 85ULL * 85 * 85 * 85:
				n += ('u' - 33) * 85ULL * 85 * 85;
			case 85ULL * 85 * 85:
				n += ('u' - 33) * 85ULL * 85;
			case 85ULL * 85:
				n += ('u' - 33) * 85ULL;
			case 85ULL:
				n += ('u' - 33);
		}
		str.resize(str.size() + 4);
		auto it = str.end();
		n = n & 0xffffffff;
		while(n){
			*--it = (unsigned char)(n & 255);
			n >>= 8;
		}
		switch(m){
			case 85ULL * 85 * 85 * 85:
				str.resize(str.size() - 4);
				break;
			case 85ULL * 85 * 85:
				str.resize(str.size() - 3);
				break;
			case 85ULL * 85:
				str.resize(str.size() - 2);
				break;
			case 85ULL:
				str.resize(str.size() - 1);
		}
	}

	char * res = new char[str.size()];
	std::memcpy(res, str.c_str(), str.size());

	return {res, str.size()};
}

int main(){
	block b;
	b.data = "Hello, there!";
	b.n    = 13;
	std::string res = toAscii85(std::move(b));
	std::cout << res << std::endl << std::endl;

	
	b = std::move(fromAscii85(res));
	std::cout << b.data;

	delete[] b.data;

	return 0;
}