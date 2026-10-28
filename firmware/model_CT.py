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