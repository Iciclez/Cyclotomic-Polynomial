#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <algorithm>
#include <cstdint>
#include <vector>
#include <sstream>
#include <iostream>
#include <functional>
#include <numeric>
#include <chrono>
#include <unordered_map>
#include <array>

std::ostream& operator<<(std::ostream& os, const std::vector<int32_t>& v)
{
	std::stringstream ss;
	if (v.size() > 0)
	{
		ss << '[';

		for (size_t n = 0; n < v.size(); ++n)
		{
			ss << v.at(n);
			if (v.size() - 1 == n)
			{
				ss << ']';
			}
			else
			{
				ss << ", ";
			}
		}

		return os << ss.str();
	}

	return os;
}

const std::vector<int32_t> subtract(const std::vector<int32_t>& p, const std::vector<int32_t>& q)
{
	std::vector<int32_t> v(p);
	v.resize(std::max(p.size(), q.size()));

	for (size_t n = 0; n < q.size(); ++n)
	{
		v.at(q.size() - 1 - n) -= q.at(q.size() - 1 - n);
	}

	while (!v.empty() && v.back() == 0)
	{
		v.pop_back();
	}

	return v;
}

const std::vector<std::complex<long double>> itfft(const std::vector<std::complex<long double>>& v, int32_t n)
{
	/* TODO: finish impl iterative fft */
	
	std::function<std::vector<std::complex<long double>>(const std::vector<std::complex<long double>>&)> bit_reverse =
		[](const std::vector<std::complex<long double>> & a) -> std::vector<std::complex<long double>>
	{
		std::vector<std::complex<long double>> A;

		for (size_t k = 0; k < a.size(); ++k)
		{
			//A.at(k)
		}

		return A;
	};

	std::vector<std::complex<long double>> A = bit_reverse(v);
	for (size_t s = 1, i = static_cast<size_t>(log2(v.size())); s <= i; ++s)
	{
		int32_t m = 1 << s; //std::pow(2, s);
		std::complex<long double> wm = std::complex<long double>(std::exp(-2 * M_PI / m)) * std::complex<long double>(0, 1);
		
		for (size_t k = 0; k < v.size(); k += m)
		{
			std::complex<long double> w(1);

			for (size_t j = 0, p = m / 2; j < p; ++j)
			{
				std::complex<long double> t = w * A.at(k + j + m / 2);
				std::complex<long double> u = w * A.at(k + j);

				A.at(k + j) = u + t;
				A.at(k + j + m / 2) = u - t;

				w = w * wm;
			}
		}
	}

	return A;
}

const std::vector<std::complex<long double>> fft(const std::vector<std::complex<long double>>& v, int32_t n, int32_t k = 1)
{
	if (abs(n) == abs(k))
	{
		return { std::accumulate(v.begin(), v.end(), std::complex<long double>()) };
	}

	std::array<std::vector<std::complex<long double>>, 2> u { };

	u.at(0).reserve(v.size() + 1);
	u.at(1).reserve(v.size() + 1);

	for (size_t i = 0; i < v.size(); ++i)
	{
		u.at(i % 2).push_back(v.at(i));
	}

	std::array<std::vector<std::complex<long double>>, 2> w{ fft(u.at(0), n, k << 1), fft(u.at(1), n, k << 1) };

	std::vector<std::complex<long double>> a;
	a.resize(abs(n / k));

	std::complex<long double> m;
	for (size_t i = 0, j = a.size() >> 1; i < j; ++i)
	{
		m = std::exp(std::complex<long double>(2 * M_PI * k * i / n) * std::complex<long double>(0, 1));

		a.at(i) = a.at(i + j) = m * w.at(1).at(i);
		a.at(i) = w.at(0).at(i) + a.at(i);
		a.at(i + j) = w.at(0).at(i) - a.at(i + j);
	}

	return a;
}

const std::vector<int32_t> multiply(const std::vector<int32_t> & p, const std::vector<int32_t> & q)
{
	int32_t n = 1 << static_cast<int32_t>(log2(p.size() + q.size() - 2) + 1);

	std::vector<std::complex<long double>> a = fft(std::vector<std::complex<long double>>(p.begin(), p.end()), n);
	std::vector<std::complex<long double>> b = fft(std::vector<std::complex<long double>>(q.begin(), q.end()), n);
	std::vector<std::complex<long double>> c;
	std::vector<int32_t> d;

	c.reserve(n);

	/* for (size_t i = 0; i < n; ++i) { c.push_back(a.at(i) * b.at(i)); } */
	std::transform(a.begin(), a.end(), b.begin(), std::back_inserter(c), std::multiplies<std::complex<long double>>());

	for (const std::complex<long double>& m : fft(c, n, -1))
	{
		d.push_back(static_cast<int32_t>(round((m / std::complex<long double>(n)).real())));
	}

	while (!d.empty() && d.back() == 0)
	{
		d.pop_back();
	}

	return d;
}

