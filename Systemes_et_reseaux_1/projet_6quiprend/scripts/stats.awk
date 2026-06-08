#!/usr/bin/awk -f
# Script de statistiques pour le jeu "6 qui prend"
# Usage: awk -f stats.awk logs/parties.log

BEGIN {
    print "================================================"
    print "      STATISTIQUES - JEU 6 QUI PREND"
    print "================================================"
    print ""
    
    # Initialisation des variables
    total_parties = 0
    total_joueurs = 0
    total_coups = 0
    total_tetes = 0
}

# Analyse des lignes de log
{
    # Compter les parties démarrées
    if ($0 ~ /Partie.*démarrée/) {
        total_parties++
        
        # Extraire le nombre de joueurs
        for (i=1; i<=NF; i++) {
            if ($i ~ /^[0-9]+$/ && prev_word == "avec") {
                nb_joueurs = $i
                total_joueurs += nb_joueurs
                joueurs_partie[nb_joueurs]++
                break
            }
            prev_word = $i
        }
    }
    
    # Compter les coups joués
    if ($0 ~ /joue carte/) {
        total_coups++
        
        # Extraire le joueur
        for (i=1; i<=NF; i++) {
            if ($i == "Joueur" && $(i+1) ~ /^[0-9]+$/) {
                joueur = $(i+1)
                coups_joueur[joueur]++
                break
            }
        }
        
        # Extraire les têtes prises
        for (i=1; i<=NF; i++) {
            if ($i == "tête" || $i == "têtes") {
                if ($(i-1) ~ /^[0-9]+$/) {
                    tetes = $(i-1)
                    total_tetes += tetes
                    tetes_joueur[joueur] += tetes
                }
                break
            }
        }
    }
    
    # Compter les parties terminées
    if ($0 ~ /Partie.*terminée/) {
        parties_terminees++
        
        # Trouver le gagnant
        for (i=1; i<=NF; i++) {
            if ($i == "Gagnant:" && $(i+1) == "Joueur" && $(i+2) ~ /^[0-9]+$/) {
                gagnant = $(i+2)
                victoires[gagnant]++
                break
            }
        }
        
        # Extraire le score du gagnant
        for (i=1; i<=NF; i++) {
            if ($i == "têtes") {
                if ($(i-1) ~ /^[0-9]+$/) {
                    score = $(i-1)
                    scores_gagnants[gagnant] = score
                }
                break
            }
        }
    }
    
    # Suivre les connexions/déconnexions
    if ($0 ~ /Nouveau client/) {
        connexions++
        # Extraire l'adresse IP
        for (i=1; i<=NF; i++) {
            if ($i ~ /^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+/) {
                ip = $i
                connexions_ip[ip]++
                break
            }
        }
    }
    
    if ($0 ~ /Client déconnecté/) {
        deconnexions++
    }
}

