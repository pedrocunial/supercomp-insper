import numpy as np
import timeit
import concurrent.futures as cf


def rbf(x, y, out):
    n = len(x)
    for i in range(n):
        out[i] = np.exp(-(x[i] - y[i])**2)

def rbf_threads(x, y, out):
    with cf.ThreadPoolExecutor(max_workers=4) as executor:
        n = 20
        size = len(out)
        step = size // n
        for i in range(0, size, step):
            executor.submit(rbf, x[i:i+step], y[i:i+step], out[i:i+step])


def rbf_processos(x, y, out):
    with cf.ProcessPoolExecutor(max_workers=4) as executor:
        n = 20
        size = len(out)
        step = size // n
        for i in range(0, size, step):
            executor.submit(rbf, x[i:i+step], y[i:i+step], out[i:i+step])

def measure_time(code):
    return timeit.timeit(code, globals=globals(), number=2)

if __name__ == '__main__':
    np.random.seed(1010)
    n = 1000000
    x = np.random.rand(n) * 10
    y = np.random.rand(n) * 10
    out = np.zeros(n)
    out2 = np.zeros(n)
    out3 = np.zeros(n)

    print('RBF sequencial', measure_time('rbf(x, y, out)'))
    print('RBF threads', measure_time('rbf_threads(x, y, out2)'))
    print('RBF processos', measure_time('rbf_processos(x, y, out3)'))
    assert np.all(np.isclose(out, out2))
    print(out3)
    assert np.all(np.isclose(out, out3))
