/*

Copyright 2018 Intel Corporation

This software and the related documents are Intel copyrighted materials,
and your use of them is governed by the express license under which they
were provided to you (License). Unless the License provides otherwise,
you may not use, modify, copy, publish, distribute, disclose or transmit
this software or the related documents without Intel's prior written
permission.

This software and the related documents are provided as is, with no
express or implied warranties, other than those that are expressly stated
in the License.

*/


#ifdef _WIN32
# include <Windows.h>
#else
# include <dlfcn.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include "sgx_stub.h"
#include <sgx_edger8r.h>
#include <sgx_uae_service.h>
#include <sgx_urts.h>
typedef void* (*func)();

static const char *dlerr= NULL;

static void _undefined_symbol (const char *symbol);
static void *_load_symbol(void *handle, const char *symbol, int *status);

#include <sys/types.h>
#include <inttypes.h>

int close_session_ocall(uint32_t sid, uint32_t timeout);
int create_session_ocall(uint32_t* sid, uint8_t* dh_msg1, uint32_t dh_msg1_size, uint32_t timeout);
int exchange_report_ocall(uint32_t sid, uint8_t* dh_msg2, uint32_t dh_msg2_size, uint8_t* dh_msg3, uint32_t dh_msg3_size, uint32_t timeout);
int invoke_service_ocall(uint8_t* pse_message_req, uint32_t pse_message_req_size, uint8_t* pse_message_resp, uint32_t pse_message_resp_size, uint32_t timeout);
int invoke_service_ocall(uint8_t* pse_message_req, uint32_t pse_message_req_size, uint8_t* pse_message_resp, uint32_t pse_message_resp_size, uint32_t timeout);

int sgx_thread_set_multiple_untrusted_events_ocall(const void **waiters, size_t total);
int sgx_thread_set_untrusted_event_ocall(const void *waiter);
int sgx_thread_setwait_untrusted_events_ocall(const void *waiter, const void *self);
int sgx_thread_wait_untrusted_event_ocall(const void *self);

void sgx_oc_cpuidex(int cpuinfo[4], int leaf, int subleaf);

;

static void *_load_libsgx_uae_service();
static void *_load_libsgx_urts();

static void *h_libsgx_uae_service= NULL;
static void *h_libsgx_urts= NULL;
static int l_libsgx_uae_service= 0;
static int l_libsgx_urts= 0;

static func p_sgx_thread_set_untrusted_event_ocall= NULL;
static func p_exchange_report_ocall= NULL;
static func p_sgx_ocall= NULL;
static func p_sgx_get_whitelist_size= NULL;
static func p_sgx_create_enclave= NULL;
static func p_sgx_calc_quote_size= NULL;
static func p_sgx_create_encrypted_enclave= NULL;
static func p_sgx_register_wl_cert_chain= NULL;
static func p_sgx_get_ps_cap= NULL;
static func p_sgx_get_quote_size= NULL;
static func p_create_session_ocall= NULL;
static func p_sgx_get_extended_epid_group_id= NULL;
static func p_sgx_thread_wait_untrusted_event_ocall= NULL;
static func p_sgx_oc_cpuidex= NULL;
static func p_sgx_init_quote= NULL;
static func p_sgx_get_whitelist= NULL;
static func p_close_session_ocall= NULL;
static func p_sgx_destroy_enclave= NULL;
static func p_sgx_ecall= NULL;
static func p_sgx_thread_setwait_untrusted_events_ocall= NULL;
static func p_sgx_report_attestation_status= NULL;
static func p_sgx_thread_set_multiple_untrusted_events_ocall= NULL;
static func p_invoke_service_ocall= NULL;
static func p_sgx_get_quote= NULL;

static int l_sgx_thread_set_untrusted_event_ocall= 0;
static int l_exchange_report_ocall= 0;
static int l_sgx_ocall= 0;
static int l_sgx_get_whitelist_size= 0;
static int l_sgx_create_enclave= 0;
static int l_sgx_calc_quote_size= 0;
static int l_sgx_create_encrypted_enclave= 0;
static int l_sgx_register_wl_cert_chain= 0;
static int l_sgx_get_ps_cap= 0;
static int l_sgx_get_quote_size= 0;
static int l_create_session_ocall= 0;
static int l_sgx_get_extended_epid_group_id= 0;
static int l_sgx_thread_wait_untrusted_event_ocall= 0;
static int l_sgx_oc_cpuidex= 0;
static int l_sgx_init_quote= 0;
static int l_sgx_get_whitelist= 0;
static int l_close_session_ocall= 0;
static int l_sgx_destroy_enclave= 0;
static int l_sgx_ecall= 0;
static int l_sgx_thread_setwait_untrusted_events_ocall= 0;
static int l_sgx_report_attestation_status= 0;
static int l_sgx_thread_set_multiple_untrusted_events_ocall= 0;
static int l_invoke_service_ocall= 0;
static int l_sgx_get_quote= 0;

