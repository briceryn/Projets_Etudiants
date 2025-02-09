#!/usr/bin/env python
# coding: utf-8

# In[1]:


import math


# In[7]:


# Constantes
angle = 80  # Angle de chute de la neige par rapport à la verticale (en degrés)
vitesse_voiture = 110  # Vitesse de la voiture en km/h


# In[ ]:





# In[10]:


# Calcul de la vitesse de la neige par rapport au sol
vitesse_sol = vitesse_voiture * math.cos(math.radians(angle))


# In[11]:


# Calcul de la vitesse de la neige par rapport à la voiture
vitesse_voiture_relative = vitesse_voiture * math.sin(math.radians(angle))


# In[12]:


# Affichage des résultats
print("Vitesse de la neige par rapport au sol:", vitesse_sol, "km/h")
print("Vitesse de la neige par rapport à la voiture:", vitesse_voiture_relative, "km/h")


# In[ ]:




