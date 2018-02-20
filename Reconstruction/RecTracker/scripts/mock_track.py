
from __future__ import print_function

r"""
MORITAT :: Mock Reconstruction and Pedagogical Tracking

 ATLAS parametrization of the track [1]:
 $d_0$ [cm] ... transverse impact parameter
                 distance of closest approach to beam line

 $z_0$ [cm] ... longitudinal impact parameter,
                 z-value at the point of closest approach to the beam line
 $\phi_0$   ... polar direction of the track at the point of closest approach
 $\cot \theta$ ... inverse slope of the track in the ($r$ - $z$) plane
 $q / p_T$ ... charge over transverse momentum $\propto $\rho$^-1$

 the curvature $\rho$ is connected to experimental quantities via

\begin{equation}
 \rho [ \textnormal{cm} ] = \frac {p_T [ \textnormal {GeV}] } { 0.003 \cdot q[ \textnormal [e] \cdot B [ \textnormal T ] };
\end{equation}

[1] Andreas Salzburger and Dietmar Kuhn (dir.). A Parametrization for Fast
Simulation of Muon Tracks in the ATLAS Inner Detector and Muon
System. Diploma thesis, Innsbruck University, 2003
"""

import numpy as np
import scipy.integrate
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import scipy.optimize
import scipy.linalg
import itertools
from matplotlib.patches import Ellipse

def construct_filename(conf, prefix="track"):
    filename = [prefix + '_x_']
    filename += ['%.4g_' % (d) for d in conf['x0']]
    filename += ['p_']
    filename += ['%.4g_' % (d) for d in conf['p0']]
    filename += ['EnLoss_%.4g_' % conf['energy_loss']]
    filename += ['ScatAng_%.4g_' % conf['scattering_angle']]
    filename += ['.dat']
    return ''.join(filename)

def parse_parameters(filename):
    conf = {}
    x0 = filename.split('x0_')[1].split('_p0')[0]
    conf[ 'x0' ] = [float(f.split('_'))]
    p0 = filename.split( 'p0_' )[1].split( '_p0' )[0]
    conf[ 'p0' ] = [float(f.split('_'))]
    energy_loss = filename.split('energy_loss_')[0].split( '_p0' )[0]
    conf[ 'energy_loss' ] = float(energy_loss) 
    return conf

# http://codereview.stackexchange.com/questions/43928
def perpendicular_vector(v):
    if v[1] == 0 and v[2] == 0:
        if v[0] == 0:
            raise ValueError('zero vector')
        else:
            return np.cross(v, np.random.rand(3))
    return np.cross(v, np.random.rand(3))


# http://stackoverflow.com/questions/6802577
def rotate(axis, theta):
    return scipy.linalg.expm(np.cross(np.eye(3), axis/scipy.linalg.norm(axis)*theta))


class Hit:
  def __init__(self, pos=None, dz=12, dphi=2*np.pi, dr=0.1):
    if pos is None:
      self.pos = [np.random.rand(1) * 2* dr - dr, # r
      np.random.rand(1) * 2 * dphi, # phi
      np.random.rand(1) * 2 * dz - dz ] # z (rel to layer)
    else:
      self.pos = pos
    
class Layer:
  def __init__(self, r=1, dz=12, hits=None):
    self.r = r
    self.dz = dz
    if hits is None:
      self.hits = [Hit(dz = dz) for i in range(4)]
    else:
      self.hits = []
      self.add_hits(hits)
  def add_hits(self, hits):
      for row in hits:
        hit_r = np.sqrt(row[0] ** 2 + row[1]**2)
        if np.round(hit_r) == self.r:
          self.hits.append(Hit([hit_r, np.arctan2(row[1] , row[0]), row[2]]))
    
class HitDoublet:
  def __init__(self, hit1=Hit(), hit2=Hit()):
    self.hit1 = hit1
    self.hit2 = hit2

  def draw_RZ(self, ax, r):
      xy = ( (0.5 * self.hit1.pos[2] + 0.5* self.hit2.pos[2]), r + 0.5)
      dx = self.hit2.pos[2] - self.hit1.pos[2]
      length = np.sqrt(1 + dx**2)+0.2
      e = Ellipse(xy=xy, width=0.1 + 1/6 * 2. / length, height=np.sqrt(1 + dx**2)+0.2, fill=True, alpha=0.5, angle=180/np.pi*np.arctan((dx) ))
      ax.add_artist(e)
  def draw_RPhi(self, ax, r):
      x1 = self.hit1.pos[0] * np.cos(self.hit1.pos[1])
      x2 = self.hit2.pos[0] * np.cos(self.hit2.pos[1])
      y1 = self.hit1.pos[0] * np.sin(self.hit1.pos[1])
      y2 = self.hit2.pos[0] * np.sin(self.hit2.pos[1])
      xy = ( (0.5 * x1 + 0.5* x2), (0.5 * y1 + 0.5 * y2))
      dx = x2 - x1
      dy = y2 - y1

      length = np.sqrt(dy**2 + dx**2)+0.2
      if length < 2: 
        e = Ellipse(xy=xy, width= 0.01 + 1/6. * 1. / length, height=length, fill=True, alpha=0.5, angle=90 + 180/np.pi*np.arctan2(dy,dx ))
        ax.add_artist(e)

    

