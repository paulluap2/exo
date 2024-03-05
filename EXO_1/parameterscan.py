import numpy as np
import subprocess
import matplotlib.pyplot as plt

# Parameters
# TODO adapt to what you need (folder path executable input filename)
repertoire = ''  # Path to the compiled code (NB: ./ is not required on Windows)
executable = 'Exercice1_student.exe'  # Name of the executable (NB: .exe extension is required on Windows)
input_filename = 'configuration.in.example'  # Name of the input file


nsteps = np.array([100,200,500,1000,2000,5000,10000,20000,50000,100000]) # TODO change
nsimul = len(nsteps)  # Number of simulations to perform

tfin = 21.64281319 # TODO: Verify that the value of tfin is EXACTLY the same as in the input file


dt = tfin / nsteps

# Analysis
# TODO insert the values
m = 0.056  

omega = 10
mu = 6
R = 0.033
rho = 1.2
v = 5
g = 9.81
Ct=0.01


# add the other variables
# TODO: Insert here the expressions for the exact final solution

a = mu*(R**3)*rho*2*np.pi*omega/m


#Cas avec g==0
"""

x_th = (v/a)*np.sin(a*tfin)
y_th = (v/a)*(1-np.cos(a*tfin))
vx_th = v*np.cos(a*tfin)
vy_th = v*np.sin(a*tfin)

"""


#Cas avec g!=0

x_th  = (g/a)*(tfin-np.sin(a*tfin)/a)
y_th  = (g/a**2)*(np.cos(a*tfin)-1)
vx_th = (g/a)*(1-np.cos(a*tfin))
vy_th = -(g/a)*np.sin(a*tfin)
L_th = 2*np.pi*g/a**2

"""
... and other parameters
"""

paramstr = 'nsteps'  # Parameter name to scan
param = nsteps  # Parameter values to scan

# Simulations
outputs = []  # List to store output file names
vy_list = []
for i in range(nsimul):
    output_file = f"{paramstr}={param[i]}.out"
    outputs.append(output_file)
    cmd = f"{repertoire}{executable} {input_filename} {paramstr}={param[i]:.15g} output={output_file}"
    print(cmd)
    subprocess.run(cmd, shell=True)
    print('Done.')


error = np.zeros(nsimul)
error_energy = np.zeros(nsimul)
fig, ax = plt.subplots(constrained_layout=True)

lw = 1.5 # line width. TODO: adjust if needed
fs = 16  # font size. TODO: adjust if needed

for i in range(nsimul):  # Iterate through the results of all simulations
    data = np.loadtxt(outputs[i])  # Load the output file of the i-th simulation
    t = data[:, 0]

    xx = data[-1, 1]  # final position, velocity, energy
    yy = data[-1, 2]
    vx = data[-1, 3]
    vy = data[-1, 4]
    En = data[-1, 5]
    vy_list.append(vy)
    # TODO compute the error for each simulation
    error[i] = np.sqrt((xx-x_th)**2+(yy-y_th)**2) 
    error_energy[i] = max(data[:,5])-min(data[:,5])

    ax.plot(t, data[:,5], label=f'nsteps={nsteps[i]}')

ax.set_xlabel('t [s]', fontsize=fs)
ax.set_ylabel('E [J]', fontsize=fs)
ax.legend()
plt.show()




fig, ax = plt.subplots(constrained_layout=True)
ax.plot(data[:, 1], data[:, 2])
ax.set_xlabel('x [m]', fontsize=fs)
ax.set_ylabel('y [m]', fontsize=fs)
ax.scatter(x_th, y_th, color='red', label='Exact final position')
#ax.axis('equal')
#ax.scatter(L_th, 0.0, color='purple', label='L theoretical')
ax.legend()


# uncomment the following 2 lines if you want debug
#import pdb
#pbd.set_trace()
plt.figure()
plt.loglog(dt, error, 'r+-', linewidth=lw)
plt.xlabel('Delta t [s]', fontsize=fs)
plt.ylabel('final position error [m]', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)

plt.figure()
plt.loglog(dt, error_energy, 'r+-', linewidth=lw)
plt.xlabel('Delta t [s]', fontsize=fs)
plt.ylabel('Energy error [J]', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)

"""
Si on n'a pas la solution analytique: on représente la quantite voulue
(ci-dessous v_y, TODO: modifier selon vos besoins)
en fonction de (Delta t)^norder, ou norder est un entier.
"""
norder = 1  # TODO: Modify if needed

plt.figure()
plt.plot(dt**norder, vy_list, 'k+-', linewidth=lw)
plt.axhline(vy_th, color='red', label='Theoretical vy')
plt.xlabel('Delta t [s]', fontsize=fs)
plt.ylabel('v_y [m/s]', fontsize=fs)
plt.xticks(fontsize=fs)
plt.yticks(fontsize=fs)
plt.grid(True)
plt.legend()

plt.show()
