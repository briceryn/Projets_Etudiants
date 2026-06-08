# Makefile pour 6 qui prend
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -lpthread

# Fichiers source
SRCS = jeu.c joueurs.c communication.c gestionnaire.c main.c
OBJS = $(SRCS:.c=.o)
HEADERS = jeu.h joueurs.h communication.h gestionnaire.h

# Exécutable principal
EXEC = 6quiprend

# Exécutables de test
TEST_JEU = test_jeu
TEST_JOUEURS = test_joueurs
TEST_COMM = test_comm
TEST_GESTIONNAIRE = test_gestionnaire

# Cible par défaut
all: $(EXEC)

# Programme principal
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Test du module jeu
$(TEST_JEU): jeu.o
	$(CC) $(CFLAGS) $< -o $@ -D TEST_JEU

# Test des joueurs
$(TEST_JOUEURS): jeu.o joueurs.o
	$(CC) $(CFLAGS) $^ -o $@ -D TEST_JOUEURS

# Test des communications
$(TEST_COMM): communication.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -D TEST_COMM

# Test du gestionnaire
$(TEST_GESTIONNAIRE): jeu.o communication.o gestionnaire.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -D TEST_GESTIONNAIRE

# Règles de compilation
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -f $(OBJS) $(EXEC) $(TEST_JEU) $(TEST_JOUEURS) $(TEST_COMM) $(TEST_GESTIONNAIRE)

# Tester tout
test: $(TEST_JEU) $(TEST_JOUEURS) $(TEST_COMM) $(TEST_GESTIONNAIRE)
	@echo "=== TEST DU MODULE JEU ==="
	./$(TEST_JEU)
	@echo "\n=== TEST DES JOUEURS ==="
	./$(TEST_JOUEURS)
	@echo "\n=== TEST DES COMMUNICATIONS ==="
	./$(TEST_COMM) --test
	@echo "\n=== TEST DU GESTIONNAIRE ==="
	./$(TEST_GESTIONNAIRE)

# Démo
demo: $(EXEC)
	./$(EXEC) --demo

# Serveur
serveur: $(EXEC)
	./$(EXEC) --serveur 8080

# Client
client: $(EXEC)
	./$(EXEC) --client localhost 8080

# Aide sur les cibles
help:
	@echo "Cibles disponibles:"
	@echo "  all        Compile tout (par défaut)"
	@echo "  clean      Nettoie les fichiers compilés"
	@echo "  test       Lance tous les tests"
	@echo "  demo       Lance la démonstration"
	@echo "  serveur    Lance le serveur sur le port 8080"
	@echo "  client     Lance un client qui se connecte à localhost:8080"
	@echo "  help       Affiche cette aide"

.PHONY: all clean test demo serveur client help
