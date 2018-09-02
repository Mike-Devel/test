//  (C) Copyright Raffi Enficiaud 2018.
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/test for the library home page.
//
//  Checks boost::exception, see https://github.com/boostorg/test/issues/147
// ***************************************************************************

// Boost.Test
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include <boost/test/tools/output_test_stream.hpp>

// BOOST
#include <boost/exception/exception.hpp>
#include <boost/exception/info.hpp>


// STL
#include <iostream>
#include <ios>

using boost::test_tools::output_test_stream;
using namespace boost::unit_test;

typedef boost::error_info<struct tag_error_code, uint32_t> error_code;
struct myexception : std::exception, virtual boost::exception
{};

// this one should generate a message as it does not execute any assertion
void exception_raised()
{
    BOOST_THROW_EXCEPTION( myexception() << error_code(123) );
}

struct output_test_stream2 : public output_test_stream {
  std::string
  get_stream_string_representation() const {
      return output_test_stream::get_stream_string_representation();
  }
};


BOOST_AUTO_TEST_CASE( test_logs )
{
#define PATTERN_FILE_NAME "log-formatter-test.pattern"
#if 0
    std::string pattern_file_name(
        framework::master_test_suite().argc <= 1
            ? (runtime_config::save_pattern() ? PATTERN_FILE_NAME : "./baseline-outputs/" PATTERN_FILE_NAME )
            : framework::master_test_suite().argv[1] );
#endif

    output_test_stream2 test_output;/*( pattern_file_name,
                                    !runtime_config::save_pattern(),
                                    true,
                                    __FILE__ );*/

    test_suite* ts_main = BOOST_TEST_SUITE( "fake master" );
    ts_main->add( BOOST_TEST_CASE( exception_raised ) );

    ts_main->p_default_status.value = test_unit::RS_ENABLED;
    boost::unit_test::unit_test_log.set_stream(test_output);

    framework::finalize_setup_phase( ts_main->p_id );
    framework::run( ts_main->p_id, false ); // do not continue the test tree to have the test_log_start/end

    boost::unit_test::unit_test_log.set_stream(std::cout);

    test_output.flush();
    std::string error_string(test_output.get_stream_string_representation());
    std::cout << "blablabla " << error_string << std::endl;
    BOOST_TEST( error_string.find("[tag_error_code*] = 123") != std::string::npos );
}
