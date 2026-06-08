#!/bin/bash
# Script d'installation pour le projet 6 qui prend

echo "=== Installation du projet 6 qui prend ==="

# 1. Création des dossiers
echo "1. Création des dossiers..."
mkdir -p logs
mkdir -p stats
mkdir -p scripts

echo "✓ Dossiers créés: logs/, stats/, scripts/"

# 2. Copie des scripts
echo "2. Configuration des scripts..."

# Vérifier si stats.awk existe
if [ ! -f "stats.awk" ]; then
    echo "Création du script stats.awk..."
    # Le script awk sera copié ici
    echo "Veuillez copier le script awk dans stats.awk"
else
    echo "✓ Script awk trouvé"
fi

# Donner les permissions d'exécution
chmod +x scripts/*.sh 2>/dev/null
chmod +x stats.awk 2>/dev/null

# 3. Vérification des dépendances
echo ""
echo "3. Vérification des dépendances..."

# Vérifier awk
if command -v awk &> /dev/null; then
    awk_version=$(awk --version | head -n1)
    echo "✓ AWK: $awk_version"
else
    echo "✗ AWK non installé. Installation requise."
    echo "  Ubuntu/Debian: sudo apt-get install gawk"
    echo "  Fedora: sudo dnf install gawk"
fi

# Vérifier pdflatex (optionnel)
if command -v pdflatex &> /dev/null; then
    echo "✓ LaTeX: $(pdflatex --version | head -n1)"
else
    echo "⚠ LaTeX non installé (optionnel pour PDF)"
fi

# Vérifier gcc
if command -v gcc &> /dev/null; then
    gcc_version=$(gcc --version | head -n1)
    echo "✓ GCC: $gcc_version"
else
    echo "✗ GCC non installé. Installation requise pour compiler le jeu."
fi

# 4. Création d'un fichier log exemple
echo ""
echo "4. Création d'un fichier log exemple..."
if [ ! -f "logs/parties.log" ]; then
    cat > logs/parties.log << 'EOF'
[2025-11-15 14:30:00] Partie #1 démarrée
[2025-11-15 14:30:01] Joueur Alice ajouté à la partie #1
[2025-11-15 14:30:02] Joueur Bob ajouté à la partie #1
[2025-11-15 14:30:03] Joueur Charlie ajouté à la partie #1
[2025-11-15 14:35:00] Alice joue la carte [15 (2 têtes)]
[2025-11-15 14:35:01] Bob joue la carte [23 (1 tête)]
[2025-11-15 14:35:02] Charlie joue la carte [55 (7 têtes)]
[2025-11-15 14:40:00] Partie #1 terminée
[2025-11-15 14:40:01] Scores finaux: Alice=15, Bob=23, Charlie=55
[2025-11-15 14:40:02] Gagnant: Bob (score le plus bas)
[2025-11-15 15:00:00] Partie #2 démarrée
[2025-11-15 15:00:01] Joueur Alice ajouté à la partie #2
[2025-11-15 15:00:02] Joueur Bob ajouté à la partie #2
[2025-11-15 15:30:00] Partie #2 terminée
[2025-11-15 15:30:01] Scores finaux: Alice=42, Bob=66
[2025-11-15 15:30:02] Gagnant: Alice
EOF
    echo "✓ Fichier log exemple créé: logs/parties.log"
else
    echo "✓ Fichier log existant"
fi

# 5. Instructions finales
echo ""
echo "=== INSTALLATION TERMINÉE ==="
echo ""
echo "Pour tester les statistiques:"
echo "  awk -f stats.awk logs/parties.log"
echo ""
echo "Pour générer un PDF (si LaTeX installé):"
echo "  ./scripts/generer_stats.sh"
echo ""
echo "Structure du projet:"
echo "  logs/          - Fichiers de log des parties"
echo "  stats/         - Statistiques générées (texte + PDF)"
echo "  scripts/       - Scripts utilitaires"
echo "  stats.awk      - Script principal de statistiques"
echo ""
echo "Bon développement !"
