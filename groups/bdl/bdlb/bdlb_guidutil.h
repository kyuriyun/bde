// bdlb_guidutil.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLB_GUIDUTIL
#define INCLUDED_BDLB_GUIDUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functions that produce Universally Unique Identifiers.
//
//@CLASSES:
//  bdlb::GuidUtil: namespace for methods for creating UUIDs.
//
//@SEE ALSO: bdlb::Guid
//
//@AUTHOR: Mickey Sweatt (msweatt1)
//
//@DESCRIPTION: This component provides a struct, 'bdlb::GuidUtil', that
// serves as a namespace for utility functions that create and work with
// Globally Unique Identifiers (GUIDs).
//
///Grammar for GUIDs Used in 'GuidFromString'
// ------------------------------------------
// This conversion performed by 'GuidFromString' is intended to be used for
// GUIDs generated by external sources that have a variety of formats.
//
///GUID String Format
// ------------------
// <SPEC>             ::=  <BRACED GUID>     |   <GUID>
//
// <BRACED GUID>      ::=  '[' <GUID> ']'    |   '[ ' <GUID> ' ]'
//                         '{' <GUID> '}'    |   '{ ' <GUID> ' }'
//
// <GUID>             ::=  <FORMATED GUID>   |   <UNFORMATTED GUID>
//
// <FORMATED GUID>    ::=  <X>{4} '-' <X>{2} '-' <X>{2} '-' <X>{2} '-' <X>{6}
//
// <UNFORMATTED GUID> ::=  <X>{16}
//
// <X>                ::=  [0123456789ABCDEFabcdef]{2}
//
// EXAMPLES:
// ---------
// { 87654321-AAAA-BBBB-CCCC-012345654321 }
// 00010203-0405-0607-0809-101112131415
// [00112233445566778899aAbBcCdDeEfF]
//
///Usage
///-----
// Suppose we are building a system for managing records for employees in a
// large international firm.  These records have no natural field which can be
// used as a unique ID, so a GUID must be created for each employee.
//
// First let us define a value-type for employees.
//..
//  class MyEmployee {
//      // This class provides a value-semantic type to represent an employee
//      // record.  These records are for internal use only.
//..
// For the sake of brevity, we provide a limited amount of data in each record.
// We additionally show a very limited scope of functionality.
//..
//      // DATA
//      bsl::string  d_name;    // name of the employee
//      double       d_salary;  // salary in some common currency
//      bdlb::Guid d_guid;    // a GUID for the employee
//
//    public:
//      // CREATORS
//      MyEmployee(const string& name, double salary);
//          // Create an object with the specified 'name' and specified
//          //'salary', generating a new GUID to represent the employee.
//
//      // ...
//
//      // ACCESORS
//      const bdlb::Guid& Guid() const;
//          // Return the 'guid' of this object.
//
//      const bsl::string& name() const;
//          // Return the 'name' of this object.
//
//      double salary() const;
//          // Return the 'salary' of this object.
//      // ...
//  };
//..
// Next, we create free functions 'operator<' and 'operator==' to allow
// comparison of 'MyEmployee' objects.  We take advantage of the monotonically
// increasing nature of sequential GUIDs to implement these methods.
//..
//
//  bool operator== (const MyEmployee& lhs, const MyEmployee& rhs);
//      // Return 'true' if the specified 'lhs' object has the same value as
//      // the specified 'rhs' object, and 'false' otherwise.  Note that two
//      // 'MyEmployee' objects have the same value if they have the same
//      // guid.
//
//  bool operator< (const MyEmployee& lhs, const MyEmployee& rhs);
//      // Return 'true' if the value of the specified 'lhs' MyEmployee object
//      // is less than the value of the specified 'rhs' MyEmployee object,
//      // and 'false' otherwise.  A MyEmployee object is less than another if
//      // the guid is less than the other.
// ...
//
//  // CREATORS
//  MyEmployee::MyEmployee(const string& name, double salary)
//  : d_name(name)
//  , d_salary(salary)
//  {
//      bdlb::GuidUtil::generate(&d_guid);
//  }
//
//  // ACCESORS
//  const bdlb::Guid& MyEmployee::Guid() const
//  {
//      return d_guid;
//  }
//
//  const bsl::string& MyEmployee::name() const
//  {
//      return d_name;
//  }
//
//  double MyEmployee::salary() const
//  {
//      return d_salary;
//  }
//
//  // FREE FUNCTIONS
//  bool operator==(const MyEmployee& lhs, const MyEmployee& rhs)
//  {
//      return lhs.Guid() == rhs.Guid();
//  }
//
//  bool operator<(const MyEmployee& lhs, const MyEmployee& rhs)
//  {
//       return lhs.Guid() < rhs.Guid();
//  }
//..
// Next, we create some employees:
//..
//      MyEmployee e1("Foo Bar"    , 1011970);
//      MyEmployee e2("John Doe"   , 12345);
//      MyEmployee e3("Joe Six-pack", 1);
//..
// Finally, we verify that the generated GUIDs are unique.
//..
//      assert(e1 < e2 || e2 < e1);
//      assert(e2 < e3 || e3 < e2);
//      assert(e1 < e3 || e3 < e1);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLB_GUID
#include <bdlb_guid.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlb {

                              // ===============
                              // struct GuidUtil
                              // ===============

struct GuidUtil {
    // This 'struct' provides a namespace for functions that create universally
    // unique Identifiers.

    // CLASS METHODS
    static void generate(Guid *result, bsl::size_t numGuids = 1);
        // Generate the optionally specified 'numGuids' (default 1) GUIDs, and
        // load the resulting GUIDs into the array referenced by the specified
        // 'result' pointer.

    static Guid generate();
        // Generate and return a single GUID.

    static int guidFromString(Guid *result, bslstl::StringRef guidString);
        // Parse the specified 'guidString' (in {GUID String Format}) and load
        // its value into the specified 'result'.  Return 0 if 'result'
        // successfully loaded, and non-zero otherwise.

    static Guid guidFromString(bslstl::StringRef guidString);
        // Parse the specified 'guidString' (in {GUID String Format}) and
        // return the converted GUID, or a default-constructed Guid if the
        // string is improperly formatted.

    static void guidToString(bsl::string *result, const Guid& guid);
        // Serialize the specified 'guid' into the specified 'result'.  The
        // 'result' string will be in a format suitable for 'guidFromString'.

    static bsl::string guidToString(const Guid& guid);
        // Convert the specified 'guid' into a string suitable for
        // 'guidFromString', and return the string.

    static int getVersion(const bdlb::Guid& guid);
        // Return the version of the specified 'guid' object.  The behavior is
        // undefined unless the contents of the 'guid' object are compliant
        // with RFC 4122.

    static bsls::Types::Uint64 getMostSignificantBits(const Guid& guid);
        // Return the most significant 8 bytes of the specified 'guid'.

    static bsls::Types::Uint64 getLeastSignificantBits(const Guid & guid);
        // Return the least significant 8 bytes of the specified 'guid'.
};

// ============================================================================
//                      INLINE DEFINITIONS
// ============================================================================

                              // ---------------
                              // struct GuidUtil
                              // ---------------
// CLASS METHODS
inline
int GuidUtil::getVersion(const Guid& guid)
{
    return (guid[6] & 0xF0) >> 4;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2014
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
