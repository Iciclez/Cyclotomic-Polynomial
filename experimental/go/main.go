package main

import (
	"fmt"
	"time"
	"math"
)

func reverse(v []int) {
	for i, j := 0, len(v) - 1; i < j; i, j = i+1, j-1 {
		v[i], v[j] = v[j], v[i]
	}
}

func subtract(p []int, q []int) []int {

	v := make([]int, len(p))
	copy(v, p)

	if len(q) > len(p) {
		for len(v) < len(q) {
			v = append(v, 0)
		}
	}

	for n := 0; n < len(q); n++ {
		v[len(q) - 1 - n] -= q[len(q) - 1 - n]
	}

	for len(v) > 0 && v[len(v) - 1] == 0 {
		v = v[:len(v) - 1]
	}
	
	return v
}

func multiply(p []int, q []int) []int {

	v := make([]int, len(p) + len(q))

	for n := 0; n < len(p); n++ {
		for m := 0; m < len(q); m++ {
			v[n + m] += p[n] * q[m]
		}
	}

	for len(v) > 0 && v[len(v) - 1] == 0 {
		v = v[:len(v) - 1]
	}
	
	return v
}


func divide_v1(p []int, q []int) []int {

	var size int

	if len(p) > len(q) {
		size = len(p)
	} else {
		size = len(q)
	}

	terms := make([]int, size + 1)

	for (len(p) == len(q) && p[len(p) - 1] == q[len(q) - 1]) || (len(p) > len(q) || p[len(p) - 1] > q[len(q) - 1]) {

		term := make([]int, len(p) - len(q) + 1)
		
		term[len(term) - 1] = p[len(p) - 1] / q[len(q) - 1]
		terms[len(term) - 1] = term[len(term) - 1]

		p = subtract(p, multiply(q, term))

		if len(p) == 0 {
			break
		}
		
	}

	for len(terms) > 0 && terms[len(terms) - 1] == 0 {
		terms = terms[:len(terms) - 1]
	}

	return terms
}

func divide(p []int, q []int) []int {
	/* p denotes dividend, q denotes divisor */
	
	dividend := make([]int, len(p))
	divisor := make([]int, len(q))

	copy(dividend, p)
	copy(divisor, q)

	return divide_v1(dividend, divisor)
}

func cyclotomicPolynomial(N uint, cache map[uint][]int) []int {
	if N == 1 {
		return []int {1, -1}
	}

	cacheValue, inCache := cache[N]
	if inCache {
		return cacheValue
	}

	prime := func(p int) bool {
		if p <= 1 {
			return false
		}

		if p <= 3 {
			return true
		}

		if p % 2 == 0 {
			return false
		}

		m := int(math.Sqrt(float64(p))) + 1
		for i := 3; i <= m; i += 2 {
			if p % i == 0  {
				return false
			}
		}

		return true
	}

	powerOf := func(x uint, p uint) bool {
		for ; x % p == 0 && x > 1; x /= p { }
		return x == 1
	}

	gcd := func(a int, b int) int {
		for b != 0 {
			t := b
			b = a % b
			a = t
		}
		return a
	}

	if prime(int(N)) {
		v := make([]int, N)
		for i := 0; i < len(v); i++ {
			v[i] = 1
		}
		return v
	}

	if (N % 2 == 0) && ((N / 2) % 2 != 0) && prime(int(N / 2)) {
		v := make([]int, int(N / 2))
		for i := 0; i < len(v); i++ {
			if i % 2 == 0 {
				v[i] = 1
			} else {
				v[i] = -1
			}
		}
		return v
	}

	if N > 1 && powerOf(uint(N), 2) {
		v := make([]int, int(N / 2) + 1)
		v[0] = 1
		v[len(v) - 1] = 1
		return v
	}

	if ((N % 12 == 0) && powerOf(N / 12, 2)) || ((N % 18 == 0) && powerOf(N / 18, 2)) {
		v := make([]int, int(N / 3) + 1)
		v[0] = 1
		v[int(len(v) / 2)] = -1
		v[len(v) - 1] = 1
		return v
	}

	if gcd(int(N), 9) == 9 && powerOf(N, 3) {
		v := make([]int, int(float64(N) / 1.5) + 1)
		v[0] = 1
		v[int(len(v) / 2)] = 1
		v[len(v) - 1] = 1
		return v
	}

	p := []int { 1 }
	n := int(N / 2)

	// factors of N
	for i := 1; i <= n; i++ {
		if int(N) % i == 0 {
			q := cyclotomicPolynomial(uint(i), cache)
			reverse(q)
			p = multiply(p, q)
		}
	}

	q := make([]int, N + 1)
	q[0] = -1
	q[len(q) - 1] = 1

	v := divide(q, p)
	reverse(v)
	cache[N] = v
	return v
}

func main() {

	cache := make(map[uint][]int)
	start := time.Now()

	for i := 1; i <= 500; i++ {
		fmt.Printf("%d: %v\n", i, cyclotomicPolynomial(uint(i), cache))
	}

	elapsed := time.Since(start)

	fmt.Println(elapsed)
}