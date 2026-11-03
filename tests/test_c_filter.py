"""Compile the C implementation and compare it against the Python reference."""
import ctypes
import os
from pathlib import Path
import shlex
import subprocess
import tempfile
import unittest

import numpy as np
from test_prototype import p

ROOT = Path(__file__).resolve().parents[1]


class CFilterTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.addClassCleanup(cls.tmp.cleanup)
        library = Path(cls.tmp.name) / 'filter.so'
        subprocess.run(shlex.split(os.environ.get('CC', 'cc')) + [
            '-std=c11', '-O2', '-Wall', '-Wextra', '-Werror', '-shared', '-fPIC',
            str(ROOT/'c/detector_filter.c'), '-lm', '-o', str(library)], check=True)
        cls.library = ctypes.CDLL(str(library))
        cls.function = cls.library.detector_filter
        cls.function.argtypes = [ctypes.POINTER(ctypes.c_double), ctypes.c_size_t,
                                 ctypes.c_size_t, ctypes.POINTER(ctypes.c_double)]
        cls.function.restype = ctypes.c_int

    def compare(self, samples, window):
        samples = np.ascontiguousarray(samples, dtype=np.float64)
        output = np.empty_like(samples)
        status = self.function(samples.ctypes.data_as(ctypes.POINTER(ctypes.c_double)),
                               samples.size, window,
                               output.ctypes.data_as(ctypes.POINTER(ctypes.c_double)))
        self.assertEqual(status, 0)
        expected = p.process(samples, window)
        self.assertTrue(np.isfinite(output).all())
        # Compare normalized values to avoid overflow inside the assertion itself.
        scale = np.max(np.abs(expected)) or 1.0
        np.testing.assert_allclose(output/scale, expected/scale, rtol=1e-12, atol=1e-14)

    def test_random_signals_and_all_boundary_windows(self):
        rng = np.random.default_rng(2026)
        for length in (1, 2, 3, 10, 31, 101):
            values = rng.normal(size=length)
            for window in range(1, length+1, 2):
                with self.subTest(length=length, window=window):
                    self.compare(values, window)

    def test_extreme_scales_and_impulses(self):
        for value in (0., np.finfo(float).max, -np.finfo(float).max, 1e-308):
            self.compare(np.full(105, value), 101)
        values = np.linspace(-.8, .8, 31)
        values[15] = .99
        for scale in (1e-300, 1., 1e300):
            self.compare(values*scale, 11)

    def test_same_input_as_python_demo(self):
        _, series, _ = p.demo()
        self.compare(series['Noisy detector'], 9)
