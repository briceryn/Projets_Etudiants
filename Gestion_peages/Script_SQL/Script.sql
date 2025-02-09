-- Création du schéma pour organiser les tables du projet
CREATE SCHEMA Gestion_peages;


-- Table des informations des automobilistes

CREATE TABLE Gestion_peages.Automobiliste (
	id SERIAL PRIMARY KEY,       -- Identifiant unique pour chaque automobiliste
	nom TEXT,                    -- Nom de l'automobiliste
	prenom TEXT,                 -- Prénom de l'automobiliste
	statut_abo VARCHAR(10),      -- Indique si l'automobiliste est "abonne" ou "non-abonne"
	adresse TEXT,                -- Adresse postale de l'automobiliste
	email VARCHAR(100)           -- Adresse e-mail de l'automobiliste
);


-- Table des informations des abonnements

CREATE TABLE Gestion_peages.Abonnement (
	id SERIAL PRIMARY KEY,              -- Identifiant unique pour chaque abonnement
	niveau VARCHAR(50),                 -- Niveau d'abonnement (par exemple, "Standard", "Premium")
	tarif_mensuel NUMERIC(10, 2),       -- Tarif mensuel pour le prêt du badge
	tarif_reduction NUMERIC(10, 2),     -- Pourcentage de réduction appliqué sur les trajets
	date_debut DATE,                    -- Date de début de l'abonnement
	date_fin DATE,                      -- Date de fin de l'abonnement
	id_automobiliste INT REFERENCES Automobiliste(id) -- Lien avec l'automobiliste concerné
);

-- Table des badges d'identification des abonnés

CREATE TABLE Gestion_peages.Badge (
	id SERIAL PRIMARY KEY,              -- Identifiant unique pour chaque badge
	id_automobiliste INT REFERENCES Automobiliste(id), -- Automobiliste possédant le badge
	date_emission DATE,                 -- Date d'émission du badge
	actif BOOLEAN DEFAULT TRUE          -- Indique si le badge est actif (par défaut, TRUE)
);


-- Table des informations des péages

CREATE TABLE Gestion_peages.Peage (
	id SERIAL PRIMARY KEY,              -- Identifiant unique pour chaque péage
	localisation TEXT,                  -- Localisation du péage
	nombre_passages INT DEFAULT 0       -- Nombre total de passages enregistrés (par défaut, 0)
);


-- Table des tickets émis pour les trajets des automobilistes

CREATE TABLE Gestion_peages.Ticket (
	id SERIAL PRIMARY KEY,              -- Identifiant unique pour chaque ticket
	date_entree TIMESTAMP,              -- Date et heure d'entrée sur l'autoroute
	peage_entree INT REFERENCES Peage(id), -- Péage d'entrée (lien avec la table Peage)
	date_sortie TIMESTAMP,              -- Date et heure de sortie de l'autoroute
	peage_sortie INT REFERENCES Peage(id), -- Péage de sortie (lien avec la table Peage)
	montant NUMERIC(10, 2),             -- Montant facturé pour le trajet
	id_automobiliste INT REFERENCES Automobiliste(id) -- Automobiliste concerné
);


-- Table des trajets réalisés

CREATE TABLE Gestion_peages.Trajet (
	id SERIAL PRIMARY KEY,              -- Identifiant unique pour chaque trajet
	id_ticket INT REFERENCES Ticket(id),-- Lien vers le ticket associé au trajet
	id_peage_entree INT REFERENCES Peage(id), -- Péage d'entrée
	id_peage_sortie INT REFERENCES Peage(id), -- Péage de sortie
	date DATE,                          -- Date du trajet
	id_automobiliste INT REFERENCES Automobiliste(id) -- Automobiliste ayant effectué le trajet
);


-- Table des plaques des véhicules pour des raisons de sécurité

CREATE TABLE Gestion_peages.Plaque (
	numero_plaque VARCHAR(15) PRIMARY KEY, -- Numéro de plaque d'immatriculation unique
	id_automobiliste INT REFERENCES Automobiliste(id) -- Automobiliste possédant le véhicule
);

