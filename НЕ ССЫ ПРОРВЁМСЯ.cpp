#include <iostream>
#include <tuple>

//шаблон пустых и не пустых списков
template<int ...N>
struct IntList;
//€вна€ специализаци€ с выделением первого элемента списка
template<int N, int ...L>
struct IntList<N, L...> {
	static const int Head = N;
	using Tail = typename IntList<L...>;
};
//€вна€ специализаци€ пустого списка
template<>
struct IntList<> {};


template<int N, typename T>
struct IntCons;
template<int N, int ...L>
struct IntCons<N, IntList<L...>> {
	using type = IntList<N, L...>;
};

template<int N, int K = 0>
struct Generate {
	using type = typename IntCons<K, typename Generate<N - 1, K + 1>::type>::type;
};
template<int K>
struct Generate<1, K> {
	using type = IntList<K>;
};
template<>
struct Generate<0> {
	using type = IntList<>;
};

template<typename F, typename T, int ...I>
auto callFunc(F &f, T & t, IntList<I...>) {
	return f(std::get<I>(t) ...);
};
template<typename F, typename ...Arg>
auto apply(F &f, std::tuple<Arg...> &t) {
	return callFunc(f,t, typename Generate<sizeof ...(Arg)>::type());
};

template<typename T>
void print(std::ostream & os) {
	os << T::Head << std::endl;
	print<T::Tail>(os);
}
template<>
void print<IntList<>>(std::ostream & os) {}

// бинарна€ метафункци€
template<int a, int b>
struct Plus{
	static int const value = a + b;
};

template<typename A, typename B, template <int, int> class F >
struct Zip {
	using type = typename IntCons<F<A::Head, B::Head>::value,
		                         typename Zip<typename A::Tail, typename B::Tail, F>::type
	                             >::type;
};
template<template <int, int> class F>
struct Zip<IntList<>, IntList<>, F> {
	using type = typename IntList<>;
};

// бинарна€ метафункци€
template<int a, int b>
struct Minus {
	static int const value = a - b;
};

template<class L>
class Quantity {
private:
	double val;
public:
	explicit Quantity(double val = 0) :val(val) {}
	double  value() const noexcept { return val; }
	Quantity operator+(const Quantity & other) const noexcept {
		return Quantity(val + other.val);
	}
	Quantity operator-(const Quantity & other) const noexcept {
		return Quantity(val - other.val);
	}
	template<class B>
	Quantity<typename Zip<L, B, Plus>::type>
		operator*(const Quantity<B> & other) const noexcept {
		return Quantity<typename Zip<L, B, Plus>::type>(val * other.value());
	}
	Quantity operator*(double val) const noexcept {
		return Quantity<L>(this->val * val);
	}
	template<class B>
	Quantity<typename Zip<L, B, Minus>::type>
		operator/(const Quantity<B> & other) const noexcept {
		return Quantity<typename Zip<L, B, Minus>::type>(val / other.value());
	}
	Quantity operator/(double val) const noexcept {
		return Quantity<L>(this->val / val);
	}
};

template<class L>
Quantity<L> operator*(double val, const Quantity<L> & other) noexcept {
	return Quantity<L>(other.value() * val);
}
template<class L>
Quantity<typename Zip<IntList<0,0,0,0,0,0,0>, L, Minus>::type> operator/(double val, const Quantity<L> & other) noexcept {
	return Quantity<typename Zip<IntList<0, 0, 0, 0, 0, 0, 0>, L, Minus>::type>(val/other.value());
}

template<int m = 0, int kg = 0, int s = 0, int A = 0, int K = 0, int mol = 0, int cd = 0>
using Dimension = IntList<m, kg, s, A, K, mol, cd>;
using NumberQ   = Quantity<Dimension<>>;           // число без размерности
using LengthQ   = Quantity<Dimension<1>>;          // метры
using MassQ     = Quantity<Dimension<0, 1>>;       // килограммы
using TimeQ     = Quantity<Dimension<0, 0, 1>>;    // секунды
using VelocityQ = Quantity<Dimension<1, 0, -1>>;   // метры в секунду
using AccelQ    = Quantity<Dimension<1, 0, -2>>;   // ускорение, метры в секунду в квадрате
using ForceQ    = Quantity<Dimension<1, 1, -2>>;   // сила в ньютонах

int main() {
	
	LengthQ   l{ 30000 };      // 30 км
	TimeQ     t{ 10 * 60 };    // 10 минут
							   // вычисление скорости
	VelocityQ v = 1/(3 * l / t);     // результат типа VelocityQ, 50 м/с

	AccelQ    a{ 9.8 };        // ускорение свободного падени€
	MassQ     m{ 80 };         // 80 кг
							   // сила прит€жени€, котора€ действует на тело массой 80 кг
	ForceQ    f = m * a;     // результат типа ForceQ
	
	{
		// два списка одной длины
		using L1 = IntList<1, 2, 3, 4, 5>;
		using L2 = IntList<1, 3, 7, 7, 2>;

		// результат применени€ Ч список с поэлементными суммами
		int k = Plus<1, 2>::value;
		using L3 = Zip<L1, L2, Plus>::type;  // IntList<2, 5, 10, 11, 7>
		print<L3>(std::cout);

		auto f = [](int x, double y, double z) { return x + y + z; };
		auto t = std::make_tuple(30, 5, 1.6);  // std::tuple<int, double, double>
		auto res = apply(f, t);                // res = 36.6*/
	}
	return 0;

}