const std::vector<int32_t> multiply_v1(const std::vector<int32_t>& p, const std::vector<int32_t>& q)
{
	/* this version of multiply uses the foil method */
	std::vector<int32_t> v(p.size() + q.size(), 0);

	for (size_t n = 0; n < p.size(); ++n)
	{
		for (size_t m = 0; m < q.size(); ++m)
		{
			v.at(n + m) += p.at(n) * q.at(m);
		}
	}
	
	while (!v.empty() && v.back() == 0)
	{
		v.pop_back();
	}

	return v;
}

const std::vector<int32_t> divide_v1(std::vector<int32_t> p, std::vector<int32_t> q)
{
	/* p denotes dividend, q denotes divisor */
	std::vector<int32_t> terms(std::max(p.size(), q.size()) + 1, 0);

	while ((p.size() == q.size() && p.back() == q.back()) ||
		(p.size() > q.size() || p.back() > q.back()))
	{
		std::vector<int32_t> term(p.size() - q.size() + 1, 0);
		term.at(term.size() - 1) = terms.at(term.size() - 1) = p.back() / q.back();

		p = subtract(p, (p.size() == q.size() ? multiply : multiply_v1)(q, term));
		
		if (p.empty())
		{
			break;
		}

	}

	while (!terms.empty() && terms.back() == 0)
	{
		terms.pop_back();
	}

	return terms;
}

const std::vector<int32_t> divide(const std::vector<int32_t>& p, const std::vector<int32_t>& q)
{
	/* TODO: impl fft fast polynomial division */
	return divide_v1(p, q);
}

const std::vector<int32_t> cyclotomic_polynomial(uint32_t N)
{
	static std::unordered_map<uint32_t, std::vector<int32_t>> cache;

	if (N == 1)
	{
		return std::vector<int32_t>({ 1, -1 });
	}

	if (cache.count(N) > 0)
	{
		return cache.at(N);
	}

	std::function<bool(int32_t)> prime = [](int32_t p) -> bool
	{
		if (p <= 1)
		{
			return false;
		}

		if (p <= 3)
		{
			return true;
		}

		if (p % 2 == 0)
		{
			return false;
		}

		int32_t m = static_cast<int32_t>(sqrt(p)) + 1;
		for (int32_t i = 3; i <= m; i += 2)
		{
			if (p % i == 0)
			{
				return false;
			}
		}

		return true;
	};

	std::function<bool(uint32_t, int32_t)> power_of = [](uint32_t x, int32_t p) -> bool
	{
		while (!(x % p) && x > 1)
		{
			x /= p;
		}

		return x == 1;
	};

	std::vector<int32_t> v;

	if (prime(N))
	{
		v.resize(N, 1);
	}
	else if ((N % 2 == 0) && ((N / 2) % 2 != 0) && prime(N / 2))
	{
		int32_t n = N / 2;
		v.reserve(n);
		for (int32_t i = 0; i < n; ++i)
		{
			v.push_back((!(i % 2)) ? 1 : -1);
		}
	}
	else if (N > 1 && power_of(N, 2))
	{
		v.resize(static_cast<size_t>(N / 2) + 1);

		v.at(0) = 1;
		v.at(v.size() - 1) = 1;
	}
	else if (((N % 12 == 0) && power_of(N / 12, 2))
		|| ((N % 18 == 0) && power_of(N / 18, 2)))
	{
		v.resize(static_cast<size_t>(N / 3) + 1);

		v.at(0) = 1;
		v.at(v.size() / 2) = -1;
		v.at(v.size() - 1) = 1;
	}
	else if (std::gcd(N, 9) == 9 && power_of(N, 3))
	{
		v.resize(static_cast<size_t>(N / 1.5) + 1);

		v.at(0) = 1;
		v.at(v.size() / 2) = 1;
		v.at(v.size() - 1) = 1;
	}
	else
	{
		std::vector<int32_t> p = { 1 };
	
		//factors of N
		for (uint32_t i = 1, n = N / 2; i <= n; ++i)
		{
			if (N % i == 0)
			{
				std::vector<int32_t> q = cyclotomic_polynomial(i);
				p = multiply(p, std::vector<int32_t>(q.rbegin(), q.rend()));
			}
		}

		std::vector<int32_t> q(static_cast<size_t>(N) + 1, 0);

		q.at(0) = -1;
		q.at(q.size() - 1) = 1;

		v = divide(q, p);
		std::reverse(v.begin(), v.end());

		cache.emplace(N, v);
	}

	return v;
}

int main()
{
	auto start = std::chrono::high_resolution_clock::now();

	for (int32_t i = 1; i <= 500; ++i)
	{
		std::cout << i << ": " << cyclotomic_polynomial(i) << '\n';
	}

	auto end = std::chrono::high_resolution_clock::now();

	std::cout << " -> " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms\n";

	return 0;
}