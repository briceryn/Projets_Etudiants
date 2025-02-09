#!/usr/bin/env python
# coding: utf-8

# In[14]:


import numpy as np
import matplotlib.pyplot as plt


# In[15]:


# Constantes
P = 100  # Puissance mécanique constante fournie par le cycliste
k = 0.1  # Constante de proportionnalité des forces de frottement
m = 70   # Masse du cycliste et du vélo


# In[16]:


# Fonction dérivée de la vitesse dv/dx
def dv_dx(v):
    return (P - k * v**3) / (m * v**2)

# Fonction de résolution de l'équation différentielle
def solve_differential_eq(v0, x):
    v = np.zeros_like(x)  # Initialisation de v(x) avec des zéros
    v[0] = v0             # Condition initiale : v(x=0) = v0

    # Résolution numérique de l'équation différentielle avec la méthode d'Euler
    for i in range(1, len(x)):
        h = x[i] - x[i-1]  # Pas de discrétisation
        v[i] = v[i-1] + h * dv_dx(v[i-1])

    return v


# In[17]:


# Conditions initiales
v0 = 10  # Vitesse initiale du cycliste
x0 = 0   # Position initiale
xf = 100 # Position finale
n_points = 1000  # Nombre de points pour la discrétisation


# In[18]:


# Discrétisation de la position
x = np.linspace(x0, xf, n_points)


# In[19]:


# Résolution de l'équation différentielle
v = solve_differential_eq(v0, x)


# In[20]:


# Tracé de la vitesse en fonction de la position
plt.plot(x, v)
plt.xlabel('Position (x)')
plt.ylabel('Vitesse (v)')
plt.title('Mouvement du cycliste')
plt.grid(True)
plt.show()


# In[ ]:




