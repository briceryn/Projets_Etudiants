package clientlib;

/**
 * Exception levée en cas d'échec d'authentification.
 */
public class AuthException extends Exception {
    public AuthException(String message) {
        super(message);
    }
}