static void _undefined_symbol (const char *symbol)
{
	fprintf(stderr, "%s: %s\n", symbol, dlerr);
	exit(1);
}

static void *_load_symbol(void *handle, const char *symbol, int *status)
{
	void *hsym;

#ifdef _WIN32
	hsym = GetProcAddress((HMODULE)handle, symbol);
	*status = (dlerr == NULL) ? 1 : -1;
#else
	dlerr= dlerror();
	hsym= dlsym(handle, symbol);
	dlerr= dlerror();
	*status= ( dlerr == NULL ) ? 1 : -1;
#endif

	return hsym;
}


static void *_load_libsgx_uae_service()
{
	if ( l_libsgx_uae_service == 0 ) {
#ifdef _WIN32
		h_libsgx_uae_service= LoadLibrary("libsgx_uae_service.dll");
#else
		h_libsgx_uae_service= dlopen("libsgx_uae_service.so", RTLD_GLOBAL|RTLD_NOW);
#endif
		l_libsgx_uae_service= ( h_libsgx_uae_service == NULL ) ? -1 : 1;
	}

	return h_libsgx_uae_service;
}

static void *_load_libsgx_urts()
{
	if ( l_libsgx_urts == 0 ) {
#ifdef _WIN32
		h_libsgx_urts= LoadLibrary("libsgx_urts.dll");
#else
		h_libsgx_urts= dlopen("libsgx_urts.so", RTLD_GLOBAL|RTLD_NOW);
#endif
		l_libsgx_urts= ( h_libsgx_urts == NULL ) ? -1 : 1;
	}

	return h_libsgx_urts;
}

int have_sgx_psw()
{
	return ( 
		_load_libsgx_uae_service() == NULL ||
		_load_libsgx_urts() == NULL 
	) ? 0 : 1;
}

int have_sgx_ufunction(const char *name)
{
	int status = 0;
	_load_symbol(h_libsgx_uae_service, name, &status);
	if ( status == 1 ) return 1;
	_load_symbol(h_libsgx_urts, name, &status);
	return (status == 1) ? 1 : 0;
}

void *get_sgx_ufunction(const char *name)
{
	void *hsym= NULL;
	int status = 0;

	hsym= _load_symbol(h_libsgx_uae_service, name, &status);
	if ( status == 1 ) return hsym;

	_load_symbol(h_libsgx_urts, name, &status);
	return (status == 1) ? hsym : NULL;
}


int sgx_thread_set_untrusted_event_ocall(const void *waiter)
{
	if ( l_sgx_thread_set_untrusted_event_ocall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_thread_set_untrusted_event_ocall)= _load_symbol(h_libsgx_urts, "sgx_thread_set_untrusted_event_ocall", &l_sgx_thread_set_untrusted_event_ocall);
	}

	if ( l_sgx_thread_set_untrusted_event_ocall == -1 ) _undefined_symbol("sgx_thread_set_untrusted_event_ocall");

	return (int) p_sgx_thread_set_untrusted_event_ocall(waiter);
}

int exchange_report_ocall(uint32_t sid, uint8_t *dh_msg2, uint32_t dh_msg2_size, uint8_t *dh_msg3, uint32_t dh_msg3_size, uint32_t timeout)
{
	if ( l_exchange_report_ocall == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_exchange_report_ocall)= _load_symbol(h_libsgx_uae_service, "exchange_report_ocall", &l_exchange_report_ocall);
	}

	if ( l_exchange_report_ocall == -1 ) _undefined_symbol("exchange_report_ocall");

	return (int) p_exchange_report_ocall(sid, dh_msg2, dh_msg2_size, dh_msg3, dh_msg3_size, timeout);
}

