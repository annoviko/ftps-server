#ifndef TLS_SESSION_H_
#define TLS_SESSION_H_


#include <gnutls/gnutls.h>
#include <gnutls/gnutlsxx.h>


#define OPERATION_SUCCESS       0
#define OPERATION_FAILURE       -1


class tls_session {
private:
    const static char *       TLS_SESSION_CERTIFICATE;
    const static char *       TLS_SESSION_PRIVATE_KEY;
    static bool               TLS_GLOBAL_STATE_INITIALIZED;

private:
    gnutls_dh_params_t                  m_dh_parameters;
    gnutls_certificate_credentials_t    m_credentials;
    gnutls_session_t                    m_session;

public:
    tls_session(void);

    tls_session(const tls_session & other_session) = delete;

    tls_session(tls_session && other_session);

    virtual ~tls_session(void);

public:
    virtual int bind(const int socket_descriptor);

    virtual int handshake(void);

    virtual ssize_t push(const char * byte_sequence, const size_t sequence_length) const;

    virtual ssize_t pull(const size_t receive_buffer_length, void * receive_buffer) const;

    virtual void close(void);

private:
    int create_tls_session(const int socket_descriptor);

    int initialize_credentials(void);

    int initialize_dh_parameters(void);

    int initialize_session(const int socket_descriptor);

    void free_tls_session(void);

private:
    static void initialize_tls_global_state(void);

public:
    tls_session & operator=(const tls_session & other_session) = delete;

    tls_session & operator=(tls_session && other_session);
};


#endif
