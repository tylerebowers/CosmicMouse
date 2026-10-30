## 1. Forward model

Magnetic Dipole Field:

$$
\vec{B}(\vec{r})=\frac{\mu_{0}}{4\pi}\,\frac{3 \hat{r}(\vec{m}\cdot\hat{r})-\vec{m}}{r^{3}}
$$

- $\mu_0 = 4\pi\cdot10^{-7}\ \text{H/m}$ = permeability of free space
- $\vec{r}$ = position vector from the dipole center to the observer
- $r = |\vec{r}|$ = distance from the dipole center to the observer
- $\hat{r} = \vec{r}/r$ = unit vector from the dipole center to the observer
- $\vec{m}$ = magnetic moment of the dipole

Magnetic Moment (3/16 in dia × 3/16 in height cylinder, N52):

$$
|\vec{m}| = \frac{B_r V}{\mu_0}
= \frac{1.42\ \text{T}\cdot 8.154\times10^{-8}\ \text{m}^3}{4\pi\cdot10^{-7}\ \text{H/m}}
\approx 0.0921\ \text{A}\cdot\text{m}^2
$$

- $B_r = 1.42\ \text{T}$ = residual induction (remanence) of N52
- $V = \pi\left(\tfrac{4.7\ \text{mm}}{2}\right)^2 \cdot 4.7\ \text{mm} = 81.54\ \text{mm}^3 = 8.154\times10^{-8}\ \text{m}^3$

(4.7mm used instead of 4.7625mm = 3/16in to account for the coating)

We can pull out a fixed term $k$, using: $\vec m = |\vec m|\,\hat m$ and $|\vec m| = B_rV/\mu_0$:

$$
\vec B = k\,\frac{3\hat r(\hat m\cdot\hat r) - \hat m}{r^3},\quad
k = \frac{\mu_0|\vec m|}{4\pi} = \frac{B_rV}{4\pi}
$$

<!-- (1.42⋅81.54)/(4⋅pi) -->
$k = 1.42T \cdot 81.54mm^3 / 4π = 9.21 T \cdot mm^3 = 9214 mT \cdot mm^3$. 

Vector $\vec r$ has 3 components, and one sensor gives 3 measurements, so if $\hat m$ (unit vector of moment) is known, the system can be solved.


## 2. Inverting one dipole

A dipole has axial symmetry (in this case, about the vector normal to cylindrical face). Because of this, I will use spherical coordinates, $r$ = distance from the origin (sensor), $a$ angle rotating from the $z$ axis $c = cos(a) = \hat r \cdot \hat m$ (normal to the top face of the sensor), and $\phi$ azimuth rotation about the $z$ axis.

Knowns:
Sensor position: $\vec{S} = (0, 14.5, 0) mm$  
Magnet strength: $k = 9214 mT \cdot mm^3$  
Measured field: $\vec{B} = (0, 5.091, 20.885) mT$ 

$$
\begin{align*}
& \vec B = k\,\frac{3\hat r(\hat m\cdot\hat r) - \hat m}{r^3}, \quad \hat m = \hat z\; \text{(magnet axis is along z)} \\
&\vec B \cdot \hat m = \frac{k}{r^3} (3\hat r(\hat m\cdot\hat r) - \hat m) \cdot \hat m = \frac{k}{r^3} (3\hat (\hat m\cdot\hat r)(\hat m\cdot\hat r) - \hat m\cdot \hat m) = \frac{k}{r^3} (3c^2 - 1) \\

&\vec B = \frac{k}{r^3} (3\hat rc - \hat m) \rightarrow |\vec B|^2 = \vec B \cdot \vec B = (\frac{k}{r^3})^2 (9c^2 - 6c^2 +1) = \frac{k}{r^3} \sqrt{ (3c^2 + 1)}\\
&\vec B \cdot \hat m =\frac{k}{r^3} (3c^2 - 1); \text{Amount of field along the magnet axis} \\
&| \vec B | = \frac{k}{r^3} \sqrt{ (3c^2 + 1)}; \text{Magnitude of field} 
\end{align*}
$$

**Now to solve for** $c$. Let $u$ be the cosine of the angle between the field and the magnet’s axis.