sgx_status_t sgx_ocall(const unsigned int index, void *ms)
{
	if ( l_sgx_ocall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_ocall)= _load_symbol(h_libsgx_urts, "sgx_ocall", &l_sgx_ocall);
	}

	if ( l_sgx_ocall == -1 ) _undefined_symbol("sgx_ocall");

	return (sgx_status_t) p_sgx_ocall(index, ms);
}

sgx_status_t sgx_get_whitelist_size(uint32_t *p_whitelist_size)
{
	if ( l_sgx_get_whitelist_size == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_whitelist_size)= _load_symbol(h_libsgx_uae_service, "sgx_get_whitelist_size", &l_sgx_get_whitelist_size);
	}

	if ( l_sgx_get_whitelist_size == -1 ) _undefined_symbol("sgx_get_whitelist_size");

	return (sgx_status_t) p_sgx_get_whitelist_size(p_whitelist_size);
}

sgx_status_t sgx_create_enclave(const char *file_name, const int debug, sgx_launch_token_t *launch_token, int *launch_token_updated, sgx_enclave_id_t *enclave_id, sgx_misc_attribute_t *misc_attr)
{
	if ( l_sgx_create_enclave == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_create_enclave)= _load_symbol(h_libsgx_urts, "sgx_create_enclave", &l_sgx_create_enclave);
	}

	if ( l_sgx_create_enclave == -1 ) _undefined_symbol("sgx_create_enclave");

	return (sgx_status_t) p_sgx_create_enclave(file_name, debug, launch_token, launch_token_updated, enclave_id, misc_attr);
}

sgx_status_t sgx_calc_quote_size(const uint8_t *p_sig_rl, uint32_t sig_rl_size, uint32_t *p_quote_size)
{
	if ( l_sgx_calc_quote_size == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_calc_quote_size)= _load_symbol(h_libsgx_uae_service, "sgx_calc_quote_size", &l_sgx_calc_quote_size);
	}

	if ( l_sgx_calc_quote_size == -1 ) _undefined_symbol("sgx_calc_quote_size");

	return (sgx_status_t) p_sgx_calc_quote_size(p_sig_rl, sig_rl_size, p_quote_size);
}

sgx_status_t sgx_create_encrypted_enclave(const char *file_name, const int debug, sgx_launch_token_t *launch_token, int *launch_token_updated, sgx_enclave_id_t *enclave_id, sgx_misc_attribute_t *misc_attr, uint8_t *sealed_key)
{
	if ( l_sgx_create_encrypted_enclave == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_create_encrypted_enclave)= _load_symbol(h_libsgx_urts, "sgx_create_encrypted_enclave", &l_sgx_create_encrypted_enclave);
	}

	if ( l_sgx_create_encrypted_enclave == -1 ) _undefined_symbol("sgx_create_encrypted_enclave");

	return (sgx_status_t) p_sgx_create_encrypted_enclave(file_name, debug, launch_token, launch_token_updated, enclave_id, misc_attr, sealed_key);
}

sgx_status_t sgx_register_wl_cert_chain(uint8_t *p_wl_cert_chain, uint32_t wl_cert_chain_size)
{
	if ( l_sgx_register_wl_cert_chain == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_register_wl_cert_chain)= _load_symbol(h_libsgx_uae_service, "sgx_register_wl_cert_chain", &l_sgx_register_wl_cert_chain);
	}

	if ( l_sgx_register_wl_cert_chain == -1 ) _undefined_symbol("sgx_register_wl_cert_chain");

	return (sgx_status_t) p_sgx_register_wl_cert_chain(p_wl_cert_chain, wl_cert_chain_size);
}

sgx_status_t sgx_get_ps_cap(sgx_ps_cap_t *p_sgx_ps_cap)
{
	if ( l_sgx_get_ps_cap == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_ps_cap)= _load_symbol(h_libsgx_uae_service, "sgx_get_ps_cap", &l_sgx_get_ps_cap);
	}

	if ( l_sgx_get_ps_cap == -1 ) _undefined_symbol("sgx_get_ps_cap");

	return (sgx_status_t) p_sgx_get_ps_cap(p_sgx_ps_cap);
}

