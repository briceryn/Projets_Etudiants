package clientlib;

/**
 * Exception générale pour les erreurs de la base de données.
 */
public class DBException extends Exception {
    public DBException(String message) {
        super(message);
    }

    public DBException(String message, Throwable cause) {
        super(message, cause);
    }
}