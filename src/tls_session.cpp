#include "tls_session.h"

#include <stdexcept>
#include <iostream>


const char * tls_session::TLS_SESSION_CERTIFICATE = "server.cert";
const char * tls_session::TLS_SESSION_PRIVATE_KEY = "server.key";

bool tls_session::TLS_GLOBAL_STATE_INITIALIZED = false;


tls_session::tls_session(void) :
        m_dh_parameters(NULL),
        m_credentials(NULL),
        m_session(NULL)
{
    initialize_tls_global_state();
}


tls_session::tls_session(tls_session && other_session) {
    m_dh_parameters = other_session.m_dh_parameters;
    m_credentials = other_session.m_credentials;
    m_session = other_session.m_session;

    other_session.m_dh_parameters = NULL;
    other_session.m_credentials = NULL;
    other_session.m_session = NULL;
}


tls_session::~tls_session(void) {
    std::cout << "TLS desctuctor" << std::endl;
    free_tls_session();
}


int tls_session::bind(const int socket_descriptor) {
    return create_tls_session(socket_descriptor);
}


int tls_session::handshake(void) {
    int tls_handshake_result = 0;
    do {
        tls_handshake_result = gnutls_handshake(m_session);
    }
    while ( (tls_handshake_result < 0) && (gnutls_error_is_fatal(tls_handshake_result) == 0) );

    if (tls_handshake_result < 0) {
        gnutls_deinit(m_session);
        m_session = NULL;

        std::cout << "TLS session: Warning: TLS handshake is not performed!" << std::endl;

        return false;
    }

    return true;
}


size_t tls_session::push(const char * byte_sequence, const size_t sequence_length) const {
    return gnutls_record_send(m_session, byte_sequence, sequence_length);
}


size_t tls_session::pull(const size_t receive_buffer_length, void * receive_buffer) const {
    return gnutls_record_recv(m_session, receive_buffer, receive_buffer_length);
}


void tls_session::close(void) {
    if (m_session != NULL) {
        gnutls_bye(m_session, GNUTLS_SHUT_WR);
    }

    free_tls_session();
}


int tls_session::create_tls_session(const int socket_descriptor) {
    if ( (initialize_credentials() == OPERATION_SUCCESS) &&
         (initialize_dh_parameters() == OPERATION_SUCCESS) &&
         (initialize_session(socket_descriptor) == OPERATION_SUCCESS) ) {

        return OPERATION_SUCCESS;
    }

    return OPERATION_FAILURE;
}


int tls_session::initialize_credentials(void) {
    std::cout << "TLS session: TLS session creating (DH, credential, session)" << std::endl;
    gnutls_certificate_allocate_credentials(&m_credentials);

    if (gnutls_certificate_set_x509_key_file(m_credentials, TLS_SESSION_CERTIFICATE, TLS_SESSION_PRIVATE_KEY, GNUTLS_X509_FMT_PEM) != GNUTLS_E_SUCCESS) {
        return OPERATION_FAILURE;
    }

    return OPERATION_SUCCESS;
}


int tls_session::initialize_dh_parameters(void) {
    unsigned int bits = gnutls_sec_param_to_pk_bits(GNUTLS_PK_DH, GNUTLS_SEC_PARAM_NORMAL);

    gnutls_dh_params_init(&m_dh_parameters);

    gnutls_dh_params_generate2(m_dh_parameters, bits);

    gnutls_certificate_set_dh_params(m_credentials, m_dh_parameters);

    return OPERATION_SUCCESS;
}


int tls_session::initialize_session(const int socket_descriptor) {
    gnutls_init(&m_session, GNUTLS_SERVER);

    gnutls_priority_t priority;
    gnutls_priority_init(&priority, "NORMAL", NULL);

    gnutls_priority_set(m_session, priority);

    gnutls_credentials_set(m_session, GNUTLS_CRD_CERTIFICATE, m_credentials);

    gnutls_certificate_server_set_request(m_session, GNUTLS_CERT_IGNORE);

    gnutls_transport_set_ptr(m_session, (gnutls_transport_ptr_t) socket_descriptor);

    return OPERATION_SUCCESS;
}


void tls_session::free_tls_session(void) {
    std::cout << "TLS session: free DH, credential, TLS session" << std::endl;

    if (m_session != NULL) {
        gnutls_deinit(m_session);
    }

    if (m_dh_parameters != NULL) {
        gnutls_dh_params_deinit(m_dh_parameters);
    }

    if (m_credentials != NULL) {
        gnutls_certificate_free_credentials(m_credentials);
    }

    m_dh_parameters = NULL;
    m_credentials = NULL;
    m_session = NULL;
}


void tls_session::initialize_tls_global_state(void) {
    if (!TLS_GLOBAL_STATE_INITIALIZED) {
        std::cout << "TLS session: gnutls_global_init() GNU TLS initialization" << std::endl;

        gnutls_global_init();

        TLS_GLOBAL_STATE_INITIALIZED = true;
    }
}


tls_session & tls_session::operator=(tls_session && other_session) {
    if (this != &other_session) {
        m_dh_parameters = other_session.m_dh_parameters;
        m_credentials = other_session.m_credentials;
        m_session = other_session.m_session;

        other_session.m_dh_parameters = NULL;
        other_session.m_credentials = NULL;
        other_session.m_session = NULL;
    }

    return *this;
}
