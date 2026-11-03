import sys,unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'src'))
import prototype as p
import numpy as np
class PrototypeTests(unittest.TestCase):
 def test_reference_recovery(self):
  _,_,metrics=p.demo()
  self.assertTrue(metrics)
 def test_invalid_data(self):
  with self.assertRaises(ValueError):p.finite([float('nan')])
  with self.assertRaises(ValueError):p.finite([])
 def test_reject_invalid_process_input(self):
  with self.assertRaises(ValueError):p.process([1,2],window=4)

 def test_extreme_finite_constants_stay_finite(self):
  for window in (1,3,11,17,101):
   for value in (np.finfo(float).max,-np.finfo(float).max,0.,1e-308):
    with self.subTest(window=window,value=value):
     samples=np.full(window+4,value)
     result=p.process(samples,window)
     self.assertEqual(result.shape,samples.shape)
     self.assertTrue(np.isfinite(result).all())
     np.testing.assert_allclose(result,samples,rtol=2e-14,atol=0)
 def test_scale_equivariance(self):
  samples=np.random.default_rng(123).uniform(-.8,.8,80)
  reference=p.process(samples,11)
  for scale in (1e-300,1e300):
   np.testing.assert_allclose(p.process(samples*scale,11)/scale,reference,rtol=1e-12,atol=1e-15)
 def test_window_requires_integer(self):
  for value in (True,np.bool_(False),3.,'3',None,float('inf')):
   with self.subTest(value=value),self.assertRaises(ValueError):
    p.process([1,2,3,4,5],value)
  np.testing.assert_allclose(p.process([1,2,3],np.int64(1)),[1,2,3])