$$
\begin{align*}
&u = \frac{\vec B\cdot\hat m}{|\vec B|} = \frac{3c^2-1}{\sqrt{3c^2+1}}\;\; \rightarrow \;\; \text{(Let }s = 3c^2 \text{)} \;\; u^2 = \frac{(s-1)^2}{s+1} \\
&u^2(s+1) = s^2 - 2s + 1 \;\; \rightarrow \;\; s^2 - (2+u^2)s + (1-u^2) = 0
\end{align*}
$$

Quadratic formula:

$$
s = \frac{(2+u^2) + \sqrt{(2+u^2)^2 - 4(1-u^2) = u^4+8u^2}}{2} = \frac{(2+u^2) + u\sqrt{u^2+8}}{2}
$$
We know that the magnet is above the sensor, so $c > 0$. (Durning initialization we can check the sign of the z reading to see if the magnet is flipped up or down; if flipped all readings will be inverted before going into calculations.)

$$
c = +\sqrt{\frac{1}{3} \cdot \left[ \frac{(2+u^2) + u\sqrt{u^2+8}}{2} \right]}
$$

**Now to solve for** $r$.

$$
r = \left(\frac{k\sqrt{s+1}}{|\vec B|}\right)^{1/3}= \left(\frac{k\sqrt{3c^2+1}}{|\vec B|}\right)^{1/3}
$$

Position:

$$
\vec s = r\,\left[ sin(a) \frac{(B_x, B_y, 0)}{\sqrt{B_x^2 + B_y^2}+cos(a) \hat z} \right]
$$



**Example:**

Measurements from U3: X -2.26mT, Y -5.12mT, Z 15.40mT 

Angle $a$: 

$$
\begin{align*}
&| \vec B |  = \sqrt{B_x^2 + B_y^2 + B_z^2} = \sqrt{(-2.26)^2 + (-5.12)^2 + (15.40)^2} = 16.385 mT\\
&u = \frac{\vec B \cdot \hat m = 15.40}{16.385} = 0.93986\\
&s = \frac{(2.88334) + 0.93986\sqrt{8.88334}}{2} = 2.84229\\
&c = -\sqrt{2.84229/3} = -0.97336 \rightarrow \cos^{-1}(0.97336) = 13.25\degree \quad (\vec m = -\vec z) \\
\end{align*}
$$

<!-- (15.40/(((−2.26)^2 + (−5.12)^2 + (15.40)^2)^0.5))^2 -->
<!-- −sqrt(({(2.88334) + 0.93986 sqrt{8.88334}}/{2})/3) -->

c is negative because the moment points down while $\hat r$ points up. That puts the magnet $13.25\degree$ off straight up.

Distance $r$:

$$
r = \left(\frac{9214\sqrt{3.84229}}{16.385}\right)^{1/3} = 10.330 mm
$$

Direction $\hat r$:

$$
\hat r = \frac{\dfrac{r^3}{k}\vec B + \hat m}{3c} = \frac{\frac{10.33^3}{9214}(2.26, 5.12, -15.40) + (0,\ 0,\ -1) = (0.27036, 0.61250, -2.84229)}{3\cdot−0.97336=−2.92008} = (-0.0926, -0.2098, 0.9734)
$$

Magnet position:

*y is flipped due to U3 sensor's location*

$$
\vec M = \vec S + r\,\hat r = (0,\ 14.5,\ 0) + 10.330\times(-0.0926,\ 0.2098,\ 0.9734) = \mathbf{(-0.96,\ 16.67,\ 10.06)\ mm}
$$

<details>

<summary>Summary of Equations</summary>

