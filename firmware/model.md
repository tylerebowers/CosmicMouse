## 1. Forward model

Magnetic Dipole Field:

```math
\vec{B}(\vec{r})=\frac{\mu_{0}}{4\pi}\,\frac{3 \hat{r}(\vec{m}\cdot\hat{r})-\vec{m}}{r^{3}}
```

- $\mu_0 = 4\pi\cdot10^{-7}\ \text{H/m}$ = permeability of free space
- $\vec{r}$ = position vector from the dipole center to the observer
- $r = |\vec{r}|$ = distance from the dipole center to the observer
- $\hat{r} = \vec{r}/r$ = unit vector from the dipole center to the observer
- $\vec{m}$ = magnetic moment of the dipole

Magnetic Moment (3/16 in dia × 3/16 in height cylinder, N52):

```math
|\vec{m}| = \frac{B_r V}{\mu_0}
= \frac{1.42\ \text{T}\cdot 8.154\times10^{-8}\ \text{m}^3}{4\pi\cdot10^{-7}\ \text{H/m}}
\approx 0.0921\ \text{A}\cdot\text{m}^2
```

- $B_r = 1.42\ \text{T}$ = residual induction (remanence) of N52
- $V = \pi\left(\tfrac{4.7\ \text{mm}}{2}\right)^2 \cdot 4.7\ \text{mm} = 81.54\ \text{mm}^3 = 8.154\times10^{-8}\ \text{m}^3$

(4.7mm used instead of 4.7625mm = 3/16in to account for the coating)

We can pull out a fixed term $k$, using: $\vec m = |\vec m|\,\hat m$ and $|\vec m| = B_rV/\mu_0$:

```math
\vec B = k\,\frac{3\hat r(\hat m\cdot\hat r) - \hat m}{r^3},\quad
k = \frac{\mu_0|\vec m|}{4\pi} = \frac{B_rV}{4\pi}
```

<!-- (1.42⋅81.54)/(4⋅pi) -->
$k = 1.42T \cdot 81.54mm^3 / 4π = 9.21 T \cdot mm^3 = 9214 mT \cdot mm^3$. 

Vector $\vec r$ has 3 components, and one sensor gives 3 measurements, so if $\hat m$ (unit vector of moment) is known, the system can be solved.


## 2. Inverting one dipole

A dipole has axial symmetry (in this case, about the vector normal to cylindrical face). Because of this, I will use spherical coordinates, $r$ = distance from the origin (sensor), $a$ angle rotating from the $z$ axis $c = cos(a) = \hat r \cdot \hat m$ (normal to the top face of the sensor), and $\phi$ azimuth rotation about the $z$ axis.

Knowns:
Sensor position: $\vec{S} = (0, 14.5, 0) mm$  
Magnet strength: $k = 9214 mT \cdot mm^3$  
Measured field: $\vec{B} = (0, 5.091, 20.885) mT$ 

```math
\vec B = k\,\frac{3\hat r(\hat m\cdot\hat r) - \hat m}{r^3}, \quad \hat m = \hat z\; \text{(magnet axis is along z)} \\
\vec B \cdot \hat m = \frac{k}{r^3} (3\hat r(\hat m\cdot\hat r) - \hat m) \cdot \hat m 
= \frac{k}{r^3} (3\hat (\hat m\cdot\hat r)(\hat m\cdot\hat r) - \hat m\cdot \hat m) 
= \frac{k}{r^3} (3c^2 - 1) \\

\vec B = \frac{k}{r^3} (3\hat rc - \hat m) \rightarrow |\vec B|^2 = \vec B \cdot \vec B = (\frac{k}{r^3})^2 (9c^2 - 6c^2 +1) = \frac{k}{r^3} \sqrt{ (3c^2 + 1)}\\
\vec B \cdot \hat m =\frac{k}{r^3} (3c^2 - 1); \text{Amount of field along the magnet axis} \\
| \vec B | = \frac{k}{r^3} \sqrt{ (3c^2 + 1)}; \text{Magnitude of field} \\ 
```

**Now to solve for** $c$. Let $u$ be the cosine of the angle between the field and the magnet’s axis.

```math
u = \frac{\vec B\cdot\hat m}{|\vec B|} = \frac{3c^2-1}{\sqrt{3c^2+1}}\;\; \rightarrow \;\; \text{(Let }s = 3c^2 \text{)} \;\; u^2 = \frac{(s-1)^2}{s+1} \\
u^2(s+1) = s^2 - 2s + 1 \;\; \rightarrow \;\; s^2 - (2+u^2)s + (1-u^2) = 0
```

Quadratic formula:

```math
s = \frac{(2+u^2) + \sqrt{(2+u^2)^2 - 4(1-u^2) = u^4+8u^2}}{2} = \frac{(2+u^2) + u\sqrt{u^2+8}}{2}
```
We know that the magnet is above the sensor, so $c > 0$. (Durning initialization we can check the sign of the z reading to see if the magnet is flipped up or down; if flipped all readings will be inverted before going into calculations.)

```math
c = +\sqrt{\frac{1}{3} \cdot \left[ \frac{(2+u^2) + u\sqrt{u^2+8}}{2} \right]}
```

**Now to solve for** $r$.

```math
r = \left(\frac{k\sqrt{s+1}}{|\vec B|}\right)^{1/3}= \left(\frac{k\sqrt{3c^2+1}}{|\vec B|}\right)^{1/3}
```

Position:

```math
\vec s = r\,\left[ sin(a) \frac{(B_x, B_y, 0)}{\sqrt{B_x^2 + B_y^2}+cos(a) \hat z} \right]
```



**Example:**

Measurements from U3: X -2.26mT, Y -5.12mT, Z 15.40mT 

Angle $a$: 

```math
| \vec B |  = \sqrt{B_x^2 + B_y^2 + B_z^2} = \sqrt{(-2.26)^2 + (-5.12)^2 + (15.40)^2} = 16.385 mT\\
u = \frac{\vec B \cdot \hat m = 15.40}{16.385} = 0.93986\\
s = \frac{(2.88334) + 0.93986\sqrt{8.88334}}{2} = 2.84229\\
c = -\sqrt{2.84229/3} = -0.97336 \rightarrow \cos^{-1}(0.97336) = 13.25\degree \quad (\vec m = -\vec z) \\
```

<!-- (15.40/(((−2.26)^2 + (−5.12)^2 + (15.40)^2)^0.5))^2 -->
<!-- −sqrt(({(2.88334) + 0.93986 sqrt{8.88334}}/{2})/3) -->

c is negative because the moment points down while $\hat r$ points up. That puts the magnet $13.25\degree$ off straight up.

Distance $r$:

```math
r = \left(\frac{9214\sqrt{3.84229}}{16.385}\right)^{1/3} = 10.330 mm
```

Direction $\hat r$:

```math
\hat r = \frac{\dfrac{r^3}{k}\vec B + \hat m}{3c} = \frac{\frac{10.33^3}{9214}(2.26, 5.12, -15.40) + (0,\ 0,\ -1) = (0.27036, 0.61250, -2.84229)}{3\cdot−0.97336=−2.92008} = (-0.0926, -0.2098, 0.9734)
```

Magnet position:

*y is flipped due to U3 sensor's location*

```math
\vec M = \vec S + r\,\hat r = (0,\ 14.5,\ 0) + 10.330\times(-0.0926,\ 0.2098,\ 0.9734) = \mathbf{(-0.96,\ 16.67,\ 10.06)\ mm}
```

The next step is to include the other 2 magnets and remove the cross-talk.


