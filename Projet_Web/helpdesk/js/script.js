// Attendre que le DOM soit prêt (CM2.pdf p.8)
$(document).ready(function() {
    
    // ============================================
    // ANIMATIONS GLOBALES (CM2.pdf p.22-23)
    // ============================================
    
    // Animation d'entrée du contenu principal
    $('main').hide().fadeIn(800);
    
    // Animation des messages d'erreur/succès
    $('.error-message, .success-message').each(function(index) {
        $(this).hide().slideDown(500).delay(3000).fadeOut(500);
    });
    
    
    // ============================================
    // PAGE CONNEXION (CM2.pdf p.16-18)
    // ============================================
    
    // Validation en temps réel du formulaire de connexion
    if ($('#login-form').length) {
        
        // Validation du nom d'utilisateur
        $('#username').on('input', function() {
            var value = $(this).val();
            if (value.length < 3 && value.length > 0) {
                $(this).addClass('error');
                $('#username-error').text('Minimum 3 caractères').fadeIn();
            } else {
                $(this).removeClass('error');
                $('#username-error').fadeOut();
            }
        });
        
        // Animation du bouton
        $('#login-form button').hover(
            function() { $(this).animate({padding: '12px 24px'}, 200); },
            function() { $(this).animate({padding: '10px 20px'}, 200); }
        );
    }
    
    
    // ============================================
    // PAGE TABLEAU DE BORD (CM2.pdf p.11-15)
    // ============================================
    
    // Filtrage dynamique des tickets
    if ($('#tickets-table').length) {
        
        // Filtre par statut
        $('#filter-statut').on('change', filterTickets);
        
        // Filtre par priorité
        $('#filter-priorite').on('change', filterTickets);
        
        // Recherche textuelle
        $('#search-tickets').on('keyup', filterTickets);
        
        function filterTickets() {
            var searchText = $('#search-tickets').val().toLowerCase();
            var statut = $('#filter-statut').val();
            var priorite = $('#filter-priorite').val();
            
            $('.ticket-row').each(function() {
                var $row = $(this);
                var show = true;
                
                // Filtre recherche
                if (searchText) {
                    var titre = $row.data('titre').toLowerCase();
                    if (titre.indexOf(searchText) === -1) {
                        show = false;
                    }
                }
                
                // Filtre statut
                if (statut !== 'tous' && show) {
                    if ($row.data('statut') !== statut) {
                        show = false;
                    }
                }
                
                // Filtre priorité
                if (priorite !== 'tous' && show) {
                    if ($row.data('priorite') !== priorite) {
                        show = false;
                    }
                }
                
                // Animation d'affichage/masquage (CM2.pdf p.22)
                if (show) {
                    $row.fadeIn(300);
                } else {
                    $row.fadeOut(300);
                }
            });
        }
        
        // Animation des statistiques
        $('.stat-card').each(function(index) {
            $(this).hide().delay(index * 200).fadeIn(500);
        });
        
        // Tooltips sur les actions (CM2.pdf p.25)
        $('.btn-view').hover(
            function() {
                $(this).append('<span class="tooltip">Voir le détail</span>');
                $(this).find('.tooltip').fadeIn(200);
            },
            function() {
                $(this).find('.tooltip').fadeOut(200, function() { $(this).remove(); });
            }
        );
    }
    
    
    // ============================================
    // PAGE CRÉATION TICKET (CM2.pdf p.16,21-22)
    // ============================================
    
    if ($('#ticket-form').length) {
        
        // Compteurs de caractères
        $('#titre').on('input', function() {
            var length = $(this).val().length;
            $('#titre-counter').text(length + '/50');
            if (length > 45) {
                $('#titre-counter').css('color', 'orange');
            } else if (length > 50) {
                $(this).val($(this).val().substring(0, 50));
            } else {
                $('#titre-counter').css('color', '#666');
            }
        });
        
        $('#description').on('input', function() {
            var length = $(this).val().length;
            $('#description-counter').text(length + '/500');
            if (length > 450) {
                $('#description-counter').css('color', 'orange');
            } else if (length > 500) {
                $(this).val($(this).val().substring(0, 500));
            } else {
                $('#description-counter').css('color', '#666');
            }
        });
        
        // Validation en temps réel
        $('#titre').on('blur', function() {
            var value = $(this).val();
            if (value.length < 3 && value.length > 0) {
                $(this).addClass('error');
                if ($(this).next('.error-feedback').length === 0) {
                    $(this).after('<div class="error-feedback">Minimum 3 caractères</div>');
                }
            } else {
                $(this).removeClass('error');
                $(this).next('.error-feedback').remove();
            }
        });
        
        // Animation des champs
        $('.form-group').each(function(index) {
            $(this).css('opacity', '0').delay(index * 100).animate({opacity: 1}, 500);
        });
        
        // Animation de la priorité (CM2.pdf p.22)
        $('#priorite').on('change', function() {
            var color;
            switch($(this).val()) {
                case 'Haute': color = '#dc3545'; break;
                case 'Moyenne': color = '#ffc107'; break;
                case 'Basse': color = '#28a745'; break;
                default: color = '#666';
            }
            $(this).animate({backgroundColor: color}, 300, function() {
                $(this).animate({backgroundColor: '#fff'}, 300);
            });
        });
    }
    
    
    // ============================================
    // PAGE DÉTAIL TICKET (CM2.pdf p.16,25-27)
    // ============================================
    
    if ($('.ticket-detail').length) {
        
        // Animation des commentaires
        $('.comment').hide().each(function(index) {
            $(this).delay(index * 200).fadeIn(500);
        });
        
        // Compteur pour commentaire
        $('#commentaire').on('input', function() {
            var length = $(this).val().length;
            $('#comment-counter').text(length + '/500');
            if (length > 450) {
                $('#comment-counter').css('color', 'orange');
            } else if (length > 500) {
                $(this).val($(this).val().substring(0, 500));
            } else {
                $('#comment-counter').css('color', '#666');
            }
        });
        
        // Ajout de commentaire en AJAX (CM2.pdf p.4,16)
        $('#comment-form').on('submit', function(e) {
            // Ne pas empêcher la soumission normale, mais ajouter une animation
            var $btn = $(this).find('button');
            $btn.html('⏳ Envoi...').prop('disabled', true);
            
            // Animation de chargement
            $btn.animate({opacity: 0.7}, 300);
        });
        
        // Mise à jour du statut (tuteur)
        if ($('#update-statut-form').length) {
            
            // Aperçu du changement avant soumission
            $('#statut-select').on('change', function() {
                var nouveauStatut = $(this).find('option:selected').text();
                var $badge = $('.statut-badge');
                
                $badge.fadeOut(200, function() {
                    $(this).text(nouveauStatut).fadeIn(200);
                });
            });
            
            // Navigation dans le DOM pour les commentaires (CM2.pdf p.27)
            $('.comment').hover(
                function() {
                    $(this).find('.comment-date').css('color', '#007bff');
                },
                function() {
                    $(this).find('.comment-date').css('color', '#666');
                }
            );
            
            // Supprimer le parent (exemple de navigation DOM)
            $('.comment').dblclick(function() {
                if (confirm('Supprimer ce commentaire ?')) {
                    $(this).slideUp(500, function() { $(this).remove(); });
                }
            });
        }
    }
    
    
    // ============================================
    // ANIMATIONS DIVERSES (CM2.pdf p.22-23)
    // ============================================
    
    // Animation des boutons
    $('button, .btn-primary, .btn-secondary, .btn-view').hover(
        function() {
            $(this).animate({opacity: 0.8}, 200);
        },
        function() {
            $(this).animate({opacity: 1}, 200);
        }
    );
    
    // Animation de survol des lignes de tableau
    $('tr').hover(
        function() { $(this).css('backgroundColor', '#f5f5f5'); },
        function() { $(this).css('backgroundColor', ''); }
    );
    
    // Scroll fluide vers les ancres
    $('a[href^="#"]').on('click', function(e) {
        e.preventDefault();
        var target = $(this.hash);
        if (target.length) {
            $('html, body').animate({
                scrollTop: target.offset().top - 100
            }, 500);
        }
    });
    
});