import math
import cmath
import time

def is_prime(p):
    if p <= 1:
        return False
    
    if p <= 3:
        return True
    
    if p % 2 == 0:
        return False

    m = int(math.sqrt(p)) + 1
    for i in range(3, m + 1, 2):
        if p % i == 0:
            return False

    return True

def is_power_of(x, p):
    while x % p == 0 and x > 1:
        x /= p
    
    return x == 1

def fft(v, n, k = 1):
    if abs(n) == abs(k):
        return sum(v)
    
    u = [list(), list()]

    for i in range(len(v)):
        u[i % 2].append(v[i])

    w = [fft(u[0], n, k << 1), fft(u[1], n, k << 1)]

    a = [0]*int(abs(n / k))
    i = 0
    j = len(a) >> 1

    while i < j:
        m = cmath.exp((2 * math.pi * k * i / n) * complex(0, 1))
        
        a[i] = a[i + j] = m * w[1][i]
        a[i] = w[0][i] + a[i]
        a[i + j] = w[0][i] - a[i + j]

        i += 1

    return a

def multiply(p, q):
    n = 1 << int(math.log2(len(p) + len(q) - 2) + 1)

    a = fft(p, n)
    b = fft(q, n)
    c = list()
    d = list()

    for i in range(n):
        c.append(a[i] * b[i])
    
    for m in fft(c, n, -1):
        d.append(round((m / n).real))
    
    while len(d) > 0 and d[len(d) - 1] == 0:
        d.pop()
    
    return d

# foil
def multiply_v1(p, q):
    v = [0]*(len(p) + len(q))

    for n in range(len(p)):
        for m in range(len(q)):
            v[n + m] += p[n] * q[m]

    while len(v) > 0 and v[len(v) - 1] == 0:
        v.pop()

    return v
   
def subtract(p, q):
    v = p.copy()

    m = max(len(p), len(q))
    if len(v) < m:
        v.extend([0]*(m - v))

    for n in range(len(q)):
        v[len(q) - 1 - n] -= q[len(q) - 1 - n]
    
    while len(v) > 0 and v[len(v) - 1] == 0:
        v.pop()

    return v

def divide(p, q):
    terms = [0]*(max(len(p), len(q)) + 1)

    while ((len(p) == len(q) and p[len(p) - 1] == q[len(q) - 1]) or (len(p) > len(q) or p[len(p) - 1] > q[len(q) - 1])):
        term = [0]*(len(p) - len(q) + 1)
        term[len(term) - 1] = terms[len(term) - 1] = p[len(p) - 1] / q[len(q) - 1]

        p = subtract(p, multiply_v1(q, term))

        if len(p) == 0:
            break
    
    while len(terms) > 0 and terms[len(terms) - 1] == 0:
        terms.pop()

    return terms

def cyclotomic_polynomial(N):

    if N == 1:
        return [1, -1]
    
    if is_prime(N):
        return [1]*N
    
    if N % 2 == 0 and ((N / 2) % 2 != 0) and is_prime(N / 2):
        v = list()
        for i in range(int(N / 2)):
            v.append(1 if i % 2 == 0 else -1)
        return v

    if N > 1 and is_power_of(N, 2):
        v = [0]*(int(N / 2) + 1)
        v[0] = 1
        v[len(v) - 1] = 1
        return v

    if ((N % 12 == 0) and is_power_of(N / 12, 2)) or ((N % 18 == 0) and is_power_of(N / 18, 2)):
        v = [0]*(int(N / 3) + 1)
        v[0] = 1
        v[int(len(v) / 2)] = -1
        v[len(v) - 1] = 1
        return v

    if math.gcd(N, 9) == 9 and is_power_of(N, 3):
        v = [0]*(int(N / 1.5) + 1)
        v[0] = 1
        v[int(len(v) / 2)] = 1
        v[len(v) - 1] = 1
        return v
    
    p = [1]

    # factors of N
    for i in range(1, int(N / 2) + 1):
        if N % i == 0:
            q = cyclotomic_polynomial(i)
            q.reverse()
            p = multiply_v1(p, q)
    
    q = [0]*(N + 1)
    q[0] = -1
    q[len(q) - 1] = 1

    v = divide(q, p)
    v.reverse()

    return v

    

def main():
    start = time.time()
    for i in range(1, 501):
        print(str(i) + ':', cyclotomic_polynomial(i))
    end = time.time()
    print(end - start)

main()