| # | Step | Equation | Purpose |
|---|---|---|---|
| 0 | Constants | $k = \dfrac{B_rV}{4\pi}$ | Magnet strength |
| 1 | Sensor axes → board frame | $\vec B_i = \mathbf R_z(\varphi_i)\,(-X_i,\ +Y_i,\ -Z_i)$ | TMAG5273: +X toward pins 1–3, +Y toward pin 1, +Z into the board |
| 2 | Polarity | $\sigma = \operatorname{sign}(B_{z,rest})$, then $\vec B \leftarrow \sigma\vec B$ | Makes the moment point up whichever way the magnet is inserted |
| 3 | Remove cross-talk *(optional, iterative)* | $\vec B^{own}_i = \vec B_i - \sum_{j\ne i} k\,\dfrac{3\hat r_{ij}(\hat m\cdot\hat r_{ij}) - \hat m}{r_{ij}^3}$ | Subtracts the neighbouring magnets' fields |
| 4 | Magnet axis | $\hat m = \hat z$ at rest; $\hat m = \mathbf R\hat z$ when tilted | Direction of the moment |
| 5 | Field magnitude | $\lvert\vec B\rvert = \sqrt{B_x^2 + B_y^2 + B_z^2}$ | Total strength |
| 6 | Field tilt | $u = \dfrac{\vec B\cdot\hat m}{\lvert\vec B\rvert}$ | Cosine of the field's angle from the magnet axis; cancels $k/r^3$ |
| 7 | Solve for the angle | $s = 3c^2 = \dfrac{(2+u^2) + u\sqrt{u^2+8}}{2}$ | Inverts $u = \dfrac{3c^2-1}{\sqrt{3c^2+1}}$ |
| 8 | Viewing angle | $c = \cos a = +\sqrt{s/3}$ | + because the magnet is in front of the sensor along $\hat m$ |
| 9 | Distance | $r = \left(\dfrac{k\sqrt{s+1}}{\lvert\vec B\rvert}\right)^{1/3}$ | Undoes the $1/r^3$ fall-off |
| 10 | Direction (general) | $\hat r = \dfrac{\dfrac{r^3}{k}\vec B + \hat m}{3c}$ | Sensor → magnet unit vector |
| 10′ | Direction (shortcut, $\hat m = \hat z$ only) | $\hat r = \sin a\,\dfrac{(B_x,\ B_y,\ 0)}{\sqrt{B_x^2+B_y^2}} + \cos a\,\hat z$ | Same result while the knob isn't tilted |
| 11 | Magnet position | $\vec M_i = \vec S_i + r\,\hat r$ | Sensor position plus the vector to the magnet |

</details>


The next step is to include the other 2 magnets to remove the cross-talk.

## 3. Removing cross-talk

Each sensor measures the sum of all three magnet fields, not just its own magnet. While the their influence is small (field strength falls as $1/r^3$) the effect changes as the knob is moved so a simple offset does not work. Fields add as vectors, so the total at sensor $i$ is:

$$
\vec B_i = \sum_{j=1}^{3} k\,\frac{3\hat r_{ij}(\hat m\cdot\hat r_{ij}) - \hat m}{r_{ij}^3},
\qquad \vec r_{ij} = \vec S_i - \vec M_j,\quad r_{ij} = |\vec r_{ij}|,\quad \hat r_{ij} = \vec r_{ij}/r_{ij}
$$

- $\vec S_i$ = position of sensor $i$
- $\vec M_j$ = position of magnet $j$
- $\vec r_{ij}$ = vector from magnet $j$ to sensor $i$ (the dipole formula needs dipole → observer)

The single-dipole inversion (sec 2) only works on one magnet's field. So before inverting, the neighboring fields are subtracted:

$$
\vec B^{own}_i = \vec B_i - \sum_{j\neq i} k\,\frac{3\hat r_{ij}(\hat m\cdot\hat r_{ij}) - \hat m}{r_{ij}^3}
$$

The neighbours' positions $\vec M_j$ are needed for this, but they are what we're solving for. Because cross-talk is small (<5%), we can iterate:

1. Pass 1: invert each sensor with no correction for first estimate of $\vec M_1, \vec M_2, \vec M_3$
2. Pass 2+: use those estimates to compute and subtract the cross-talk, then invert again
3. Repeat until the positions stop changing (several passes)

Each pass shrinks the error by some magnitude. Most error in the estimated neighbor positions enters through the small cross-talk term (EMI from component placement can also cause issues).

**Geometry**