sgx_status_t sgx_get_quote_size(const uint8_t *p_sig_rl, uint32_t *p_quote_size)
{
	if ( l_sgx_get_quote_size == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_quote_size)= _load_symbol(h_libsgx_uae_service, "sgx_get_quote_size", &l_sgx_get_quote_size);
	}

	if ( l_sgx_get_quote_size == -1 ) _undefined_symbol("sgx_get_quote_size");

	return (sgx_status_t) p_sgx_get_quote_size(p_sig_rl, p_quote_size);
}

int create_session_ocall(uint32_t *sid, uint8_t *dh_msg1, uint32_t dh_msg1_size, uint32_t timeout)
{
	if ( l_create_session_ocall == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_create_session_ocall)= _load_symbol(h_libsgx_uae_service, "create_session_ocall", &l_create_session_ocall);
	}

	if ( l_create_session_ocall == -1 ) _undefined_symbol("create_session_ocall");

	return (int) p_create_session_ocall(sid, dh_msg1, dh_msg1_size, timeout);
}

sgx_status_t sgx_get_extended_epid_group_id(uint32_t *p_extended_epid_group_id)
{
	if ( l_sgx_get_extended_epid_group_id == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_extended_epid_group_id)= _load_symbol(h_libsgx_uae_service, "sgx_get_extended_epid_group_id", &l_sgx_get_extended_epid_group_id);
	}

	if ( l_sgx_get_extended_epid_group_id == -1 ) _undefined_symbol("sgx_get_extended_epid_group_id");

	return (sgx_status_t) p_sgx_get_extended_epid_group_id(p_extended_epid_group_id);
}

int sgx_thread_wait_untrusted_event_ocall(const void *self)
{
	if ( l_sgx_thread_wait_untrusted_event_ocall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_thread_wait_untrusted_event_ocall)= _load_symbol(h_libsgx_urts, "sgx_thread_wait_untrusted_event_ocall", &l_sgx_thread_wait_untrusted_event_ocall);
	}

	if ( l_sgx_thread_wait_untrusted_event_ocall == -1 ) _undefined_symbol("sgx_thread_wait_untrusted_event_ocall");

	return (int) p_sgx_thread_wait_untrusted_event_ocall(self);
}

void sgx_oc_cpuidex(int *cpuinfo, int leaf, int subleaf)
{
	if ( l_sgx_oc_cpuidex == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_oc_cpuidex)= _load_symbol(h_libsgx_urts, "sgx_oc_cpuidex", &l_sgx_oc_cpuidex);
	}

	if ( l_sgx_oc_cpuidex == -1 ) _undefined_symbol("sgx_oc_cpuidex");

	return (void) p_sgx_oc_cpuidex(cpuinfo, leaf, subleaf);
}

sgx_status_t sgx_init_quote(sgx_target_info_t *p_target_info, sgx_epid_group_id_t *p_gid)
{
	if ( l_sgx_init_quote == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_init_quote)= _load_symbol(h_libsgx_uae_service, "sgx_init_quote", &l_sgx_init_quote);
	}

	if ( l_sgx_init_quote == -1 ) _undefined_symbol("sgx_init_quote");

	return (sgx_status_t) p_sgx_init_quote(p_target_info, p_gid);
}

sgx_status_t sgx_get_whitelist(uint8_t *p_whitelist, uint32_t whitelist_size)
{
	if ( l_sgx_get_whitelist == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_whitelist)= _load_symbol(h_libsgx_uae_service, "sgx_get_whitelist", &l_sgx_get_whitelist);
	}

	if ( l_sgx_get_whitelist == -1 ) _undefined_symbol("sgx_get_whitelist");

	return (sgx_status_t) p_sgx_get_whitelist(p_whitelist, whitelist_size);
}

int close_session_ocall(uint32_t sid, uint32_t timeout)
{
	if ( l_close_session_ocall == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_close_session_ocall)= _load_symbol(h_libsgx_uae_service, "close_session_ocall", &l_close_session_ocall);
	}

	if ( l_close_session_ocall == -1 ) _undefined_symbol("close_session_ocall");

	return (int) p_close_session_ocall(sid, timeout);
}

sgx_status_t sgx_destroy_enclave(const sgx_enclave_id_t enclave_id)
{
	if ( l_sgx_destroy_enclave == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_destroy_enclave)= _load_symbol(h_libsgx_urts, "sgx_destroy_enclave", &l_sgx_destroy_enclave);
	}

	if ( l_sgx_destroy_enclave == -1 ) _undefined_symbol("sgx_destroy_enclave");

	return (sgx_status_t) p_sgx_destroy_enclave(enclave_id);
}

