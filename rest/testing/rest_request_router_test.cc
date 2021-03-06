/** rest_request_router_test.cc
    Jeremy Barnes, 31 March 2015
    Copyright (c) 2015 mldb.ai inc.  All rights reserved.
    This file is part of MLDB. Copyright 2015 mldb.ai inc. All rights reserved.

*/

#define BOOST_TEST_MAIN
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include "mldb/rest/rest_request_router.h"
#include "mldb/rest/in_process_rest_connection.h"


using namespace std;
using namespace MLDB;


BOOST_AUTO_TEST_CASE( test_header_matching )
{
    RestRequestRouter router;

    string callDone;

    auto asyncCallback = [&] (RestConnection & connection,
                              const RestRequest & request,
                              RestRequestParsingContext & context)
        {
            connection.sendResponse(200, "async", "text/plain");
            return RestRequestRouter::MR_YES;
        };

    auto syncCallback = [&] (RestConnection & connection,
                              const RestRequest & request,
                              RestRequestParsingContext & context)
        {
            connection.sendResponse(200, "sync", "text/plain");
            return RestRequestRouter::MR_YES;
        };

 
    router.addRoute("/test", { "GET", "header:async=true" },
                    "Async route", asyncCallback,
                    Json::Value());

    router.addRoute("/test", { "GET" },
                    "Sync route", syncCallback,
                    Json::Value());

    RestRequest request;
    request.verb = "GET";
    request.resource = "/test";
    request.header.headers["async"] = "true";

    auto conn = InProcessRestConnection::create();

    router.handleRequest(*conn, request);
    conn->waitForResponse();
    
    BOOST_CHECK_EQUAL(conn->response(), "async");

    request.header.headers.erase("async");

    cerr << "request " << request << endl;

    auto conn2 = InProcessRestConnection::create();

    router.handleRequest(*conn2, request);
    conn->waitForResponse();
    
    BOOST_CHECK_EQUAL(conn2->response(), "sync");
}

BOOST_AUTO_TEST_CASE( test_hidden_route )
{
    RestRequestRouter router;

    string callDone;

    auto callback = [&] (RestConnection & connection,
                              const RestRequest & request,
                              RestRequestParsingContext & context)
        {
            connection.sendResponse(200, "", "text/plain");
            return RestRequestRouter::MR_YES;
        };

    router.addRoute("/test", { "GET", "POST" },
                    "Rest route", callback,
                    Json::Value());

    BOOST_CHECK_THROW( router.addRoute("/test", { "GET", "header:async=true" },
                                       "With header", callback,
                                       Json::Value() ), AnnotatedException);

    BOOST_CHECK_THROW( router.addRoute("/test", { "GET", "query:async=true" },
                                       "With query param", callback,
                                       Json::Value() ), AnnotatedException);

    BOOST_CHECK_THROW( router.addRoute("/test", { "GET" },
                                       "With verb subset", callback,
                                       Json::Value() ), AnnotatedException);

    BOOST_CHECK_THROW( router.addRoute("/test", { "GET", "POST" },
                                       "Matching exactly", callback,
                                       Json::Value() ), AnnotatedException);

    router.addRoute(Rx("/test/([a-z]*)", ""), { "GET", "POST" },
                    "Regex test route", callback,
                    Json::Value());

    BOOST_CHECK_THROW( router.addRoute("/test/dummy", { "GET", "POST" },
                                       "Matching regex", callback,
                                       Json::Value() ), AnnotatedException);

    router.addRoute("/test/dummy/", { "GET", "POST" },
                                       "Not matching regex", callback,
                    Json::Value());
}
