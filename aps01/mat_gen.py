X = 150

print("0.1 0.1", X, X)
print("0.00012345")

for i in range(X):
    print("100 ", end='')

print()

for i in range(X-1):
    for j in range(X):
        if j == 0:
            print("75 ", end='')
        elif j == X - 1:
            print("150")
        else:
            print("0 ", end='')
