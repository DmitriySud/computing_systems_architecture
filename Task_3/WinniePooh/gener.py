import random

primes = list({ 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71 })

def gen_number():
    res = 1
    cnt = 0
    while (res < 1000000): 
        res *= primes[random.randint(0, len(primes) - 1)]
        cnt +=1
    if (cnt == 7):
        res *= 2
    return res


def gen_number_7():
    res = 10 ** 10
    while res > 100000000 :
        res = 1
        for i in range(7): res *= primes[random.randint(0, len(primes) - 1)]
    return res


for test in range(10): 
    path  = "input\\test%d.txt" % test

    file = open(path, "w")

    n = random.randint(3, 20)
    m = random.randint(5, 20)
    k = random.randint(1, n+1)

    file.write("%d %d %d\n" % (n,m,k))

    field = list()
    for i in range(n):
        field.append(list())
        for j in range(m):
            field[i].append(gen_number())
            
    winnie_i = random.randint(0,n-1)
    winnie_j = random.randint(0,m-1)

    field[winnie_i][winnie_j] = gen_number_7()

    for i in range(n):
        for j in range(m):
            file.write("%d " % field[i][j])
        file.write("\n")
    file.write("Winnie Pooh : [%d][%d]" % (winnie_i, winnie_j))
    file.close()
