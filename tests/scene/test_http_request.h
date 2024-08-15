/**************************************************************************/
/*  test_http_request.h                                                   */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#ifndef TEST_HTTP_REQUEST_H
#define TEST_HTTP_REQUEST_H

#include "scene/main/http_request.h"
#include "tests/core/io/test_http_client_mock.h"
#include "tests/test_macros.h"

#include "thirdparty/cpp_mock/cpp_mock.h"

namespace TestHTTPRequest {

static inline Array build_array() {
	return Array();
}
template <typename... Targs>
static inline Array build_array(Variant item, Targs... Fargs) {
	Array a = build_array(Fargs...);
	a.push_front(item);
	return a;
}

static inline PackedStringArray build_headers() {
	return PackedStringArray();
}
template <typename... Targs>
static inline PackedStringArray build_headers(Variant item, Targs... Fargs) {
	PackedStringArray psa = build_headers(Fargs...);
	psa.push_back(item);
	return psa;
}

TEST_CASE("[Network][HTTPRequest] Download chunk size is set when HTTP client is disconnected") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;
	int expected_value = 42;

	When(http_client->get_status).Return(HTTPClient::STATUS_DISCONNECTED);

	http_request->set_download_chunk_size(expected_value);

	Verify(http_client->set_read_chunk_size).With(expected_value);

	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest] Download chunk size is not set when HTTP client is not disconnected") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;
	int expected_value = 42;

	When(http_client->get_status).Return(HTTPClient::STATUS_CONNECTED);

	ERR_PRINT_OFF;
	http_request->set_download_chunk_size(expected_value);
	ERR_PRINT_ON;

	Verify(http_client->set_read_chunk_size).With(expected_value).Times(0);

	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest] ERR_UNCONFIGURED when HTTPRequest is not inside tree") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);

	String url = "http://foo.com";
	ERR_PRINT_OFF;
	Error error = http_request->request(url);
	ERR_PRINT_ON;

	CHECK(error == Error::ERR_UNCONFIGURED);

	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest][SceneTree] Request when disconnected") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	SceneTree::get_singleton()->get_root()->add_child(http_request);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;

	When(http_client->get_status).Return(HTTPClient::STATUS_DISCONNECTED);
	SIGNAL_WATCH(http_request, "request_completed");

	String url = "http://foo.com";
	Error error = http_request->request(url);

	SceneTree::get_singleton()->process(0);

	CHECK(http_request->is_processing_internal() == false);
	CHECK(error == Error::OK);
	SIGNAL_CHECK("request_completed", build_array(build_array(HTTPRequest::Result::RESULT_CANT_CONNECT, 0, PackedStringArray(), PackedByteArray())));

	SIGNAL_UNWATCH(http_request, "request_completed");
	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest][SceneTree] GET Request") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	SceneTree::get_singleton()->get_root()->add_child(http_request);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;

	When(http_client->get_status).Return({ HTTPClient::STATUS_RESOLVING, HTTPClient::STATUS_CONNECTING,
			// First STATUS_CONNECTED is to send the request, second STATUS_CONNECTED is to receive request
			HTTPClient::STATUS_CONNECTED, HTTPClient::STATUS_CONNECTED });
	When(http_client->get_response_code).Return(HTTPClient::ResponseCode::RESPONSE_OK);
	When(http_client->has_response).Return(true);
	SIGNAL_WATCH(http_request, "request_completed");

	String url = "http://foo.com";
	Error error = http_request->request(url);

	// Call process for each status
	for (int i = 0; i < 4; i++) {
		SceneTree::get_singleton()->process(0);
	}

	Verify(http_client->request).With(HTTPClient::Method::METHOD_GET, String("/"), build_headers("Accept-Encoding: gzip, deflate"), (uint8_t *)nullptr, 0).Times(1);
	SIGNAL_CHECK("request_completed", build_array(build_array(HTTPRequest::Result::RESULT_SUCCESS, HTTPClient::ResponseCode::RESPONSE_OK, PackedStringArray(), PackedByteArray())));
	CHECK_FALSE(http_request->is_processing_internal());
	CHECK(error == Error::OK);

	SIGNAL_UNWATCH(http_request, "request_completed");
	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest][SceneTree] GET Request with body and headers") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	SceneTree::get_singleton()->get_root()->add_child(http_request);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;
	PackedByteArray body = String("Godot Rules!!!").to_utf8_buffer();

	When(http_client->get_status).Return({ HTTPClient::STATUS_RESOLVING, HTTPClient::STATUS_CONNECTING, HTTPClient::STATUS_CONNECTED, HTTPClient::STATUS_BODY });
	When(http_client->get_response_code).Return(HTTPClient::ResponseCode::RESPONSE_OK);
	When(http_client->has_response).Return(true);
	When(http_client->get_response_headers).Do([](List<String> *r_response) -> Error {
		r_response->push_front("Server: Mock");
		return Error::OK;
	});
	When(http_client->get_response_body_length).Return(body.size());
	When(http_client->read_response_body_chunk).Return(body);
	SIGNAL_WATCH(http_request, "request_completed");

	String url = "http://foo.com";
	Error error = http_request->request(url);

	// Call process for each status
	for (int i = 0; i < 4; i++) {
		SceneTree::get_singleton()->process(0);
	}

	Verify(http_client->request).With(HTTPClient::Method::METHOD_GET, String("/"), build_headers("Accept-Encoding: gzip, deflate"), (uint8_t *)nullptr, 0).Times(1);
	SIGNAL_CHECK("request_completed",
			build_array(build_array(HTTPRequest::Result::RESULT_SUCCESS, HTTPClient::ResponseCode::RESPONSE_OK, build_headers("Server: Mock"), body)));
	CHECK_FALSE(http_request->is_processing_internal());
	CHECK(error == Error::OK);

	SIGNAL_UNWATCH(http_request, "request_completed");
	memdelete(http_request);
	HTTPClientMock::reset_current();
}