class Layers:
  def __init__(self, hits=None):
    self.layers = [Layer(i, hits=hits) for i in range(2,6)]
  def add_hits(self, hits):
    for l in self.layers:
      l.add_hits(hits)
      
  def draw_RPhi(self, ax=None):
    if ax is None:
      fig = plt.figure(figsize=(7,7))
      ax = plt.axes()
    else:
      fig = plt.gcf()
    for l in self.layers:
      e = Ellipse(xy=(0,0), width=2*l.r, height=2*l.r, fill=False, lw=3, color="steelblue")
      ax.add_artist(e)
      #ax.plot([-l.dz, l.dz], [l.r,l.r], '-', lw=3, color="steelblue")
      for h in l.hits: 
        print(h.pos[0], h.pos[1])
        ax.plot([h.pos[0]* np.cos(h.pos[1])], [h.pos[0]* np.sin(h.pos[1])], 'o')
    for earlier, later in zip(self.layers[:-1], self.layers[1:]):
      for h1 in earlier.hits:
        for h2 in later.hits:
          hd = HitDoublet(h2, h1)
          hd.draw_RPhi(ax, earlier.r)
    ax.margins(0.05)
    plt.xlim(-6,6)
    plt.ylim(-6,6)
    ax.plot()
    return fig, ax

  def draw_RZ(self, ax=None):
    if ax is None:
      fig = plt.figure()
      ax = plt.axes()
    else:
      fig = plt.gcf()

    for l in self.layers:
      ax.plot([-l.dz, l.dz], [l.r,l.r], '-', lw=3, color="steelblue")
      for h in l.hits: 
        ax.plot([h.pos[2]], [l.r], 'o')
    for earlier, later in zip(self.layers[:-1], self.layers[1:]):
      for h1 in earlier.hits:
        for h2 in later.hits:
          hd = HitDoublet(h2, h1)
          hd.draw_RZ(ax, earlier.r)
    ax.margins(0.05)
    ax.plot()
    return fig, ax


  def draw_3D(self):
      fig = plt.figure()
      ax = fig.add_subplot(111, projection='3d')
      for l in self.layers:
          r = l.r
          x = np.linspace(-1, 1, 100)
          z = np.linspace(-2, 2, 100)
          Xc, Zc = np.meshgrid(x, z)
          Yc = np.sqrt(1 - Xc**2)
          Yc = r * Yc
          Xc = r * Xc
          #Yc = np.append(Yc, -1 * Yc)
          plot_args = {'rstride': 20,
                      'cstride' : 10,
                      'alpha' : 0.2,
                      'lw': 0}
          for _yc in [Yc, -1 * Yc]:
              ax.plot_surface(Xc, _yc, Zc, **plot_args) 
      return fig, ax


def detector_track_intersection(full_track, radii=list(range(2,6))):
    # convention: expect x, y coordinates in first two columns of track
    # np.searchsorted expects data to be ordered
    # so split it into segments that are assumed to be straight
    result = []
    num_segments = 20
    l = full_track.shape[0]
    segment_length = l / num_segments
    for i in range(num_segments):
        track_segment = full_track[ i * segment_length : (i+1) * segment_length, : ] 
        track_radius = np.linalg.norm(track_segment[:, :2], axis=1)
        hit_indices = np.searchsorted(track_radius, radii)
        hit_indices = hit_indices[ hit_indices < track_segment.shape[0]]
        hit_indices = hit_indices[ 0 < hit_indices ]
        result.append(track_segment[hit_indices, :3])
        if hit_indices.shape[0] == 0:
            track_radius = np.linalg.norm(track_segment[::-1, :2], axis=1)
            hit_indices = np.searchsorted(track_radius, radii)
            hit_indices = hit_indices[ hit_indices < track_segment.shape[0]]
            hit_indices = hit_indices[ 0 < hit_indices ]
            result.append(track_segment[::-1, :3][hit_indices, :3])
        
    return np.concatenate(result) 

    
def detector_response(track, radii=list(range(2,6)), sigma=.1):
    hits = detector_track_intersection(track, radii=radii)
    noise = np.random.normal(loc=0, scale=sigma, size=hits.shape)
    # constrict to detector surfaces, let second column of noise be the angle 
    # (from the track to the registered hit)
    noise_angle =  noise[:, 1]
    hits[:, 0] = hits[:, 0] * np.cos(noise_angle) - hits[:, 1] * np.sin(noise_angle)
    hits[:, 1] = hits[:, 1] * np.cos(noise_angle) + hits[:, 0] * np.sin(noise_angle)
    hits[:, 2] = hits[:, 2] + noise[:, 2]
    return hits


def position_derivative(t, x, B=0.3):
    """
    For odeint solver. x[:3] is the position and x[3:6] the velocity,
    using cartesian coordinates.
    """
    # change of position due to velocity,
    # change of velocity due to magnetic field
    dx_dp = np.array([x[3], x[4], x[5], 
                      -1 * B * x[4], B * x[3], 0]) 
    return dx_dp


def propagate(**kwargs):
    # set default arguments
    x0 = kwargs.pop('x0', [0, 0, 0])
    p0 = kwargs.pop('p0', [1, 0, 1])
    Dfun = kwargs.pop('Dfun', None)
    time_points = kwargs.pop('time_points', (0.001, 15, 10000))
    radii = kwargs.pop('radii', list(range(2,6)))
    thickness = kwargs.pop('thickness', 0.001) 
    energy_loss = kwargs.pop('energy_loss', 0.00) 
    scattering_angle = kwargs.pop('scattering_angle', 0.00) 
    # set up ode integrator
    ig = scipy.integrate.ode(position_derivative, Dfun)
    ig.set_integrator('zvode',
                       method='bdf')
    ig.set_initial_value(x0 + p0, t=0.)
    time_of_last_collision = 0 
    result = []
    rmax = 7
    rmax_reached = False
    for t in np.linspace(*time_points):
        delta_t = t - ig.t
        time_of_last_collision += delta_t
        particle_state = ig.integrate(t)
        particle_state = particle_state.real
        result.append(particle_state)
        particle_radius = np.sqrt(result[-1][0]**2 + result[-1][1]**2).real
        if particle_radius > rmax:
          rmax_reached = True
        for radius in radii:
            if (radius < particle_radius < radius + thickness and
                time_of_last_collision > 0.01) and rmax_reached == False:
                print('detector collision...', t, particle_radius)
                # energy loss proportional to momentum 
                _dp = particle_state[3:6]
                _dp = _dp - energy_loss * _dp
                # multiple scattering leads to some arbitrary small rotation
                r = rotate( perpendicular_vector(_dp), scattering_angle )
                _dp = np.dot(r, _dp)
                ig.set_initial_value( list(particle_state[:3]) + list(_dp), t=t)
                time_of_last_collision = 0 
                break
    return np.array(result)


def helix(params, time_points=(0,4,1000)):
    path_points = np.linspace(*time_points)
    d0, z0, phi0, cotTheta, q_pT = params
    # motion in z linear, unchanged by magnetic field in z
    z = z0 + cotTheta * path_points
    # cartesian coordinates of point of closest approach
    x0 =  d0 * np.cos(phi0)
    y0 = d0 * np.sin(phi0)
    rho = 1. / q_pT
    # cartesian coordinates of helix center
    xc = x0 - rho * np.cos(phi0) 
    yc = y0 - rho * np.sin(phi0)
    # generate real space points from track parameters
    x = xc + rho * np.cos(path_points + phi0)
    y = yc + rho * np.sin(path_points + phi0)
    return np.array([x, y, z]).T


def helix_at_detector(params, radii=list(range(2,6))):
    track = helix(params)
    hits = detector_track_intersection(track, radii=radii)
    return hits
    

def helix_residuals(params, measurements):
    m_shape = measurements.shape
    h = helix_at_detector(params)
    g = np.zeros(measurements.shape)
    minh = min(h.shape[0], g.shape[0])
    g[:minh] = h[:minh]
    return (g - measurements).flatten()

def fit_parameters_curvefit(hits, x0):
    params = scipy.optimize.leastsq(helix_residuals, x0, args=(hits))
    return params

def fit_parameters(hits, x0):
    params = scipy.optimize.leastsq(helix_residuals, x0, args=(hits))
    return params


def hough_transform(xy):
    thetadim = 100
    theta = np.linspace(0, 2 * np.pi, thetadim)
    Rdim = 100
    Rbins = np.linspace(0, 10, Rdim) 
    Rtheta = np.zeros((thetadim, Rdim))
    for point in xy:
        R  = ( ( point[0]**2 + point[1]**2 ) / 
               ( 2 * ( point[0] * np.cos(theta) + point[1] * np.sin(theta) ) ))
        Rdigitized = np.digitize(R, Rbins)
        for i, r in enumerate(Rdigitized):
            Rtheta[i, r-1] += 1
    return Rtheta


