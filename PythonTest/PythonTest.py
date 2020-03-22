from itertools import islice
from random import random
from time import perf_counter

COUNT = 500000  # Change this value depending on the speed of your computer
DATA = list(islice(iter(lambda: (random() - 0.5) * 3.0, None), COUNT))

e = 2.7182818284590452353602874713527

def sinh(x):
    return (1 - (e ** (-2 * x))) / (2 * (e ** -x))

def cosh(x):
    return (1 + (e ** (-2 * x))) / (2 * (e ** -x))

def tanh(x):
    tanh_x = sinh(x) / cosh(x)
    return tanh_x

def test(fn, name):
    start = perf_counter()
    result = fn(DATA)
    duration = perf_counter() - start
    print('{} took {:.3f} seconds\n\n'.format(name, duration))

    for d in result:
        assert -1 <= d <= 1, " incorrect values"

# bot test
_MIN_PULSE_WIDTH = 600
_MAX_PULSE_WIDTH = 2400
_DEFAULT_PULSE_WIDTH = 1500
_FREQUENCY = 60
def map(x, in_min, in_max, out_min, out_max):
    '''To map the value from arange to another'''
    return (x - in_min) * (out_max - out_min) // (in_max - in_min) + out_min

def _angle_to_analog(angle):
    ''' Calculate 12-bit analog value from giving angle '''
    pulse_wide   = map(angle, 0, 180, _MIN_PULSE_WIDTH, _MAX_PULSE_WIDTH)
    analog_value = int(float(pulse_wide) / 1000000 * _FREQUENCY * 4096)
    print ('angle {} pulse_wide {} analog_value {}'.format(angle, pulse_wide, analog_value))

    return analog_value
# bot test END

if __name__ == "__main__":
    print('Running benchmarks with COUNT = {}'.format(COUNT))

    test(lambda d: [tanh(x) for x in d], '[tanh(x) for x in d] (Python implementation)')

    from superfastcode import fast_tanh
    test(lambda d: [fast_tanh(x) for x in d], '[fast_tanh(x) for x in d] (CPython C++ extension)')

    from superfastcode2 import fast_tanh2
    test(lambda d: [fast_tanh2(x) for x in d], '[fast_tanh2(x) for x in d] (PyBind11 C++ extension)')


    # bot test
    for i in range(10, 170, 10):
        _angle_to_analog(i)