TEST_CASE("[Network][HTTPRequest][SceneTree] POST Request with body and headers") {
	HTTPClientMock::make_current();
	HTTPRequest *http_request = memnew(HTTPRequest);
	SceneTree::get_singleton()->get_root()->add_child(http_request);
	HTTPClientMock *http_client = HTTPClientMock::current_instance;
	String body("Godot Rules!!!");

	When(http_client->get_status).Return({ HTTPClient::STATUS_RESOLVING, HTTPClient::STATUS_CONNECTING,
			// First STATUS_CONNECTED is to send the request, second STATUS_CONNECTED is to receive request
			HTTPClient::STATUS_CONNECTED, HTTPClient::STATUS_CONNECTED });
	When(http_client->get_response_code).Return(HTTPClient::ResponseCode::RESPONSE_CREATED);
	When(http_client->has_response).Return(true);
	SIGNAL_WATCH(http_request, "request_completed");

	String url = "http://foo.com";
	Error error = http_request->request(url, build_headers("Accept: text/json"), HTTPClient::Method::METHOD_POST, body);

	// Call process for each status
	for (int i = 0; i < 4; i++) {
		SceneTree::get_singleton()->process(0);
	}

	Verify(http_client->request).With(HTTPClient::Method::METHOD_POST, String("/"), build_headers("Accept-Encoding: gzip, deflate", "Accept: text/json"), cpp_mock::matching::any_matcher(), body.size() - 1).Times(1);
	SIGNAL_CHECK("request_completed", build_array(build_array(HTTPRequest::Result::RESULT_SUCCESS, HTTPClient::ResponseCode::RESPONSE_CREATED, PackedStringArray(), PackedByteArray())));
	CHECK_FALSE(http_request->is_processing_internal());
	CHECK(error == Error::OK);

	SIGNAL_UNWATCH(http_request, "request_completed");
	memdelete(http_request);
	HTTPClientMock::reset_current();
}

} // namespace TestHTTPRequest

#endif // TEST_HTTP_REQUEST_H
