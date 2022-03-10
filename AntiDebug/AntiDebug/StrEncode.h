
namespace crypt {
#define XSTR_SEED ((__TIME__[7] - '0') * 1ull    + (__TIME__[6] - '0') * 10ull  + \
                   (__TIME__[4] - '0') * 60ull   + (__TIME__[3] - '0') * 600ull + \
                   (__TIME__[1] - '0') * 3600ull + (__TIME__[0] - '0') * 36000ull + __COUNTER__)
    constexpr unsigned long long linear_congruent_generator(unsigned rounds) {
        return 1013904223ull + (1664525ull * ((rounds > 0) ? linear_congruent_generator(rounds - 1) : (XSTR_SEED))) % 0xFFFFFFFF;
    }
#define Random() linear_congruent_generator(10)
#define XSTR_RANDOM_NUMBER(Min, Max) (Min + (Random() % (Max - Min + 1)))

    constexpr const unsigned long long XORKEY = XSTR_RANDOM_NUMBER(0, 0xFE);
    constexpr const unsigned long long XORKEY0 = XSTR_RANDOM_NUMBER(0, 0xE0);
    constexpr const unsigned long long XORKEY1 = XSTR_RANDOM_NUMBER(0, 0xE1);
    constexpr const unsigned long long XORKEY2 = XSTR_RANDOM_NUMBER(0, 0xE2);
    constexpr const unsigned long long XORKEY3 = XSTR_RANDOM_NUMBER(0, 0xE3);
    constexpr const unsigned long long XORKEY4 = XSTR_RANDOM_NUMBER(0, 0xE4);
    constexpr const unsigned long long XORKEY5 = XSTR_RANDOM_NUMBER(0, 0xE5);
    constexpr const unsigned long long XORKEY6 = XSTR_RANDOM_NUMBER(0, 0xE6);
    constexpr const unsigned long long XORKEY7 = XSTR_RANDOM_NUMBER(0, 0xE7);
    constexpr const unsigned long long XORKEY8 = XSTR_RANDOM_NUMBER(0, 0xE8);
    constexpr const unsigned long long XORKEY9 = XSTR_RANDOM_NUMBER(0, 0xE9);

    template<typename Char >
    constexpr Char encrypt_character(const Char character, int index) {
        return character ^ (static_cast<Char>(XORKEY) + index);
    }

    template <typename Char, unsigned size, unsigned long long counter>
    class Xor_string {
    public:
        Char _string[size * 7];
        const unsigned char _counter = (unsigned char)counter;
        const unsigned _nb_chars = ((size - 1) ^ _counter);
        inline constexpr Xor_string(const Char* string) : _string{}
        {
            for (unsigned i = 0u; i < size; ++i)
            {
                _string[i * 7 + 0] = (Char)(XORKEY0 ^ _counter);    //干扰
                _string[i * 7 + 1] = (Char)(XORKEY1 ^ _counter);    //干扰
                _string[i * 7 + 2] = (Char)(XORKEY2 ^ _counter);    //干扰
                _string[i * 7 + 3] = (Char)(XORKEY3 ^ _counter);    //干扰
                _string[i * 7 + 4] = (Char)(XORKEY4 ^ _counter);    //干扰
                _string[i * 7 + 5] = (Char)(XORKEY5 ^ _counter);    //干扰
                _string[i * 7 + 6] = (Char)(XORKEY6 ^ _counter);    //干扰
                _string[i * 7 + (_counter % 7)] = encrypt_character<Char>(string[i], i); //真实数据
            }
        }
        const Char* decrypt() const
        {
            Char* string = const_cast<Char*>(_string);
            unsigned __nb_chars = _nb_chars;
            __nb_chars ^= (unsigned char)counter;
            for (unsigned t = 0; t < __nb_chars; t++)
            {
                string[t] = _string[t * 7 + ((unsigned char)counter % 7)] ^ (static_cast<Char>(XORKEY) + t);
            }
            //string[_nb_chars] = '\0';
            string[__nb_chars] &= 0x70;
            string[__nb_chars] &= 0x07;
            return string;
        }

    };

}

#define XorS(name, my_string)    constexpr crypt::Xor_string<char, (sizeof(my_string) / sizeof(char)), __COUNTER__*__COUNTER__> name(my_string)
#define XorString(my_string) []{ constexpr crypt::Xor_string<char, (sizeof(my_string) / sizeof(char)), __COUNTER__*__COUNTER__> expr(my_string); return expr; }().decrypt()
#define _c( string ) XorString( string )
#define XorWS(name, my_string)       constexpr crypt::Xor_string<wchar_t, (sizeof(my_string) / sizeof(wchar_t)), __COUNTER__*__COUNTER__> name(my_string)
#define XorWideString(my_string) []{ constexpr crypt::Xor_string<wchar_t, (sizeof(my_string) / sizeof(wchar_t)), __COUNTER__*__COUNTER__> expr(my_string); return expr; }().decrypt()
#define _cw( string ) XorWideString( string )