-- Insertion des 30 automobilistes
INSERT INTO Gestion_peages.Automobiliste (nom, prenom, statut_abo, adresse, email)
VALUES
('Dupont', 'Jean', 'abonne', '123 Rue Principale, Paris', 'jean.dupont@example.com'),
('Martin', 'Sophie', 'non-abonne', '456 Rue des Lilas, Lyon', 'sophie.martin@example.com'),
('Bernard', 'Paul', 'abonne', '789 Avenue de la République, Marseille', 'paul.bernard@example.com'),
('Durand', 'Marie', 'non-abonne', '321 Rue du Centre, Toulouse', 'marie.durand@example.com'),
('Moreau', 'Luc', 'abonne', '654 Boulevard Victor Hugo, Bordeaux', 'luc.moreau@example.com'),
('Simon', 'Claire', 'non-abonne', '987 Chemin des Fleurs, Lille', 'claire.simon@example.com'),
('Lemoine', 'Julie', 'abonne', '147 Allée des Érables, Nice', 'julie.lemoine@example.com'),
('Roux', 'Antoine', 'non-abonne', '258 Place du Marché, Nantes', 'antoine.roux@example.com'),
('Blanc', 'Alice', 'abonne', '369 Rue de la Gare, Strasbourg', 'alice.blanc@example.com'),
('Fontaine', 'Michel', 'non-abonne', '741 Rue de la Fontaine, Rennes', 'michel.fontaine@example.com'),
('Thomas', 'Elodie', 'abonne', '852 Avenue des Champs, Dijon', 'elodie.thomas@example.com'),
('Perrin', 'Nicolas', 'non-abonne', '963 Rue de l Église, Orléans', 'nicolas.perrin@example.com'),
('Bonnet', 'Emma', 'abonne', '159 Impasse des Jardins, Grenoble', 'emma.bonnet@example.com'),
('Robert', 'Lucas', 'non-abonne', '753 Rue des Vosges, Reims', 'lucas.robert@example.com'),
('Richard', 'Chloé', 'abonne', '357 Avenue de Provence, Metz', 'chloe.richard@example.com'),
('Petit', 'Hugo', 'non-abonne', '951 Chemin de la Forêt, Montpellier', 'hugo.petit@example.com'),
('Girard', 'Manon', 'abonne', '159 Boulevard Saint-Michel, Clermont-Ferrand', 'manon.girard@example.com'),
('Muller', 'Jules', 'non-abonne', '753 Rue de Lorraine, Nancy', 'jules.muller@example.com'),
('Lefèvre', 'Camille', 'abonne', '357 Place Saint-Pierre, Tours', 'camille.lefevre@example.com'),
('Mercier', 'Noah', 'non-abonne', '951 Avenue Jean Moulin, Saint-Étienne', 'noah.mercier@example.com'),
('Dupuy', 'Léa', 'abonne', '159 Rue du Moulin, Angers', 'lea.dupuy@example.com'),
('Michel', 'Théo', 'non-abonne', '753 Boulevard des Étoiles, Brest', 'theo.michel@example.com'),
('David', 'Sarah', 'abonne', '357 Allée des Peupliers, Le Havre', 'sarah.david@example.com'),
('Bertrand', 'Axel', 'non-abonne', '951 Rue des Alizés, Perpignan', 'axel.bertrand@example.com'),
('Morel', 'Louise', 'abonne', '159 Avenue de Bretagne, Amiens', 'louise.morel@example.com'),
('Fournier', 'Émile', 'non-abonne', '753 Rue de Normandie, Caen', 'emile.fournier@example.com'),
('Girault', 'Zoé', 'abonne', '357 Place des Arts, Limoges', 'zoe.girault@example.com'),
('Lucas', 'Mathis', 'non-abonne', '951 Rue des Cévennes, Pau', 'mathis.lucas@example.com'),
('Dumont', 'Anaïs', 'abonne', '159 Boulevard du Midi, Bayonne', 'anais.dumont@example.com'),
('Renaud', 'Eva', 'non-abonne', '753 Rue des Pyrénées, Tarbes', 'eva.renaud@example.com');

-- Insertion de 10 abonnements correspondant à des automobilistes existants
INSERT INTO Gestion_peages.Abonnement (niveau, tarif_mensuel, tarif_reduction, date_debut, date_fin, id)
VALUES
('Standard', 20.00, 10.00, '2023-01-01', '2023-12-31', 1),
('Premium', 30.00, 20.00, '2023-01-01', '2023-12-31', 3),
('Basic', 15.00, 5.00, '2023-01-01', '2023-12-31', 5),
('Standard', 20.00, 10.00, '2023-01-01', '2023-12-31', 7),
('Premium', 30.00, 20.00, '2023-01-01', '2023-12-31', 9),
('Basic', 15.00, 5.00, '2023-01-01', '2023-12-31', 11),
('Standard', 20.00, 10.00, '2023-01-01', '2023-12-31', 13),
('Premium', 30.00, 20.00, '2023-01-01', '2023-12-31', 15),
('Basic', 15.00, 5.00, '2023-01-01', '2023-12-31', 17),
('Standard', 20.00, 10.00, '2023-01-01', '2023-12-31', 19);

-- Insertion de badges pour les automobilistes abonnés
INSERT INTO Gestion_peages.Badge (id_automobiliste, date_emission, actif)
VALUES
(1, '2023-01-05', TRUE),
(3, '2023-02-10', TRUE),
(5, '2023-03-15', TRUE),
(7, '2023-04-20', TRUE),
(9, '2023-05-25', TRUE),
(11, '2023-06-30', TRUE),
(13, '2023-07-05', TRUE),
(15, '2023-08-10', TRUE),
(17, '2023-09-15', TRUE),
(19, '2023-10-20', TRUE);

-- Insertion de 5 péages
INSERT INTO Gestion_peages.Peage (localisation, nombre_passages)
VALUES
('Paris - Porte de Bercy', 1500),
('Lyon - A7 Nord', 1200),
('Marseille - Tunnel Prado', 800),
('Bordeaux - Rocade Ouest', 1000),
('Lille - A25 Est', 600);