Sensors are placed at radius 14.5 mm, however the hall effect element placements are not in the center of the package. The true position is slightly offset according to the [docs (pg 13)](https://www.ti.com/lit/ds/symlink/tmag5273.pdf):

- 1.85 mm from the pin-1 end, 2.9mm package height = 1.45 mm to center, so 0.40 mm past the package center, toward pin 3 (radially inward)
- 0.68 mm from the pins 1–3 edge, 1.6mm package width = 0.8mm to center, so 0.12 mm from the package center, toward pins 1–3.
- 0.73 mm below the top of the package (side view). This plane is $z = 0$.

$$
\begin{align*}
&\text{U3 } \vec S_1 = (0, 14.5, 0) + (-0.12, -0.4, 0) = (-0.12, 14.1, 0) \\
&\text{U4 } \vec S_2 = (12.557, -7.25, 0) + (-0.2864, 0.30392, 0) = (12.271, -6.946, 0) \\
&\text{U5 } \vec S_3 = (-12.557, -7.25, 0)+ (0.4064, 0.0961, 0) = (-12.151, -7.154, 0) \\
\end{align*}
$$

Magnets at rest, at radius 16 mm and height $h$:

$$
\begin{align*}
&\vec M_1 = (0,\ 16,\ h)\\
&\vec M_2 = (13.852,\ -8.008,\ h)\\
&\vec M_3 = (-13.852,\ -8.008,\ h)\\
\end{align*}
$$

This is a lot to do by hand, so instead it can be done in Python:


<details>

<summary>Code</summary>

```python

import numpy as np

K = 1.42 * np.pi * 2.35**2 * 4.7 / (4 * np.pi) * 1000# B_r V / 4pi  (mT*mm^3)
S = np.array([[-0.12, 14.1, 0], [12.271, -6.946, 0], [-12.151, -7.154, 0]]) # Hall elements (mm)
M_REST = np.array([[0, 16, 0], [13.852, -8.008, 0], [-13.852, -8.008, 0]]) # magnets at rest
PHI = np.radians([0, -120, 120]) # footprint rotation
m = np.array([0, 0, 1.0]) # magnet axis

def to_board(raw, i): # TMAG5273 axes -> board frame
    c, s = np.cos(PHI[i]), np.sin(PHI[i])
    x, y, z = -raw[0], raw[1], -raw[2]
    return np.array([c * x - s * y, s * x + c * y, z])

def dipole(r): # field at r from a magnet
    d = np.linalg.norm(r); u = r / d
    return K * (3 * u * (m @ u) - m) / d**3

def invert(B, Si):# section 2
    Bn = np.linalg.norm(B); u = B @ m / Bn
    s = ((2 + u*u) + u * np.sqrt(u*u + 8)) / 2
    c = np.sqrt(s / 3); r = (K * np.sqrt(s + 1) / Bn) ** (1/3)
    return Si + r * (B * r**3 / K + m) / (3 * c)

B = [to_board(b, i) for i, b in enumerate([[2.26,5.12,-15.40]])]
B = [np.sign(B[0][2]) * b for b in B] # polarity
n = len(B)

M = M_REST.astype(float)
for p in range(1, 6):
    old = M.copy()
    for i in range(n):
        xt = sum(dipole(S[i] - old[j]) for j in range(3) if j != i) if p > 1 else 0
        M[i] = invert(B[i] - xt, S[i])
    M[n:, 2] = M[:n, 2].mean() # unmeasured magnets: rest, same height
    print(f"pass {p}: " + "  ".join(f"M{i+1}={np.round(M[i], 3)}" for i in range(n)))
    if p > 1 and np.abs(M - old).max() < 0.005: break
```

</details>

```bash
pass 1: M1=[-1.076 16.267 10.055]
pass 2: M1=[-1.023 16.434  9.86 ]
pass 3: M1=[-1.022 16.43   9.859]
```

We can see that the final magnet position, after cross-talk is removed, changes the position by ~2%. However, we have a problem because when we solve each position independently, the magnets relative positioning no longer matches up. 

## 4. Levenberg–Marquardt least squares fit

Solving each magnet on its own lets the three positions drift apart. The magnets are fixed into a rigid knob, so their spacing never changes as are the sensors to the PCB. While component positions will vary due to manufacturing differences (I am hand soldering) we can at least parallelize the calculation then use a least squares solver to find the best fit.  

Unknowns: (the knob pose)

$$
\mathbf p = (x, y, z, \phi, \theta, \psi)
$$

- $x, y, z$ = position of the knob center.
- $\phi, \theta, \psi$ = roll, pitch, yaw

Magnet positions:

$$
\begin{align*}
&\vec q_1 = (0, 16, 0)  &\text{Above U3}\\
&\vec q_2 = (13.852, -8.008, 0)  &\text{Above U4}\\
&\vec q_3 = (-13.852, -8.008, 0)  &\text{Above U5}
\end{align*}
$$

For a given pose, the magnets and their axis in the board frame are:

$$
\vec M_j(\mathbf p) = \mathbf R(\phi, \theta, \psi)\vec q_j + \vec T \hat m_j
$$

Residuals:

$$
\vec B^{model}_i(\mathbf p) = \sum_{j=1}^{3} k\,\frac{3\hat r_{ij}(\hat m\cdot\hat r_{ij}) - \hat m}{r_{ij}^3},
\qquad \vec r_{ij} = \vec S_i - \vec M_j(\mathbf p)
$$

Stack the differences into one vector of 9 residuals:

$$
\mathbf r(\mathbf p) = 
\begin{bmatrix} 
\vec B^{model}_1 - \vec B_1 \\ 
\vec B^{model}_2 - \vec B_2 \\ 
\vec B^{model}_3 - \vec B_3 
\end{bmatrix}
\quad (9 \times 1)
$$

The best pose minimizes the sum of squared residuals:

$$
\mathbf p^* = \arg\min_{\mathbf p}\ F(\mathbf p),
\quad F(\mathbf p) = \tfrac12\,\mathbf r(\mathbf p)^\mathsf{T}\mathbf r(\mathbf p) = \frac{1}{2} \sum_{i=1}^{3} \left\| \vec{B}_{i, \text{pred}}(\mathbf{p}) - \vec{B}_{i, \text{measured}} \right\|^2
$$

There are 9 equations and 6 unknowns, so the system is over-determined. (2 magnets and 2 sensors could be used) The 3 extra equations average out noise and the leftover residual shows how well the model fits.


$\mathbf r(\mathbf p)$ is non-linear (because of $1/r^3$, the unit vectors and the rotations), so it is solved iteratively. Near the current guess, the residual is approximately linear.
$\mathbf J$ (the Jacobian) says how each of the 9 readings change when each of the 6 pose values change. It is computed numerically: nudge each pose value by a tiny amount $\varepsilon$ and see how the
residuals change. The update step is:

$$
\Delta\mathbf p = \mathbf J^\mathsf{T}\vec r \big(\mathbf J^\mathsf{T}\mathbf J + \lambda\,\mathrm{diag}(\mathbf J^\mathsf{T}\mathbf J)\big)^{-1} 
$$

- Small $\lambda$: Gauss–Newton step, fast compared to large $\lambda$.
- Large $\lambda$: this is a short step similar to gradient descent.

Each iteration:

1. Compute $\mathbf r$ and $\mathbf J$ at the current $\mathbf p$
2. Solve for $\Delta\mathbf p$ (a 6×6 linear system)
3. If $F(\mathbf p + \Delta\mathbf p) < F(\mathbf p)$: accept the step and set $\lambda \leftarrow \lambda/10$ (trust the linear model more)
4. Otherwise: reject the step, set $\lambda \leftarrow 10\lambda$ (take a shorter, safer step) and try again
5. Stop when $\Delta\mathbf p$ is negligible

Starting from the rest pose (or, in firmware, the previous frame's pose), it converges in a handful of iterations.

<details>

<summary>Code</summary>

```python
import sys
import numpy as np

K = 1.42 * np.pi * 2.35**2 * 4.7 / (4 * np.pi) * 1000            # B_r V / 4pi  (mT*mm^3)
S = np.array([[-0.12, 14.1, 0], [12.271, -6.946, 0], [-12.151, -7.154, 0]])   # Hall elements (mm)
Q = np.array([[0, 16, 0], [13.852, -8.008, 0], [-13.852, -8.008, 0]])        # magnets in knob frame
PHI = np.radians([0, -120, 120])

def to_board(raw, i):
    c, s = np.cos(PHI[i]), np.sin(PHI[i]); x, y, z = -raw[0], raw[1], -raw[2]
    return np.array([c * x - s * y, s * x + c * y, z])

def rot(rx, ry, rz):
    cx, sx, cy, sy, cz, sz = np.cos(rx), np.sin(rx), np.cos(ry), np.sin(ry), np.cos(rz), np.sin(rz)
    return (np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]]) @ np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
            @ np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]]))

def magnets(p): # p = x, y, z, roll, pitch, yaw [, k scale]
    R = rot(*p[3:6]); return Q @ R.T + p[:3], R @ [0, 0, 1]

def model(p): # all 9 readings: every magnet at every sensor
    M, m = magnets(p); k = K * (p[6] if len(p) > 6 else 1)
    out = []
    for s in S:
        B = 0
        for Mj in M:
            r = s - Mj; d = np.linalg.norm(r); u = r / d
            B = B + k * (3 * u * (m @ u) - m) / d**3
        out.append(B)
    return np.concatenate(out)

def fit(b, p): # Levenberg-Marquardt
    for _ in range(50):
        r = model(p) - b
        J = np.array([(model(p + e) - model(p)) / 1e-6 for e in np.eye(len(p)) * 1e-6]).T
        A = J.T @ J; dp = np.linalg.solve(A + 1e-3 * np.diag(np.diag(A)), -J.T @ r)
        p = p + dp
        if np.abs(dp).max() < 1e-8: break
    return p, np.sqrt(np.mean((model(p) - b) ** 2))

vals = np.array([-0.80, 4.68, -14.88, 0.37, 3.83, -19.97, -0.95, 2.48, -19.11])
B = np.concatenate([to_board(vals[3*i:3*i+3], i) for i in range(3)])
B *= np.sign(B[2])                               # polarity

for name, p0 in [("pose (k fixed)", np.r_[0, 0, 10, 0, 0, 0]), ("pose + strength", np.r_[0, 0, 10, 0, 0, 0, 1])]:
    p, rms = fit(B, p0.astype(float))
    M, _ = magnets(p)
    print(f"\n{name}: rms residual {rms:.3f} mT" + (f", strength x{p[6]:.3f}" if len(p) > 6 else ""))
    print(f"  center ({p[0]:.3f}, {p[1]:.3f}, {p[2]:.3f}) mm   roll {np.degrees(p[3]):.2f}  "
          f"pitch {np.degrees(p[4]):.2f}  yaw {np.degrees(p[5]):.2f} deg")
    for j in range(3):
        print(f"  M{j+1} = ({M[j][0]:.3f}, {M[j][1]:.3f}, {M[j][2]:.3f})   radius {np.hypot(*M[j][:2]):.3f}")
```

```bash
pose (k fixed): rms residual 0.759 mT
  center (0.183, 0.430, 9.684) mm   roll 1.33  pitch 0.48  yaw -0.25 deg
  M1 = (0.257, 16.425, 10.054)   radius 16.427
  M2 = (13.998, -7.638, 9.382)   radius 15.946
  M3 = (-13.705, -7.515, 9.615)   radius 15.630

pose + strength: rms residual 0.428 mT, strength x1.516
  center (0.233, 0.417, 11.117) mm   roll 1.56  pitch 0.55  yaw -0.36 deg
  M1 = (0.337, 16.411, 11.553)   radius 16.415
  M2 = (14.032, -7.673, 10.767)   radius 15.993
  M3 = (-13.670, -7.501, 11.031)   radius 15.593
```

</details>

**Results:**

| | Independent (section 3) | Rigid LM fit |
|---|---|---|
| $\vec M_1$ (mm) | (0.22, 16.40, 10.06) | (0.26, 16.43, 10.05) |
| $\vec M_2$ (mm) | (13.53, −7.54, 9.44) | (14.00, −7.64, 9.38) |
| $\vec M_3$ (mm) | (−13.21, −7.40, 9.67) | (−13.71, −7.52, 9.62) |
| Spacing (mm) | 26.7–27.4 (drifts) | 27.70–27.72  |
| Knob pose | ... | x 0.18, y 0.43, z 9.68 mm; roll 1.33°, pitch 0.48°, yaw −0.25° |


## 5. Implementation

1. Calibrate at rest (and when user holds both buttons). Run the fit and keep the rest pose $\mathbf p_0$ and the leftover residual $\vec o = \vec B_{rest} - \vec B^{model}(\mathbf p_0)$ as a fixed offset. To absorb geometry errors like the 0.5 mm above, plus sensor offsets and dipole-model error.
2. Every frame: fit $\mathbf r(\mathbf p) = \vec B^{model}(\mathbf p) + \vec o - \vec B$, starting from the previous frame's pose (2–3 iterations is enough), and report $\mathbf p - \mathbf p_0$.


