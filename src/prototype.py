"""检波信号滤波 — deterministic numerical prototype."""
import argparse,json
from pathlib import Path
import numpy as np

def finite(a,ndim=None):
 a=np.asarray(a,dtype=float)
 if not np.isfinite(a).all() or not a.size or (ndim is not None and a.ndim!=ndim): raise ValueError('Invalid shape, empty or nonfinite input')
 return a
def process(samples,window=9):
 samples=finite(samples,1)
 if window<1 or window%2!=1 or window>len(samples): raise ValueError('window must be odd and within signal length')
 padded=np.pad(samples,window//2,mode='reflect')
 median=np.median(np.lib.stride_tricks.sliding_window_view(padded,window),axis=1)
 return np.convolve(np.pad(median,window//2,mode='edge'),np.ones(window)/window,mode='valid')

def demo():
 rng=np.random.default_rng(42);x=np.linspace(0,6,600);truth=1+.3*np.sin(x*2);raw=truth+rng.normal(0,.1,len(x));raw[::67]+=1.5
 result=process(raw)
 assert np.mean((result-truth)**2)<np.mean((raw-truth)**2)*.25
 return x,{'Noisy detector':raw,'Filtered':result,'Synthetic truth':truth},{'raw_rmse':float(np.sqrt(np.mean((raw-truth)**2))),'filtered_rmse':float(np.sqrt(np.mean((result-truth)**2)))}

def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--output',type=Path,default=Path('artifacts'));args=p.parse_args();args.output.mkdir(parents=True,exist_ok=True)
 x,series,metrics=demo()
 import matplotlib
 matplotlib.use('Agg')
 import matplotlib.pyplot as plt
 if x is None:
  fig,axes=plt.subplots(1,len(series),figsize=(14,4),layout='constrained');axes=np.atleast_1d(axes)
  for ax,(label,values) in zip(axes,series.items()):ax.imshow(values,cmap='viridis');ax.set_title(label);ax.set_axis_off()
 else:
  fig,ax=plt.subplots(figsize=(11,5),layout='constrained')
  for label,values in series.items():ax.plot(x,values,label=label)
  ax.legend();ax.grid(alpha=.2)
 fig.suptitle('Synthetic prototype validation — not field measurements')
 fig.savefig(args.output/'result.png',dpi=140);plt.close(fig)
 np.savez_compressed(args.output/'results.npz',**{str(i):v for i,v in enumerate(series.values())})
 (args.output/'metrics.json').write_text(json.dumps(metrics,indent=2)+'\n');print(json.dumps(metrics,indent=2))
if __name__=='__main__':main()