-- Insertion de 10 tickets liés aux automobilistes et péages
INSERT INTO Gestion_peages.Ticket (date_entree, peage_entree, date_sortie, peage_sortie, montant, id_automobiliste)
VALUES
('2023-11-01 08:30:00', 1, '2023-11-01 09:00:00', 2, 15.00, 1),
('2023-11-02 10:00:00', 3, '2023-11-02 10:45:00', 4, 12.50, 3),
('2023-11-03 14:00:00', 2, '2023-11-03 14:30:00', 5, 10.00, 5),
('2023-11-04 09:15:00', 4, '2023-11-04 10:00:00', 1, 20.00, 7),
('2023-11-05 07:00:00', 5, '2023-11-05 07:45:00', 3, 18.00, 9),
('2023-11-06 15:00:00', 1, '2023-11-06 15:30:00', 2, 11.00, 11),
('2023-11-07 11:00:00', 3, '2023-11-07 11:40:00', 4, 16.00, 13),
('2023-11-08 12:30:00', 2, '2023-11-08 13:15:00', 5, 13.50, 15),
('2023-11-09 10:00:00', 4, '2023-11-09 10:50:00', 1, 17.00, 17),
('2023-11-10 08:30:00', 5, '2023-11-10 09:00:00', 3, 19.00, 19);

-- Insertion de 10 trajets correspondant aux tickets créés
INSERT INTO Gestion_peages.Trajet (id_ticket, id_peage_entree, id_peage_sortie, date, id_automobiliste)
VALUES
(1, 1, 2, '2023-11-01', 1),
(2, 3, 4, '2023-11-02', 3),
(3, 2, 5, '2023-11-03', 5),
(4, 4, 1, '2023-11-04', 7),
(5, 5, 3, '2023-11-05', 9),
(6, 1, 2, '2023-11-06', 11),
(7, 3, 4, '2023-11-07', 13),
(8, 2, 5, '2023-11-08', 15),
(9, 4, 1, '2023-11-09', 17),
(10, 5, 3, '2023-11-10', 19);

-- Insertion de plaques pour 10 automobilistes
INSERT INTO Gestion_peages.Plaque (numero_plaque, id_automobiliste)
VALUES
('AA-123-BB', 1),
('CC-456-DD', 3),
('EE-789-FF', 5),
('GG-012-HH', 7),
('II-345-JJ', 9),
('KK-678-LL', 11),
('MM-901-NN', 13),
('OO-234-PP', 15),
('QQ-567-RR', 17),
('SS-890-TT', 19);

--Quels sont les peages les plus empruntes ?
SELECT p.localisation, COUNT(t.id) AS nombre_passages
FROM Gestion_peages.Peage p
JOIN Gestion_peages.Ticket t ON p.id = t.peage_entree OR p.id = t.peage_sortie
GROUP BY p.localisation
ORDER BY nombre_passages DESC
LIMIT 10;

--Pour un utilisateur abonne donne en parametre, lui retourner la liste de ses trajets 
SELECT t.date_entree, p1.localisation AS peage_entree, p2.localisation AS peage_sortie, t.montant
FROM Gestion_peages.Ticket t
JOIN Gestion_peages.Peage p1 ON t.peage_entree = p1.id
JOIN Gestion_peages.Peage p2 ON t.peage_sortie = p2.id
WHERE t.id_automobiliste = ?;

--Quels sont les automobilistes non abonnes qui font souvent le meme trajet 
SELECT a.nom, a.prenom, p1.localisation AS peage_entree, p2.localisation AS peage_sortie, COUNT(*) AS nb_trajets
FROM Gestion_peages.Ticket t
JOIN Gestion_peages.Automobiliste a ON t.id_automobiliste = a.id
JOIN Gestion_peages.Peage p1 ON t.peage_entree = p1.id
JOIN Gestion_peages.Peage p2 ON t.peage_sortie = p2.id
WHERE a.statut_abo = 'non-abonne'
GROUP BY a.id, a.nom, a.prenom, p1.localisation, p2.localisation
HAVING COUNT(*) > 1
ORDER BY nb_trajets DESC;

--Quelles sont les portions d’autoroute les plus empruntees 
SELECT p1.localisation AS peage_entree, p2.localisation AS peage_sortie, COUNT(*) AS nb_passages
FROM Gestion_peages.Ticket t
JOIN Gestion_peages.Peage p1 ON t.peage_entree = p1.id
JOIN Gestion_peages.Peage p2 ON t.peage_sortie = p2.id
GROUP BY p1.localisation, p2.localisation
ORDER BY nb_passages DESC
LIMIT 10;

--Quels sont les automobilistes dont l’abonnement est termine mais qui ont encore en leur possession un badge ?
SELECT a.nom, a.prenom, b.id AS badge_id
FROM Gestion_peages.Automobiliste a
JOIN Gestion_peages.Abonnement ab ON a.id = ab.id_automobiliste
JOIN Gestion_peages.Badge b ON a.id = b.id_automobiliste
WHERE ab.date_fin < CURRENT_DATE AND b.actif = TRUE;