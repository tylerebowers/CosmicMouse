## 1. Forward model

Magnetic Dipole Field:

```math
\vec{B}(\vec{r})=\frac{\mu_{0}}{4\pi}\,\frac{3\hat{r}(\vec{m}\cdot\hat{r})-\vec{m}}{r^{3}}
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
\vec B = k\,\frac{3\hat r(\hat m\cdot\hat r) - \hat m}{r^3},\qquad
k = \frac{\mu_0|\vec m|}{4\pi} = \frac{B_rV}{4\pi}
```

<!-- (1.42⋅81.54)/(4⋅pi) -->
$k = 1.42T \cdot 81.54mm^3 / 4π = 9.21 T \cdot mm^3 = 9214 mT \cdot mm^3$. 

Vector $\vec r$ has 3 components, and one sensor gives 3 measurements, so if $\hat m$ (unit vector of moment) is known, the system can be solved.
