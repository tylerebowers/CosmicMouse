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