sgx_status_t sgx_ecall(const sgx_enclave_id_t eid, const int index, const void *ocall_table, void *ms)
{
	if ( l_sgx_ecall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_ecall)= _load_symbol(h_libsgx_urts, "sgx_ecall", &l_sgx_ecall);
	}

	if ( l_sgx_ecall == -1 ) _undefined_symbol("sgx_ecall");

	return (sgx_status_t) p_sgx_ecall(eid, index, ocall_table, ms);
}

int sgx_thread_setwait_untrusted_events_ocall(const void *waiter, const void *self)
{
	if ( l_sgx_thread_setwait_untrusted_events_ocall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_thread_setwait_untrusted_events_ocall)= _load_symbol(h_libsgx_urts, "sgx_thread_setwait_untrusted_events_ocall", &l_sgx_thread_setwait_untrusted_events_ocall);
	}

	if ( l_sgx_thread_setwait_untrusted_events_ocall == -1 ) _undefined_symbol("sgx_thread_setwait_untrusted_events_ocall");

	return (int) p_sgx_thread_setwait_untrusted_events_ocall(waiter, self);
}

sgx_status_t sgx_report_attestation_status(const sgx_platform_info_t *p_platform_info, int attestation_status, sgx_update_info_bit_t *p_update_info)
{
	if ( l_sgx_report_attestation_status == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_report_attestation_status)= _load_symbol(h_libsgx_uae_service, "sgx_report_attestation_status", &l_sgx_report_attestation_status);
	}

	if ( l_sgx_report_attestation_status == -1 ) _undefined_symbol("sgx_report_attestation_status");

	return (sgx_status_t) p_sgx_report_attestation_status(p_platform_info, attestation_status, p_update_info);
}

int sgx_thread_set_multiple_untrusted_events_ocall(const void **waiters, size_t total)
{
	if ( l_sgx_thread_set_multiple_untrusted_events_ocall == 0 ) {
		if ( h_libsgx_urts == 0 ) _load_libsgx_urts();
		*(void **)(&p_sgx_thread_set_multiple_untrusted_events_ocall)= _load_symbol(h_libsgx_urts, "sgx_thread_set_multiple_untrusted_events_ocall", &l_sgx_thread_set_multiple_untrusted_events_ocall);
	}

	if ( l_sgx_thread_set_multiple_untrusted_events_ocall == -1 ) _undefined_symbol("sgx_thread_set_multiple_untrusted_events_ocall");

	return (int) p_sgx_thread_set_multiple_untrusted_events_ocall(waiters, total);
}

int invoke_service_ocall(uint8_t *pse_message_req, uint32_t pse_message_req_size, uint8_t *pse_message_resp, uint32_t pse_message_resp_size, uint32_t timeout)
{
	if ( l_invoke_service_ocall == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_invoke_service_ocall)= _load_symbol(h_libsgx_uae_service, "invoke_service_ocall", &l_invoke_service_ocall);
	}

	if ( l_invoke_service_ocall == -1 ) _undefined_symbol("invoke_service_ocall");

	return (int) p_invoke_service_ocall(pse_message_req, pse_message_req_size, pse_message_resp, pse_message_resp_size, timeout);
}

sgx_status_t sgx_get_quote(const sgx_report_t *p_report, sgx_quote_sign_type_t quote_type, const sgx_spid_t *p_spid, const sgx_quote_nonce_t *p_nonce, const uint8_t *p_sig_rl, uint32_t sig_rl_size, sgx_report_t *p_qe_report, sgx_quote_t *p_quote, uint32_t quote_size)
{
	if ( l_sgx_get_quote == 0 ) {
		if ( h_libsgx_uae_service == 0 ) _load_libsgx_uae_service();
		*(void **)(&p_sgx_get_quote)= _load_symbol(h_libsgx_uae_service, "sgx_get_quote", &l_sgx_get_quote);
	}

	if ( l_sgx_get_quote == -1 ) _undefined_symbol("sgx_get_quote");

	return (sgx_status_t) p_sgx_get_quote(p_report, quote_type, p_spid, p_nonce, p_sig_rl, sig_rl_size, p_qe_report, p_quote, quote_size);
}