END {
    # ========== 1. STATISTIQUES GÉNÉRALES ==========
    print "1. STATISTIQUES GÉNÉRALES"
    print "   -----------------------"
    printf "   Parties démarrées:    %3d\n", total_parties
    printf "   Parties terminées:    %3d\n", parties_terminees
    printf "   Coups joués:          %3d\n", total_coups
    printf "   Têtes de bœuf totales: %3d\n", total_tetes
    printf "   Joueurs totaux:       %3d\n", total_joueurs
    if (total_parties > 0) {
        printf "   Moyenne joueurs/partie: %.1f\n", total_joueurs/total_parties
        printf "   Moyenne coups/partie:   %.1f\n", total_coups/total_parties
        printf "   Moyenne têtes/coup:     %.1f\n", total_tetes/total_coups
    }
    print ""
    
    # ========== 2. RÉPARTITION DES JOUEURS ==========
    print "2. RÉPARTITION DES JOUEURS PAR PARTIE"
    print "   -----------------------------------"
    for (nb in joueurs_partie) {
        printf "   %2d joueurs: %3d partie(s) (%.1f%%)\n", 
               nb, joueurs_partie[nb], (joueurs_partie[nb]/total_parties)*100
    }
    print ""
    
    # ========== 3. CLASSEMENT DES JOUEURS ==========
    print "3. CLASSEMENT DES JOUEURS"
    print "   ----------------------"
    
    # Trier les joueurs par nombre de victoires
    n = 0
    for (j in victoires) {
        joueurs_tries[++n] = j
    }
    
    # Tri à bulles simple
    for (i=1; i<=n; i++) {
        for (j=i+1; j<=n; j++) {
            if (victoires[joueurs_tries[j]] > victoires[joueurs_tries[i]]) {
                temp = joueurs_tries[i]
                joueurs_tries[i] = joueurs_tries[j]
                joueurs_tries[j] = temp
            }
        }
    }
    
    # Afficher le classement
    for (i=1; i<=n && i<=10; i++) {
        j = joueurs_tries[i]
        printf "   %2d. Joueur %2d: %2d victoire(s)", i, j, victoires[j]
        if (scores_gagnants[j]) {
            printf " (%.1f têtes/victoire)", scores_gagnants[j]/victoires[j]
        }
        if (coups_joueur[j]) {
            printf ", %3d coup(s)", coups_joueur[j]
        }
        if (tetes_joueur[j]) {
            printf ", %3d tête(s)", tetes_joueur[j]
        }
        print ""
    }
    print ""
    
    # ========== 4. STATISTIQUES DES COUPS ==========
    print "4. STATISTIQUES DES COUPS"
    print "   -----------------------"
    if (total_coups > 0) {
        # Trouver le joueur le plus actif
        max_coups = 0
        joueur_actif = 0
        for (j in coups_joueur) {
            if (coups_joueur[j] > max_coups) {
                max_coups = coups_joueur[j]
                joueur_actif = j
            }
        }
        printf "   Joueur le plus actif: Joueur %s (%d coups)\n", 
               joueur_actif, max_coups
        
        # Trouver le joueur avec le moins de têtes en moyenne
        min_tetes_moy = 1000
        joueur_economique = 0
        for (j in tetes_joueur) {
            if (coups_joueur[j] > 0) {
                moyenne = tetes_joueur[j] / coups_joueur[j]
                if (moyenne < min_tetes_moy) {
                    min_tetes_moy = moyenne
                    joueur_economique = j
                }
            }
        }
        printf "   Joueur le plus économique: Joueur %s (%.2f têtes/coup)\n",
               joueur_economique, min_tetes_moy
    }
    print ""
    
    # ========== 5. CONNEXIONS RÉSEAU ==========
    print "5. STATISTIQUES RÉSEAU"
    print "   -------------------"
    printf "   Connexions totales:    %3d\n", connexions
    printf "   Déconnexions totales:  %3d\n", deconnexions
    printf "   Clients actifs max:    %3d\n", connexions - deconnexions
    
    # Adresses IP uniques
    ips_uniques = 0
    for (ip in connexions_ip) {
        ips_uniques++
    }
    printf "   Adresses IP uniques:   %3d\n", ips_uniques
    
    if (ips_uniques > 0) {
        print "   Adresses IP fréquentes:"
        count = 0
        for (ip in connexions_ip) {
            if (connexions_ip[ip] > 1 && count < 5) {
                printf "     %s: %d connexion(s)\n", ip, connexions_ip[ip]
                count++
            }
        }
    }
    print ""
    
    # ========== 6. ANALYSE TEMPORELLE ==========
    print "6. ANALYSE TEMPORELLE"
    print "   ------------------"
    if (NR > 0) {
        # Première et dernière ligne pour calculer la durée
        first_line = NR - 100  # approximatif
        if (first_line < 1) first_line = 1
        
        printf "   Première entrée: ligne %d\n", first_line
        printf "   Dernière entrée:  ligne %d\n", NR
        printf "   Total lignes log: %d\n", NR
        
        # Estimation du temps de jeu (approximatif)
        if (total_coups > 0) {
            temps_estime = total_coups * 30  # 30 secondes par coup
            heures = int(temps_estime / 3600)
            minutes = int((temps_estime % 3600) / 60)
            printf "   Temps de jeu estimé: %dh%02dm\n", heures, minutes
        }
    }
    
    print ""
    print "================================================"
    print "      FIN DU RAPPORT - " strftime("%d/%m/%Y %H:%M:%S")
    print "================================================"
}
